#include "stdafx.h"
#include "Networking.h"

#include "Globals.h"
#include "Memory\bitstream.h"
#include "CustomPackets\CustomPackets.h"
#include "XLive\UserManagement\CUser.h"
#include "H2MOD/Modules/Config/Config.h"
#include "H2MOD\Modules\MapChecksum\MapChecksumSync.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "Util/base64.h"

extern SOCKET game_network_data_gateway_socket_1000;
extern SOCKET game_network_message_gateway_socket_1001;
extern int __cdecl QoSLookUpImpl(int a1, signed int a2, int a3, int a4);

CustomNetwork *network = new CustomNetwork;

const char* getTextForEnum(int enumVal) {
	return packet_type_strings[enumVal];
}

void __cdecl request_write(void* a1, int a2, int a3) {
	bitstream::p_data_encode_integer()(a1, "identifier", *(DWORD *)a3, 32);
	bitstream::p_data_encode_integer()(a1, "flags", *(DWORD *)(a3 + 4), 8);
	TRACE_GAME_NETWORK_N("[H2MOD-network] connection request write, identifier=%d, flags=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4));
}

bool __cdecl request_read(void* a1, int a2, int a3) {
	*(DWORD *)a3 = bitstream::p_data_decode_integer()(a1, "identifier", 32);
	*(DWORD *)(a3 + 4) = bitstream::p_data_decode_integer()(a1, "flags", 8);
	return bitstream::p_packet_is_valid()(a1) == 0;
}

void __cdecl refuse_write(void* a1, int a2, int a3) {
	bitstream::p_data_encode_integer()(a1, "remote-identifier", *(DWORD *)a3, 32);
	bitstream::p_data_encode_integer()(a1, "reason", *(DWORD *)(a3 + 4), 3);
}

bool __cdecl refuse_read(void* a1, int a2, int a3) {
	*(DWORD *)a3 = bitstream::p_data_decode_integer()(a1, "remote-identifier", 32);
	*(DWORD *)(a3 + 4) = bitstream::p_data_decode_integer()(a1, "reason", 3);
	bool isValid = bitstream::p_packet_is_valid()(a1) == 0;
	//TRACE_GAME_NETWORK_N("[H2MOD-network] connection refuse read, remote-identifier=%d, reason=%d, isValid=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), isValid);
	return isValid;
}

void __cdecl establish_write(void* a1, int a2, int a3) {
	userManager.sendSecurePacket(game_network_data_gateway_socket_1000, 1000);
	bitstream::p_data_encode_integer()(a1, "remote-identifier", *(DWORD *)a3, 32);
	bitstream::p_data_encode_integer()(a1, "reason", *(DWORD *)(a3 + 4), 32);
	//TRACE_GAME_NETWORK_N("[H2MOD-network] connection establish write, remote-identifier=%d, identifier=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4));
}

bool __cdecl establish_read(void* a1, int a2, int a3) {
	*(DWORD *)a3 = bitstream::p_data_decode_integer()(a1, "remote-identifier", 32);
	*(DWORD *)(a3 + 4) = bitstream::p_data_decode_integer()(a1, "identifier", 32);
	bool isValid = bitstream::p_packet_is_valid()(a1) == 0;
	//TRACE_GAME_NETWORK_N("[H2MOD-network] connection establish read, remote-identifier=%d, identifier=%d, isValid=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), isValid);
	return isValid;
}

void __cdecl closed_write(void* a1, int a2, int a3) {
	memset(&userManager.game_host_xn, NULL, sizeof(XNADDR));
	bitstream::p_data_encode_integer()(a1, "remote-identifier", *(DWORD *)a3, 32);
	bitstream::p_data_encode_integer()(a1, "identifier", *(DWORD *)(a3 + 4), 32);
	bitstream::p_data_encode_integer()(a1, "closure-reason", *(DWORD *)(a3 + 8), 5);
	//TRACE_GAME_NETWORK_N("[H2MOD-network] connection closed write, remote-identifier=%d, identifier=%d, closureReason=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), *(DWORD *)(a3 + 8));
}

bool __cdecl closed_read(void* a1, int a2, int a3) {
	signed int v3; // edi@2
	bool result; // al@4

	*(DWORD *)a3 = bitstream::p_data_decode_integer()(a1, "remote-identifier", 32);
	*(DWORD *)(a3 + 4) = bitstream::p_data_decode_integer()(a1, "identifier", 32);
	*(DWORD *)(a3 + 8) = bitstream::p_data_decode_integer()(a1, "closure-reason", 5);
	result = false;
	bool isValid = bitstream::p_packet_is_valid()(a1);
	if (!isValid)
	{
		v3 = *(DWORD *)(a3 + 8);
		if (v3 >= 0 && v3 < 18)
			result = true;
	}
	//TRACE_GAME_NETWORK_N("[H2MOD-network] connection closed read, remote-identifier=%d, reason=%d, closureReason=%d, isValid=%d, result=%d",
	//	*(DWORD *)a3, *(DWORD *)(a3 + 4), *(DWORD *)(a3 + 8), isValid, result);
	return result;
}

bool __cdecl deserializePlayerAdd(void* a1, int a2, int a3) {
	typedef bool(__cdecl* deserialize_player_add)(void* a1, int a2, int a3);
	auto p_deserialize_player_add = reinterpret_cast<deserialize_player_add>(h2mod->GetBase() + (h2mod->Server ? 0x1D110B : 0x1F0752));

	//inform new players of the current map info
	// send server map checksums to client
	//MapChecksumSync::SendState();
	return p_deserialize_player_add(a1, a2, a3);
}

typedef void(__cdecl *register_connection_packets)(void* a1);
register_connection_packets register_connection_packets_method;

void __cdecl registerConnectionPackets(void* packetObject) {
	register_packet_impl(packetObject, 4, "connect-request", 0, 8, 8, request_write, request_read, 0); //request
	register_packet_impl(packetObject, 5, "connect-refuse", 0, 8, 8, refuse_write, refuse_read, 0); //refuse
	register_packet_impl(packetObject, 6, "connect-establish", 0, 8, 8, establish_write, establish_read, 0); //establish
	register_packet_impl(packetObject, 7, "connect-closed", 0, 12, 12, closed_write, closed_read, 0); //closed
}

typedef void(__cdecl *register_player_packets)(void* a1);
register_player_packets register_player_packets_method;

void __cdecl registerPlayerPackets(void* packetObject) {

	register_packet_impl(packetObject, 25, "membership-update", 0, 15784, 15784,
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D0072 : 0x1EF6B9)),
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D0496 : 0x1EFADD)), 0);

	register_packet_impl(packetObject, 37, "virtual-couch-update", 0, 216, 216,
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D09FB : 0x1F0042)),
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D0AFC : 0x1F0143)), 0);

	register_packet_impl(packetObject, 38, "virtual-couch-request", 0, 208, 208,
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D0C1D : 0x1F0264)),
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D0CD6 : 0x1F031D)), 0);

	register_packet_impl(packetObject, 26, "peer-properties", 0, 208, 208,
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D0DAE : 0x1F03F5)),
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D0E99 : 0x1F04E0)), 0);

	register_packet_impl(packetObject, 27, "delegate-leadership", 0, 44, 44,
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D0FA7 : 0x1F05EE)),
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D0FD3 : 0x1F061A)), 0);

	register_packet_impl(packetObject, 28, "boot-machine", 0, 44, 44,
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D100B : 0x1F0652)),
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D1037 : 0x1F067E)), 0);

	register_packet_impl(packetObject, 29, "player-add", 0, 168, 168,
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D106F : 0x1F06B6)),
		(void*)deserializePlayerAdd, 0);

	register_packet_impl(packetObject, 30, "player-refuse", 0, 20, 20,
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D11D8 : 0x1F081F)),
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D1218 : 0x1F085F)), 0);

	register_packet_impl(packetObject, 31, "player-remove", 0, 12, 12,
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D1275 : 0x1F08BC)),
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D12A3 : 0x1F08EA)), 0);

	register_packet_impl(packetObject, 32, "player-properties", 0, 156, 156,
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D12EE : 0x1F0935)),
		(void*)(h2mod->GetBase() + (h2mod->Server ? 0x1D1365 : 0x1F09AC)), 0);
}

typedef int(__stdcall *send_packet)(void* thisx, void *a2, unsigned int type, unsigned int size, int packet_data_obj);
send_packet send_packet_method;

int __stdcall sendPacket(void* thisx, void *a2, unsigned int type, unsigned int size, int packet_data_obj) {
	TRACE_GAME_NETWORK_N("[h2mod-network] send packet type=%d, typeName=%s, size=%d", type, getTextForEnum(type), size);
	return send_packet_method(thisx, a2, type, size, packet_data_obj);
}

bool decodePacketTypeAndSize(void *thisx, void* a2, signed int *a3, int a4) {
	char *v4; // ebp@1
	signed int v5; // eax@2
	int v6; // eax@4
	bool result; // al@7

	v4 = (char *)thisx;
	*a3 = bitstream::p_data_decode_integer()(a2, "type", 8);
	*(DWORD *)a4 = bitstream::p_data_decode_integer()(a2, "size", 16);
	TRACE_GAME_NETWORK_N("[h2mod-network] received packet decoded type=%d, typeName=%s, size=%d", *a3, getTextForEnum(*a3), *(DWORD *)a4);
	if (bitstream::p_packet_is_valid()(a2)
		|| (v5 = *a3, *a3 < 0)
		|| v5 >= 49
		|| (v6 = (int)&v4[32 * v5], !*(BYTE *)v6)
		|| *(DWORD *)a4 < *(DWORD *)(v6 + 12)
		|| *(DWORD *)a4 > *(DWORD *)(v6 + 16))
	{
		result = 0;
	}
	else
	{
		result = bitstream::p_packet_is_valid()(a2) == 0;
	}
	return result;
}

typedef char(__stdcall *receive_packet)(void *thisx, void* a2, int packetType, unsigned int *size, void *packet_obj);
receive_packet receive_packet_method;

char __stdcall receivePacket(void *thisx, void* a2, int packetType, unsigned int *size, void *packet_obj) {
	char *v5; // ebp@1
	int v6; // esi@2
	char result; // al@2
	TRACE_GAME_NETWORK_N("[h2mod-network] received packet");
	v5 = (char *)thisx;
	typedef bool(__thiscall *decode_type_and_size)(void* thisx, int a2, signed int* a3, int a4);
	decode_type_and_size decode_type_and_size_method = (decode_type_and_size)(h2mod->GetBase() + (h2mod->Server ? 0x1CA1DA : 0x1E8217));
	if (decodePacketTypeAndSize(thisx, a2, (signed int *)packetType, (int)size))
	{
		TRACE_GAME_NETWORK_N("[h2mod-network] received packet succesfully decoded");
		v6 = (int)&v5[32 * *(DWORD *)packetType];
		memset(packet_obj, 0, *size);
		result = (*(int(__cdecl **)(void*, unsigned int, void *))(v6 + 24))(a2, *size, packet_obj);// calls packet read/write method
	}
	else
	{
		TRACE_GAME_NETWORK_N("[h2mod-network] received packet was invalid");
		result = 0;
	}
	return result;
}

typedef void(__stdcall *receive_data_from_socket)(DWORD* thisx);
receive_data_from_socket receive_data_from_socket_method;

void __stdcall receiveDataFromSocket(DWORD* thisx) {
	DWORD *v1; // ebp@1
	char v2; // bl@2
	signed int v3; // edi@2
	DWORD *v4; // ebp@2
	int v5; // esi@5
	DWORD *v6; // [sp+10h] [bp-1E4Ch]@1
	unsigned int a1; // [sp+14h] [bp-1E48h]@0
	int v8; // [sp+18h] [bp-1E44h]@0
	int v9; // [sp+1Ch] [bp-1E40h]@6
//	int v10; // [sp+20h] [bp-1E3Ch]@11
//	int v11; // [sp+24h] [bp-1E38h]@11
//	int v12; // [sp+28h] [bp-1E34h]@11
//	int v13; // [sp+2Ch] [bp-1E30h]@11
	int a3; // [sp+30h] [bp-1E2Ch]@11
	int v15; // [sp+38h] [bp-1E24h]@11
//	int v16; // [sp+3Ch] [bp-1E20h]@11
//	int v17; // [sp+40h] [bp-1E1Ch]@11
//	int v18; // [sp+44h] [bp-1E18h]@11
//	int v19; // [sp+48h] [bp-1E14h]@11
	char buf[4096]; // [sp+E58h] [bp-1004h]@5
	char *ptr = buf;

	typedef int(__cdecl *h2_calls_socketrecvfrom_type)(int a1, char* buf, int len);
	h2_calls_socketrecvfrom_type h2_calls_socketrecvfrom = (h2_calls_socketrecvfrom_type)(h2mod->GetBase() + 0x1F565D);

	typedef bool(__cdecl *sub_1261E77_type)(int a1);
	sub_1261E77_type sub_1261E77 = (sub_1261E77_type)(h2mod->GetBase() + 0x1F1E77);

	typedef char(__stdcall *sub_122A93B_type)(unsigned int a1, char* buf, int a3);
	sub_122A93B_type sub_122A93B = (sub_122A93B_type)(h2mod->GetBase() + 0x1BA93B);

	typedef int(__thiscall *h2_calls_packet_dispatcher_type)(DWORD* thisx, int a2);
	h2_calls_packet_dispatcher_type h2_calls_packet_dispatcher = (h2_calls_packet_dispatcher_type)(h2mod->GetBase() + 0x1BAFB0);

	v1 = thisx;
	v6 = thisx;
	do
	{
		v2 = 0;
		v3 = 0;
		v4 = v1 + 3;
		while (v3 < 4)
		{
			if (*v4)
			{
				v5 = h2_calls_socketrecvfrom(*v4, ptr, 4096);
				if (v5 > 0 && sub_1261E77((int)&v9))
				{
					TRACE_GAME_NETWORK_N("[h2mod-network] received socket data %s", ptr);
					a1 = v5;
					v8 = v3;
					v2 = 1;
				}
			}
			++v3;
			++v4;
			if (v2)
				goto LABEL_11;
		}
		if (!v2)
			return;
	LABEL_11:
		memset((char *)&a3, 0, 3624);
		v1 = v6;
		a3 = v8;
		v15 = v9;
		//v16 = v10; // avoid warnings.
		//v17 = v11;
		//v18 = v12;
		//v19 = v13;
		if (sub_122A93B(a1, ptr, (int)&a3))
			h2_calls_packet_dispatcher(v6, (int)&a3);
	} while (v2);
}

typedef int(__cdecl *serialize_peer_properties)(void* packetObject, int a2, int a3);
serialize_peer_properties serialize_peer_properties_method;

int __cdecl serializePeerPropertiesPacket(void* packetObject, int a2, int a3) {
	//tell the host you can load the map (even if you can't)
	*(DWORD *)(a3 + 104) = 4;
	*(DWORD *)(a3 + 108) = 100;
	return serialize_peer_properties_method(packetObject, a2, a3);
}

/* This should happen whenever someone quits or joins a game. */
typedef void(__cdecl *serialize_membership_packet)(void* a1, int a2, int a3);
serialize_membership_packet serialize_membership_packet_method;

void __cdecl serializeMembershipPacket(void* a1, int a2, int a3) {
	
	/* 
	//TODO-Issue-34: turn on when ready to enable feature
	int v3 = 0;
	if (*(WORD*)(a3 + 32) > 0)
	{
		int v4 = a3 + 0x26;
		do {
			
			//let the host tell everyone else the game can start
			if (*(BYTE *)(v4 + 140)) {
				TRACE_GAME_NETWORK_N("[h2mod-network] Serialize Original Peer map status - %d", *(DWORD*)(v4 + 142));
				TRACE_GAME_NETWORK_N("[h2mod-network] Serialize Original Peer map progress percentage - %d", *(DWORD *)(v4 + 146));

				*(DWORD *)(v4 + 142) = 4;
				*(DWORD *)(v4 + 146) = 100;
				TRACE_GAME_NETWORK_N("[h2mod-network] Serialize New Peer map status - %d", *(DWORD *)(v4 + 142));
				TRACE_GAME_NETWORK_N("[h2mod-network] Serialize New Peer map progress percentage - %d", *(DWORD *)(v4 + 146));
			}
			++v3;
			v4 += 184;
		} while (v3 < *(WORD*)(a3 + 32));
	}*/
	//advLobbySettings->sendLobbySettingsPacket();
	// send server map checksums to client
	serialize_membership_packet_method(a1, a2, a3);
}


/* This happens when someone joins or quits a game, the membership packet contains various data about the person. */
typedef int(__cdecl *deserialize_membership_packet)(void* a1, int a2, int a3);
deserialize_membership_packet deserialize_membership_packet_method;

int __cdecl deserializeMembershipPacket(void* a1, int a2, int a3) {

	/* Call the original first so the packet is de-serialized into the provided address. */
	int ret = deserialize_membership_packet_method(a1, a2, a3);

	//TODO-Issue-34: need to figure out how to force clients who cannot load the map to correctly fail to load the map
	/*
	int peerCount = *(WORD *)(a3 + 32);
	if (peerCount > 0) {
		int v3 = 0;
		int peerOffset = a3 + 0x26;
		do {
			int peerMapStatus = *(DWORD*)(peerOffset + 142);
			*(DWORD*)(peerOffset + 142) = peerMapStatus;//failedToLoadTheMap ? 1 : peerMapStatus;
			TRACE_GAME_NETWORK_N("[h2mod-network] Deserialize Peer map status - %d", peerMapStatus);
			peerOffset += 184;
			++v3;
		} while (v3 < peerCount);
	}*/

	/* The data that gets de-serialized is stored in a3 */
	/* 0x20 offset = total peers offset, 0x22 = total players */
	WORD player_count = *(WORD*)((BYTE*)a3 + 0x20); 
	
	TRACE_GAME_NETWORK_N("[h2mod-network] deserializeMemberShipPacket player count: %i", player_count);
	
	/*There's an array of players in + 0x26 */
	BYTE *PlayerArray = (BYTE*)((BYTE*)a3 + 0x26);

	/* Loop the array */
	for (int i = 0; i < player_count; i++)
	{
		/*Each player object in the array +0x02 contains 'peer-address' which is XNADDR */
		XNADDR *nXN = (XNADDR*)((BYTE*)PlayerArray + 0x02);

		// disable for now until I figure out if this is right: Nuke
		//userManager.CreateUser(nXN); // Adds all the necessary information about the user so we don't have to request it from master.

		PlayerArray += 0xB8;
	}

	return ret;
}

typedef void(__cdecl *serialize_parameters_update_packet)(void* a1, int a2, int a3);
serialize_parameters_update_packet serialize_parameters_update_packet_method;

void __cdecl serializeParametersUpdatePacket(void* a1, int a2, int a3) {
	serialize_parameters_update_packet_method(a1, a2, a3);
}

typedef void(__stdcall *tjoin_game)(void* thisptr, int a2, int a3, int a4, int a5, XNADDR* host_xn, int a7, int a8, int a9, int a10, int a11, char a12, int a13, int a14);
tjoin_game pjoin_game;

void __stdcall join_game(void* thisptr, int a2, int a3, int a4, int a5, XNADDR* host_xn, int a7, int a8, int a9, int a10, int a11, char a12, int a13, int a14)
{
	memset(&userManager.game_host_xn, NULL, sizeof(XNADDR));
	memcpy(&userManager.game_host_xn, host_xn, sizeof(XNADDR));
	TRACE_GAME_NETWORK_N("[H2MOD-Network] copied host information, XNADDR: %08X", userManager.game_host_xn.ina.s_addr);
	userManager.sendSecurePacket(game_network_message_gateway_socket_1001, 1001);
	userManager.CreateUser(host_xn, FALSE);
	return pjoin_game(thisptr, a2, a3, a4, a5, host_xn, a7, a8, a9, a10, a11, a12, a13, a14);
}

/* WIP */
/* All this does is patch some checks that cause using actual ip addresses not to work. */
/* When a call to XNetXnaddrtoInaddr happens we provide the actual ip address rather than a secure key */
typedef char(__stdcall *cmp_xnkid)(int thisx, int a2);
cmp_xnkid p_cmp_xnkid;
char __stdcall xnkid_cmp(int thisx, int a2)
{
	return 1;
}

void removeXNetSecurity()
{
	DWORD dwBack;
	/* XNKEY bs */
	p_cmp_xnkid = (cmp_xnkid)DetourClassFunc((BYTE*)h2mod->GetBase() + (h2mod->Server ? 0x199F02 : 0x1C284A), (BYTE*)xnkid_cmp, 9);
	VirtualProtect(p_cmp_xnkid, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	BYTE jmp = 0xEB;
	// apparently the secure address has 1 free byte 
	// after HTONL call, game is checking the al register (the lower 8 bits of eax register) if it is zero, if not everything network related will fail
	WriteBytes(h2mod->GetBase() + (h2mod->Server ? 0x1961F8 : 0x1B5DBE), &jmp, 1);
	NopFill<2>(h2mod->GetBase() + (h2mod->Server ? 0x196684 : 0x1B624A));
	NopFill<2>(h2mod->GetBase() + (h2mod->Server ? 0x19663B : 0x1B6201));
	NopFill<2>(h2mod->GetBase() + (h2mod->Server ? 0x1966F4 : 0x1B62BC));
	
}

void applyConnectionPatches()
{
	DWORD dwBack;
	//removeXNetSecurity();

	// force hard-coded qos data in-lobby
	PatchCall(h2mod->GetBase() + (h2mod->Server ? 0x1B7B8A : 0x1BDCB0), QoSLookUpImpl);

	//NopFill<9>(h2mod->GetBase() + (h2mod->Server ? 0x1B3CC3 : 0x1F1F94)); // check if secure/ipaddress != 127.0.0.1
	// disable network observer (broken on H2V)
	//WriteValue<BYTE>(h2mod->GetBase() + (h2mod->Server ? 0x1A92BA : 0x1B555C), (BYTE)0);
	// also ping bars
	//WriteValue<BYTE>(h2mod->GetBase() + (h2mod->Server ? 0x1C1B7F : 0x1D4E35), 0xEB);

	// makes Live network not as laggy 
	//int data = 500;
	//WriteValue<int>(h2mod->GetBase() + (h2mod->Server ? 0x24896 : 0x28702), data);
	
	// research
	static float unk_flt_ = 60.0f;
	DWORD addresses[] = { 0x1BDE27, 0x1BE2FA, 0x1BFB3C, 0x1C11FA, 0x1C12BF };
	DWORD addresses_dedi[] = { 0x1B7D01, 0x1B81D4, 0x1B9A1C, 0x1BB0DA, 0x1BB19F };
	int size = (h2mod->Server ? sizeof(addresses_dedi) : sizeof(addresses)) / sizeof(int);
	for (int i = 0; i < size; i++)
	{
		DWORD addr = h2mod->Server ? addresses_dedi[i] : addresses[i];
		WritePointer(h2mod->GetBase() + addr + 4, &unk_flt_);
	}

	// increase max bits per second of LIVE netcode (3000 bytes -> ~8000 bytes)
	WriteValue<DWORD>((DWORD)h2mod->GetAddress(0x1AAD63 + 6, 0x1AB268 + 6), 61440);

	if (!h2mod->Server)
	{
		pjoin_game = (tjoin_game)DetourClassFunc((BYTE*)h2mod->GetBase() + 0x1CDADE, (BYTE*)join_game, 13);
		VirtualProtect(pjoin_game, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	}
}

void CustomNetwork::applyNetworkHooks() {
	DWORD dwBack;
	DWORD deserializePlayerAddOffset = 0x1F0753;
	DWORD serializeParametersUpdatePacketOffset = 0x1EDC41;

	if (h2mod->Server) {
		deserializePlayerAddOffset = 0x1D110C;
	}

	///////////////////////////////////////////////
	//connection/player packet customizations below
	///////////////////////////////////////////////

	register_connection_packets_method = (register_connection_packets)DetourFunc((BYTE*)h2mod->GetAddress(0x1F1B36, 0x1D24EF), (BYTE*)registerConnectionPackets, 5);
	VirtualProtect(register_connection_packets_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	
	//use for debugging
	//register_player_packets_method = (register_player_packets)DetourFunc((BYTE*)h2mod->GetAddress(0x1F0A55, 0x1D140E), (BYTE*)registerPlayerPackets, 5);
	//VirtualProtect(register_player_packets_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	serialize_membership_packet_method = (serialize_membership_packet)DetourFunc((BYTE*)h2mod->GetAddress(0x1EF6B9, 0x1D0072), (BYTE*)serializeMembershipPacket, 5);
	VirtualProtect(serialize_membership_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	deserialize_membership_packet_method = (deserialize_membership_packet)DetourFunc((BYTE*)h2mod->GetAddress(0x1EFADD, 0x1D0496), (BYTE*)deserializeMembershipPacket, 12);
	VirtualProtect(deserialize_membership_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	//serialize_parameters_update_packet_method = (serialize_parameters_update_packet)DetourFunc((BYTE*)h2mod->GetAddress(0x1F03F5, 0x1CE5FA), (BYTE*)serializeParametersUpdatePacket, 5);
	//VirtualProtect(serialize_parameters_update_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	//TODO-Issue-34: turning this on makes hosts not care if clients can load map or now
	//serialize_peer_properties_method = (serialize_peer_properties)DetourFunc((BYTE*)h2mod->GetBase() + serializePeerPropertiesPacketOffset, (BYTE*)serializePeerPropertiesPacket, 5);
	//VirtualProtect(serialize_peer_properties_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	/////////////////////////////////////////////////////////////////////
	//send/recv packet functions below (for troubleshooting and research)
	/////////////////////////////////////////////////////////////////////
	//send_packet_method = (send_packet)DetourClassFunc((BYTE*)h2mod->GetAddress(0x1E8296, 0x1CA259), (BYTE*)sendPacket, 8);
	//VirtualProtect(send_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	//receive_packet_method = (receive_packet)DetourClassFunc((BYTE*)h2mod->GetAddress(0x1E82E0, 0x1CA2A3), (BYTE*)receivePacket, 11);
	//VirtualProtect(receive_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	//receive_data_from_socket_method = (receive_data_from_socket)DetourClassFunc((BYTE*)h2mod->GetAddress(0x1BAFB5), (BYTE*)receiveDataFromSocket, 10);
	//VirtualProtect(receive_data_from_socket_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	CustomPackets::ApplyGamePatches();
	applyConnectionPatches();
}