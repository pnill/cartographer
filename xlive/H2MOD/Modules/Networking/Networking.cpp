#include "Globals.h"
#include <XLive\UserManagement\CUser.h>
#include "H2MOD/Modules/Config/Config.h"
#include "H2MOD\Modules\MapChecksum\MapChecksumSync.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "Util/base64.h"

//original = 0x3DA8
#define MEMBERSHIP_PACKET_SIZE 0x3DA8
#define MEMBERSHIP_PACKET_SIZE_RAW_BYTES 0xA8, 0x3D
//original = 0x194
#define CHAT_PACKET_ORG_SIZE 0x194
#define CHAT_PACKET_EXTEND_SIZE 0x2000
#define CHAT_PACKET_SIZE CHAT_PACKET_ORG_SIZE + CHAT_PACKET_EXTEND_SIZE

extern unsigned short H2Config_base_port;
extern SOCKET game_network_data_gateway_socket_1000;
extern SOCKET game_network_message_gateway_socket_1001;
extern int __cdecl QoSLookUpImpl(int a1, signed int a2, int a3, int a4);

const char* getTextForEnum(int enumVal) {
	return packet_type_strings[enumVal];
}

typedef void(__thiscall *data_decode_string)(void* thisx, int a2, int a3, int a4);
data_decode_string getDataDecodeStringMethod() {
	return (data_decode_string)(h2mod->GetBase() + (h2mod->Server ? 0xCE561 : 0xD1FA7));
}

typedef int(__thiscall *data_decode_address)(int thisx, int a1, int a2);
data_decode_address getDataDecodeAddressMethod() {
	return (data_decode_address)(h2mod->GetBase() + (h2mod->Server ? 0xCE5B7 : 0xD1FFD));
}

typedef int(__thiscall *data_decode_id)(int thisx, int a1, int a2, int a3);
data_decode_id getDataDecodeId() {
	return (data_decode_id)(h2mod->GetBase() + (h2mod->Server ? 0xCE54F : 0xD1F95));
}

typedef unsigned int(__thiscall *data_decode_integer)(int thisx, int a1, int a2);
data_decode_integer getDataDecodeIntegerMethod() {
	return (data_decode_integer)(h2mod->GetBase() + (h2mod->Server ? 0xCE49F : 0xD1EE5));
}

typedef bool(__thiscall *data_decode_bool)(int thisx, int a2);
data_decode_bool getDataDecodeBoolMethod() {
	return (data_decode_bool)(h2mod->GetBase() + (h2mod->Server ? 0xCE501 : 0xD1F47));
}

typedef int(__thiscall *data_encode_integer)(void *thisx, int name, unsigned int value, signed int a3);
data_encode_integer getDataEncodeIntegerMethod() {
	return (data_encode_integer)(h2mod->GetBase() + (h2mod->Server ? 0xCDD80 : 0xD17C6));
}

typedef int(__thiscall *data_encode_string)(void* thisx, int a2, int a3, int a4);
data_encode_string getDataEncodeStringMethod() {
	return (data_encode_string)(h2mod->GetBase() + (h2mod->Server ? 0xCDE99 : 0xD18DF));
}

typedef int(__thiscall *data_encode_id)(void* thisx, int a1, int a2, int a3);
data_encode_id getDataEncodeIdMethod() {
	return (data_encode_id)(h2mod->GetBase() + (h2mod->Server ? 0xCDE87 : 0xD18CD));
}

typedef BYTE*(__thiscall *data_encode_boolean)(int thisx, int a2, char a3);
data_encode_boolean getDataEncodeBooleanMethod() {
	return (data_encode_boolean)(h2mod->GetBase() + (h2mod->Server ? 0xCDE40 : 0xD1886));
}

// sign flag
int8_t __SETS__(int x)
{
	if (sizeof(int) == 1)
		return int8_t(x) < 0;
	if (sizeof(int) == 2)
		return int16_t(x) < 0;
	if (sizeof(int) == 4)
		return int32_t(x) < 0;
	return int64_t(x) < 0;
}

// overflow flag of subtraction (x-y)
int8_t __OFSUB__(int x, int y)
{
	int y2 = y;
	int8_t sx = __SETS__(x);
	return (sx ^ __SETS__(y2)) & (sx ^ __SETS__(x - y2));
}

typedef unsigned int(__stdcall *trigger_membership_packet)(int thisx);
trigger_membership_packet trigger_membership_packet_method;

unsigned int __stdcall triggerMembershipPacketData(int thisx) {
	//return trigger_membership_packet_method(thisx);

	unsigned int v1; // ebx@1
	int v2; // esi@1
	int v3; // edi@2
	char *v4; // ecx@5
	int v5; // eax@10
	unsigned int v6; // ebx@19
	int v7; // edi@20
	int *v8; // eax@22
	int v10; // [sp+Ch] [bp-7B5Ch]@1
	int v11; // [sp+10h] [bp-7B58h]@1
			 //below are two stack arrays that 
	int a7[MEMBERSHIP_PACKET_SIZE]; // [sp+14h] [bp-7B54h]@18
	char v13[MEMBERSHIP_PACKET_SIZE]; // [sp+3DBCh] [bp-3DACh]@16

	typedef bool(__thiscall *sub_13FD95D_method)(char *thisx, int a2, int a3, unsigned __int8 a4);
	sub_13FD95D_method sub_13FD95D = (sub_13FD95D_method)(h2mod->GetBase() + (h2mod->Server ? 0x1B7837 : 0x1BD95D));
	
	typedef int(__thiscall *sub_13FD9FA_method)(void *thisx, int a2, int a3, unsigned __int8 a4);
	sub_13FD9FA_method sub_13FD9FA = (sub_13FD9FA_method)(h2mod->GetBase() + (h2mod->Server ? 0x1B78D4 : 0x1BD9FA));

	typedef char(__thiscall *dynamic_packet_check_method)(void *thisx, int a2, int a3, char a4, unsigned int type, unsigned int size, int a7);
	dynamic_packet_check_method dynamic_packet_check = (dynamic_packet_check_method)(h2mod->GetBase() + (h2mod->Server ? 0x1B8C1A : 0x1BED40));

	typedef unsigned int(__cdecl *write_to_memory_method)(unsigned int a1, int a2, unsigned int size);
	write_to_memory_method write_to_memory = (write_to_memory_method)(h2mod->GetBase() + (h2mod->Server ? 0x234D60 : 0x287E18));

	//0x1C3800
	typedef int(__thiscall *generate_membership_packet_data)(DWORD* thisx, int a2, int a3, int a4);
	generate_membership_packet_data generate_membership_packet_data_method = (generate_membership_packet_data)(h2mod->GetBase() + (h2mod->Server ? 0x19AEB8 : 0x1C3800));

	v1 = 0;
	v2 = thisx;
	v11 = 0;
	v10 = 0;
	if (*(DWORD *)(thisx + 132) > 0)
	{
		v3 = thisx + 29128;
		do
		{
			if (*(BYTE *)(v3 - 3))
			{
				if (!*(BYTE *)(v3 - 1))
				{
					v4 = *(char **)(v2 + 8);
					//TRACE_GAME_NETWORK_N("CONDITION1, %d", *(BYTE *)(v3 - 1));
					if ((*(DWORD *)&v4[1856 * *(DWORD *)v3 + 128] == 7 && *(DWORD *)(v3 + 4) != *(DWORD *)(v2 + 112)))
					{
						//TRACE_GAME_NETWORK_N("CONDITION2, firstArg=%d, secondArg=%d, thirdArg=%d", *(DWORD *)&v4[1856 * *(DWORD *)v3 + 128], *(DWORD *)(v3 + 4), *(DWORD *)(v2 + 112));
						if (sub_13FD95D(v4, *(DWORD *)(v2 + 20), *(DWORD *)v3, 0x19u))
						{
							//TRACE_GAME_NETWORK_N("CONDITION3");
							sub_13FD9FA(*(void **)(v2 + 8), *(DWORD *)(v2 + 20), *(DWORD *)v3, 0x19u);
						}
						else if (*(DWORD *)(v2 + 29600) != 8)
						{
							//TRACE_GAME_NETWORK_N("CONDITION4, arg=%d", *(DWORD *)(v2 + 29600));
							v5 = *(DWORD *)(v3 + 4);
							if (v5 == -1 || v5 != *(DWORD *)(v2 + 0x24E0)) {
								//TRACE_GAME_NETWORK_N("CONDITION5");
								v10 |= 1 << v1;
							}
							else {
								//TRACE_GAME_NETWORK_N("CONDITION6");
								v11 |= 1 << v1;
							}
						}
					}
				}
			}
			++v1;
			v3 += 28;
		} while (v1 < *(DWORD *)(v2 + 132));
		if (v10) {
			//TRACE_GAME_NETWORK_N("CONDITION7");
			generate_membership_packet_data_method((DWORD *)v2, v2 + 112, 0, (int)&v13);
		}
		if (v11) {
			//TRACE_GAME_NETWORK_N("CONDITION8");
			generate_membership_packet_data_method((DWORD *)v2, v2 + 112, v2 + 9440, (int)&a7);
		}
	}
	v6 = 0;
	if (*(DWORD *)(v2 + 132) > 0)
	{
		v7 = v2 + 29125;
		while (!((1 << v6) & v11))
		{
			//TRACE_GAME_NETWORK_N("CONDITION9");
			if (((1 << v6) & v10))
			{
				//TRACE_GAME_NETWORK_N("CONDITION10");
				v8 = (int *)&v13;
			LABEL_25:
				*(DWORD *)(v7 + 7) = *(DWORD *)(v2 + 112);
				if (*(BYTE *)v7) {
					//TRACE_GAME_NETWORK_N("CONDITION11");

					dynamic_packet_check(
						*(void **)(v2 + 8),
						*(DWORD *)(v2 + 20),
						*(DWORD *)(v7 + 3),
						0,
						0x19u,
						MEMBERSHIP_PACKET_SIZE,
						(int)v8);

					//reset old values
					/*
					*(DWORD *)&v4[1856 * *(DWORD *)v3 + 128] = oldarg1;
					*(DWORD *)(v3 + 4) = oldarg2;
					*(DWORD *)(v2 + 112) = oldarg3;
					*(DWORD *)(v2 + 29600) = oldarg4;*/
				}
			}
			++v6;
			v7 += 28;
			if (v6 >= *(DWORD *)(v2 + 132))
				return write_to_memory(v2 + 0x24E0, v2 + 112, 0x2470u);
		}
		v8 = a7;
		goto LABEL_25;
	}
	return write_to_memory(v2 + 0x24E0, v2 + 112, 0x2470u);
}

typedef bool(__thiscall *valid_packet_type)(int thisx);
valid_packet_type getIsValidPacketMethod() {
	return (valid_packet_type)(h2mod->GetBase() + (h2mod->Server ? 0xCD706 : 0xD114C));
}

int __cdecl request_write(void* a1, int a2, int a3) {
	getDataEncodeIntegerMethod()(a1, (int)"identifier", *(DWORD *)a3, 32);
	int result = getDataEncodeIntegerMethod()(a1, (int)"flags", *(DWORD *)(a3 + 4), 8);
	TRACE_GAME_NETWORK_N("[H2MOD-network] connection request write, identifier=%d, flags=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4));
	return result;
}

bool __cdecl request_read(int a1, int a2, int a3) {
	*(DWORD *)a3 = getDataDecodeIntegerMethod()(a1, (int)"identifier", 32);
	*(DWORD *)(a3 + 4) = getDataDecodeIntegerMethod()(a1, (int)"flags", 8);
	return getIsValidPacketMethod()(a1) == 0;
}

int __cdecl refuse_write(void* a1, int a2, int a3) {
	getDataEncodeIntegerMethod()(a1, (int)"remote-identifier", *(DWORD *)a3, 32);
	int result = getDataEncodeIntegerMethod()(a1, (int)"reason", *(DWORD *)(a3 + 4), 3);
	return result;
}

bool __cdecl refuse_read(int a1, int a2, int a3) {
	*(DWORD *)a3 = getDataDecodeIntegerMethod()(a1, (int)"remote-identifier", 32);
	*(DWORD *)(a3 + 4) = getDataDecodeIntegerMethod()(a1, (int)"reason", 3);
	bool isValid = getIsValidPacketMethod()(a1) == 0;
	//TRACE_GAME_NETWORK_N("[H2MOD-network] connection refuse read, remote-identifier=%d, reason=%d, isValid=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), isValid);
	return isValid;
}

int __cdecl establish_write(void* a1, int a2, int a3) {
	userManager.sendSecurePacket(game_network_data_gateway_socket_1000, 1000);
	getDataEncodeIntegerMethod()(a1, (int)"remote-identifier", *(DWORD *)a3, 32);
	int result = getDataEncodeIntegerMethod()(a1, (int)"reason", *(DWORD *)(a3 + 4), 32);
	//TRACE_GAME_NETWORK_N("[H2MOD-network] connection establish write, remote-identifier=%d, identifier=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4));
	return result;
}

bool __cdecl establish_read(int a1, int a2, int a3) {
	*(DWORD *)a3 = getDataDecodeIntegerMethod()(a1, (int)"remote-identifier", 32);
	*(DWORD *)(a3 + 4) = getDataDecodeIntegerMethod()(a1, (int)"identifier", 32);
	bool isValid = getIsValidPacketMethod()(a1) == 0;
	//TRACE_GAME_NETWORK_N("[H2MOD-network] connection establish read, remote-identifier=%d, identifier=%d, isValid=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), isValid);
	return isValid;
}

int __cdecl closed_write(void* a1, int a2, int a3) {
	getDataEncodeIntegerMethod()(a1, (int)"remote-identifier", *(DWORD *)a3, 32);
	getDataEncodeIntegerMethod()(a1, (int)"identifier", *(DWORD *)(a3 + 4), 32);
	int result = getDataEncodeIntegerMethod()(a1, (int)"closure-reason", *(DWORD *)(a3 + 8), 5);
	//TRACE_GAME_NETWORK_N("[H2MOD-network] connection closed write, remote-identifier=%d, identifier=%d, closureReason=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), *(DWORD *)(a3 + 8));
	return result;
}

bool __cdecl closed_read(int a1, int a2, int a3) {
	signed int v3; // edi@2
	bool result; // al@4

	*(DWORD *)a3 = getDataDecodeIntegerMethod()(a1, (int)"remote-identifier", 32);
	*(DWORD *)(a3 + 4) = getDataDecodeIntegerMethod()(a1, (int)"identifier", 32);
	*(DWORD *)(a3 + 8) = getDataDecodeIntegerMethod()(a1, (int)"closure-reason", 5);
	result = false;
	bool isValid = getIsValidPacketMethod()(a1);
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
	mapManager->sendMapInfoPacket(-1);
	//inform new players of the current advanced lobby settings
	//advLobbySettings->sendLobbySettingsPacket();
	// send server map checksums to client
	//MapChecksumSync::SendState();
	return p_deserialize_player_add(a1, a2, a3);
}

typedef void(__cdecl *register_connection_packets)(void* a1);
register_connection_packets register_connection_packets_method;

void __cdecl registerConnectionPackets(void* packetObject) {
	typedef void(__thiscall *register_packet_type)(void *packetObject, int type, int name, int a4, int size1, int size2, int write_packet_method, int read_packet_method, int a9);
	register_packet_type register_packet = (register_packet_type)(h2mod->GetBase() + (h2mod->Server ? 0x1CA199 : 0x1E81D6));

	register_packet(packetObject, 4, (int)"connect-request", 0, 8, 8, (int)request_write, (int)request_read, 0); //request
	register_packet(packetObject, 5, (int)"connect-refuse", 0, 8, 8, (int)refuse_write, (int)refuse_read, 0); //refuse
	register_packet(packetObject, 6, (int)"connect-establish", 0, 8, 8, (int)establish_write, (int)establish_read, 0); //establish
	register_packet(packetObject, 7, (int)"connect-closed", 0, 12, 12, (int)closed_write, (int)closed_read, 0); //closed
}

typedef void(__cdecl *register_player_packets)(void* a1);
register_player_packets register_player_packets_method;

void __cdecl registerPlayerPackets(void* packetObject) {
	typedef void(__thiscall *register_packet_type)(void *packetObject, int type, int name, int a4, int size1, int size2, int write_packet_method, int read_packet_method, int a9);
	register_packet_type register_packet = (register_packet_type)(h2mod->GetBase() + (h2mod->Server ? 0x1CA199 : 0x1E81D6));

	register_packet(packetObject, 25, (int)"membership-update", 0, MEMBERSHIP_PACKET_SIZE, MEMBERSHIP_PACKET_SIZE,
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D0072 : 0x1EF6B9)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D0496 : 0x1EFADD)), 0);

	register_packet(packetObject, 37, (int)"virtual-couch-update", 0, 216, 216,
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D09FB : 0x1F0042)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D0AFC : 0x1F0143)), 0);

	register_packet(packetObject, 38, (int)"virtual-couch-request", 0, 208, 208,
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D0C1D : 0x1F0264)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D0CD6 : 0x1F031D)), 0);

	register_packet(packetObject, 26, (int)"peer-properties", 0, 208, 208,
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D0DAE : 0x1F03F5)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D0E99 : 0x1F04E0)), 0);

	register_packet(packetObject, 27, (int)"delegate-leadership", 0, 44, 44,
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D0FA7 : 0x1F05EE)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D0FD3 : 0x1F061A)), 0);

	register_packet(packetObject, 28, (int)"boot-machine", 0, 44, 44,
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D100B : 0x1F0652)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D1037 : 0x1F067E)), 0);

	register_packet(packetObject, 29, (int)"player-add", 0, 168, 168,
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D106F : 0x1F06B6)),
		(int)deserializePlayerAdd, 0);

	register_packet(packetObject, 30, (int)"player-refuse", 0, 20, 20,
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D11D8 : 0x1F081F)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D1218 : 0x1F085F)), 0);

	register_packet(packetObject, 31, (int)"player-remove", 0, 12, 12,
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D1275 : 0x1F08BC)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D12A3 : 0x1F08EA)), 0);

	register_packet(packetObject, 32, (int)"player-properties", 0, 156, 156,
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D12EE : 0x1F0935)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D1365 : 0x1F09AC)), 0);
}

typedef int(__stdcall *send_packet)(void* thisx, void *a2, unsigned int type, unsigned int size, int packet_data_obj);
send_packet send_packet_method;

int __stdcall sendPacket(void* thisx, void *a2, unsigned int type, unsigned int size, int packet_data_obj) {
	TRACE_GAME_NETWORK_N("[h2mod-network] send packet type=%d, typeName=%s, size=%d", type, getTextForEnum(type), size);
	return send_packet_method(thisx, a2, type, size, packet_data_obj);
}

bool decodePacketTypeAndSize(void *thisx, int a2, signed int *a3, int a4) {
	char *v4; // ebp@1
	signed int v5; // eax@2
	int v6; // eax@4
	bool result; // al@7

	v4 = (char *)thisx;
	*a3 = getDataDecodeIntegerMethod()(a2, (int)"type", 8);
	*(DWORD *)a4 = getDataDecodeIntegerMethod()(a2, (int)"size", 16);
	TRACE_GAME_NETWORK_N("[h2mod-network] received packet decoded type=%d, typeName=%s, size=%d", *a3, getTextForEnum(*a3), *(DWORD *)a4);
	if (getIsValidPacketMethod()(a2)
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
		result = getIsValidPacketMethod()(a2) == 0;
	}
	return result;
}

typedef char(__stdcall *receive_packet)(void *thisx, int a2, int packetType, unsigned int *size, void *packet_obj);
receive_packet receive_packet_method;

char __stdcall receivePacket(void *thisx, int a2, int packetType, unsigned int *size, void *packet_obj) {
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
		result = (*(int(__cdecl **)(int, unsigned int, void *))(v6 + 24))(a2, *size, packet_obj);// calls packet read/write method
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

void __cdecl serializeChatPacket(void* a1, int a2, int a3) {

	typedef void(__cdecl *serialize_chat_packet_type)(void* a1, int a2, int a3);
	auto serialize_chat_packet_method = reinterpret_cast<serialize_chat_packet_type>(h2mod->GetBase() + (h2mod->Server ? 0x1CD7E1 : 0x1ECE28)); ;
	serialize_chat_packet_method(a1, a2, a3);

	if (network->networkCommand != nullptr && network->networkCommand[0] != '\0') {
		std::string encoded_message = base64_encode(network->networkCommand, network->command_size);
		INT32 packet_size = encoded_message.size();
		if (LOG_CHECK(packet_size <= CHAT_PACKET_EXTEND_SIZE)) {
			char* commandToWrite = (char*)(a3 + (CHAT_PACKET_ORG_SIZE));
			memset(commandToWrite, 0, CHAT_PACKET_EXTEND_SIZE);
			memcpy(commandToWrite, encoded_message.c_str(), packet_size);
			//TODO: if text is precense, don't send over the command, as this call stack is from someone sending text
			getDataEncodeStringMethod()(a1, (int)"command", (int)commandToWrite, CHAT_PACKET_EXTEND_SIZE);
		}
		else {
			TRACE_GAME_NETWORK_N("Can't send data larger than chat extend size. data_size=%d, extend_size=%d", packet_size, CHAT_PACKET_EXTEND_SIZE);
		}
	}
}

bool __cdecl deserializeChatPacket(void* a1, int a2, int a3) {

	typedef bool(__cdecl *deserialize_chat_packet_type)(void* a1, int a2, int a3);
	auto deserialize_chat_packet_method = reinterpret_cast<deserialize_chat_packet_type>(h2mod->GetBase() + (h2mod->Server ? 0x1CD8A4 : 0x1ECEEB));
	bool result = deserialize_chat_packet_method(a1, a2, a3);

	char* command = (char*)(a3 + (CHAT_PACKET_ORG_SIZE));
	getDataDecodeStringMethod()((void*)a1, (int)"command", (int)command, CHAT_PACKET_EXTEND_SIZE);
	//TRACE_GAME_NETWORK_N("[h2mod-network] chat packet deserialize code cave, commandText=%s", command);
	//TRACE_GAME_NETWORK_N("[h2mod-network] chat packet deserialize code cave, isFromServer=%d, text=%s", isServer, text);

	if (command != NULL && command[0] != '\0') {
		std::string command_decoded = base64_decode(command);
		network->queuedNetworkCommands.push_front(command_decoded);
	}

	return result;
}

int CustomNetwork::getPacketOffset() {
	int packetOffset = (*(int*)(h2mod->GetBase() + (h2mod->Server ? 0x520B94 : 0x51C474)));
	int packetOffsetTmp = *(DWORD *)(packetOffset + 29600);
	if (packetOffsetTmp == 5 || packetOffsetTmp == 6 || packetOffsetTmp == 7 || packetOffsetTmp == 8) {
		//valid memory offset
		return packetOffset;
	}

	packetOffset = (*(int*)(h2mod->GetBase() + (h2mod->Server ? 0x520B94 : 0x51C474))) + 31624;
	return packetOffset;// there can't be any other place for the data
}

void CustomNetwork::sendCustomPacket(int peerIndex) {
	int packetDataObj = this->getPacketOffset();
	typedef int(__thiscall* sub_12320C8_type)(int thisx, DWORD* a2, DWORD* a3);
	sub_12320C8_type sub_12320C8 = (sub_12320C8_type)(h2mod->GetBase() + (h2mod->Server ? 0x1997DB : 0x1C20C8));

	typedef char(__thiscall *dynamic_packet_check_method)(void *thisx, int a2, int a3, char a4, unsigned int type, unsigned int size, int a7);
	dynamic_packet_check_method dynamic_packet_check = (dynamic_packet_check_method)(h2mod->GetBase() + (h2mod->Server ? 0x1B8C1A : 0x1BED40));

	BYTE packetdata[CHAT_PACKET_SIZE];
	memset(packetdata, 0, (sizeof BYTE) * CHAT_PACKET_SIZE);

	int playerCountAddr = sub_12320C8((int)packetDataObj, 0, 0);
	if (*(DWORD *)(playerCountAddr + 20) > 0)
	{
		if (peerIndex != *(DWORD *)(packetDataObj + 29120))
		{
			//only send the command packet to the given peer index
			char* newPacketObject = (char*)(packetDataObj);
			dynamic_packet_check(
				*((void **)newPacketObject + 2),
				*((DWORD *)newPacketObject + 5),
				*(DWORD *)&newPacketObject[28 * peerIndex + 29128],
				0,
				packet_type::text_chat,
				CHAT_PACKET_SIZE,
				(int)&packetdata);
		}
	}
}

void CustomNetwork::sendCustomPacketToAllPlayers() {
	int packetDataObj = this->getPacketOffset();
	typedef int(__thiscall* sub_12320C8_type)(int thisx, DWORD* a2, DWORD* a3);
	sub_12320C8_type sub_12320C8 = (sub_12320C8_type)(h2mod->GetBase() + (h2mod->Server ? 0x1997DB : 0x1C20C8));

	typedef signed int(__thiscall* sub_13C31B2_type)(char* thisx, int a2);
	sub_13C31B2_type sub_13C31B2 = (sub_13C31B2_type)(h2mod->GetBase() + (h2mod->Server ? 0x19A86A : 0x1C31B2));

	typedef char(__thiscall *dynamic_packet_check_method)(void *thisx, int a2, int a3, char a4, unsigned int type, unsigned int size, int a7);
	dynamic_packet_check_method dynamic_packet_check = (dynamic_packet_check_method)(h2mod->GetBase() + (h2mod->Server ? 0x1B8C1A : 0x1BED40));

	BYTE packetdata[CHAT_PACKET_SIZE];
	memset(packetdata, 0, (sizeof BYTE) * CHAT_PACKET_SIZE);

	int playerCountAddr = sub_12320C8((int)packetDataObj, 0, 0);
	int peerIndex = 0;
	int peerCount = *(DWORD *)(playerCountAddr + 20);
	TRACE_GAME_NETWORK_N("[h2mod-network] Peer count %d", peerCount);
	if (peerCount > 0) {
		do {
			TRACE_GAME_NETWORK_N("[h2mod-network] sending packet to all players, peerIndex=%d,", peerIndex);
			//only send the command packet to valid peers
			char* newPacketObject = (char*)(packetDataObj);
			if (peerIndex != *(DWORD *)((int)newPacketObject + 29120)) {
				signed int peerIndexPacketStatus = sub_13C31B2(newPacketObject, peerIndex);
				if (peerIndexPacketStatus != -1) {
					if (peerIndex != *((DWORD *)newPacketObject + 7280)) {
						if (newPacketObject[28 * peerIndex + 29125]) {
							dynamic_packet_check(
								*((void **)newPacketObject + 2),
								*((DWORD *)newPacketObject + 5),
								*(DWORD *)&newPacketObject[28 * peerIndex + 29128],
								0,
								packet_type::text_chat,
								CHAT_PACKET_SIZE,
								(int)&packetdata);
						}
					}
				}
			}
			++peerIndex;
		} while (peerIndex < peerCount);
	}
}

typedef void(__cdecl *register_chat_packets)(void* packetObject);
register_chat_packets register_chat_packets_method;

void __cdecl registerChatPackets(void* packetObject) {
	typedef void(__thiscall *register_packet_type)(void *packetObject, int type, int name, int a4, int size1, int size2, int write_packet_method, int read_packet_method, int a9);
	register_packet_type register_packet = (register_packet_type)(h2mod->GetBase() + (h2mod->Server ? 0x1CA199 : 0x1E81D6));

	register_packet(packetObject, packet_type::text_chat, (int)"text chat", 0,
		CHAT_PACKET_SIZE,
		CHAT_PACKET_SIZE,
		(int)serializeChatPacket,
		(int)deserializeChatPacket,
		0);
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
	mapManager->sendMapInfoPacket(-1);
	//advLobbySettings->sendLobbySettingsPacket();
	// send server map checksums to client
	//MapChecksumSync::SendState();
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
	mapManager->sendMapInfoPacket(-1);
	//advLobbySettings->sendLobbySettingsPacket();
	// send server map checksums to client
	//MapChecksumSync::SendState();
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

void __cdecl serialize_join_request(void *a1, int a2, int a3)
{
	typedef void(__cdecl* join_request_serialize_impl)(void* a1, int a2, int a3);
	auto p_join_request_serialize_impl = reinterpret_cast<join_request_serialize_impl>(h2mod->GetBase() + 0x1F0BC0);
	p_join_request_serialize_impl(a1, a2, a3);
}

bool __cdecl deserialize_join_request(void* a1, int a2, int a3)
{
	typedef bool(__cdecl* deserialize_join_request_impl)(void* a1, int a2, int a3);
	auto p_deserialize_join_request_impl = reinterpret_cast<deserialize_join_request_impl>(h2mod->GetBase() + (h2mod->Server ? 0x1D16B5 : 0x1F0CFC));

	bool ret = p_deserialize_join_request_impl(a1, a2, a3);

	/*XNADDR* joining_player_addr = reinterpret_cast<XNADDR*>((BYTE*)a3 + 1320);
	if (ret == true)
		userManager.CreateUser(joining_player_addr, TRUE);*/
	
	return ret;
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
	WritePointer(h2mod->GetBase() + (h2mod->Server ? 0x1D20E4 : 0x1F172B), (void*)deserialize_join_request);

	// force hard-coded qos data in-lobby
	PatchCall(h2mod->GetBase() + (h2mod->Server ? 0x1B7B8A : 0x1BDCB0), QoSLookUpImpl);

	//NopFill<9>(h2mod->GetBase() + (h2mod->Server ? 0x1B3CC3 : 0x1F1F94)); // check if secure/ipaddress != 127.0.0.1
	// disable network observer (broken on H2V)
	//WriteValue<BYTE>(h2mod->GetBase() + (h2mod->Server ? 0x1A92BA : 0x1B555C), (BYTE)0);
	// also ping bars
	//WriteValue<BYTE>(h2mod->GetBase() + (h2mod->Server ? 0x1C1B7F : 0x1D4E35), 0xEB);

	// makes Live network not as laggy 
	int data = 250;
	WriteValue<int>(h2mod->GetBase() + (h2mod->Server ? 0x24896 : 0x28702), data);
	
	// research
	/*static float unk_flt_ = 120.0f;
	DWORD addresses[] = { 0x1BDE27, 0x1BE2FA, 0x1BFB3C, 0x1C11FA, 0x1C12BF };
	DWORD addresses_dedi[] = { 0x1B7D01, 0x1B81D4, 0x1B9A1C, 0x1BB0DA, 0x1BB19F };
	int size = (h2mod->Server ? sizeof(addresses_dedi) : sizeof(addresses)) / sizeof(int);
	for (int i = 0; i < size; i++)
	{
		DWORD addr = h2mod->Server ? addresses_dedi[i] : addresses[i];
		WritePointer(h2mod->GetBase() + addr + 4, &unk_flt_);
	}*/

	

	if (!h2mod->Server)
	{
		pjoin_game = (tjoin_game)DetourClassFunc((BYTE*)h2mod->GetBase() + 0x1CDADE, (BYTE*)join_game, 13);
		VirtualProtect(pjoin_game, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		WritePointer(h2mod->GetBase() + 0x1F1730, (void*)serialize_join_request);
	}
}

void CustomNetwork::applyNetworkHooks() {
	DWORD dwBack;
	DWORD registerConnectionPacketsOffset = 0x1F1B36;
	DWORD registerPlayerPacketsOffset = 0x1F0A55;
	DWORD serializePacketsOffset = 0x1E8296;
	DWORD deserializePacketsOffset = 0x1E82E0;
	DWORD registerChatPacketsOffset = 0x1ECFB7;
	DWORD sendChatPacketOffset = 0x1CADF7;
	DWORD sendChatPacketOffset2 = 0x1C81EC;
	DWORD deserializePlayerAddOffset = 0x1F0753;
	DWORD serializeMembershipPacketOffset = 0x1EF6B9;
	DWORD deserializeMembershipPacketOffset = 0x1EFADD;
	DWORD serializeParametersUpdatePacketOffset = 0x1EDC41;
	DWORD serializePeerPropertiesPacketOffset = 0x1F03F5;

	if (h2mod->Server) {
		registerConnectionPacketsOffset = 0x1D24EF;
		registerPlayerPacketsOffset = 0x1D140E;
		serializePacketsOffset = 0x1CA259;
		deserializePacketsOffset = 0x1CA2A3;
		registerChatPacketsOffset = 0x1CD970;
		sendChatPacketOffset = 0x19F9DC;
		sendChatPacketOffset2 = 0x1A237F;
		deserializePlayerAddOffset = 0x1D110C;
		serializeMembershipPacketOffset = 0x1D0072;
		serializeParametersUpdatePacketOffset = 0x1CE5FA;
		deserializeMembershipPacketOffset = 0x1D0496;
	}

	///////////////////////////////////////////////
	//connection/player packet customizations below
	///////////////////////////////////////////////

	register_connection_packets_method = (register_connection_packets)DetourFunc((BYTE*)h2mod->GetBase() + registerConnectionPacketsOffset, (BYTE*)registerConnectionPackets, 5);
	VirtualProtect(register_connection_packets_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	
	//use for debugging
	//register_player_packets_method = (register_player_packets)DetourFunc((BYTE*)h2mod->GetBase() + registerPlayerPacketsOffset, (BYTE*)registerPlayerPackets, 5);
	//VirtualProtect(register_player_packets_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	serialize_membership_packet_method = (serialize_membership_packet)DetourFunc((BYTE*)h2mod->GetBase() + serializeMembershipPacketOffset, (BYTE*)serializeMembershipPacket, 5);
	VirtualProtect(serialize_membership_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	deserialize_membership_packet_method = (deserialize_membership_packet)DetourFunc((BYTE*)h2mod->GetBase() + deserializeMembershipPacketOffset, (BYTE*)deserializeMembershipPacket, 12);
	VirtualProtect(deserialize_membership_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	serialize_parameters_update_packet_method = (serialize_parameters_update_packet)DetourFunc((BYTE*)h2mod->GetBase() + serializeParametersUpdatePacketOffset, (BYTE*)serializeParametersUpdatePacket, 5);
	VirtualProtect(serialize_parameters_update_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	//TODO-Issue-34: turning this on makes hosts not care if clients can load map or now
	//serialize_peer_properties_method = (serialize_peer_properties)DetourFunc((BYTE*)h2mod->GetBase() + serializePeerPropertiesPacketOffset, (BYTE*)serializePeerPropertiesPacket, 5);
	//VirtualProtect(serialize_peer_properties_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	///////////////////////////////////////
	//text chat packet customizations below
	///////////////////////////////////////

	WriteValue<DWORD>(h2mod->GetBase() + sendChatPacketOffset + 1, CHAT_PACKET_SIZE);
	WriteValue<DWORD>(h2mod->GetBase() + sendChatPacketOffset2 + 1, CHAT_PACKET_SIZE);
	
	register_chat_packets_method = (register_chat_packets)DetourFunc((BYTE*)h2mod->GetBase() + registerChatPacketsOffset, (BYTE*)registerChatPackets, 6);
	VirtualProtect(register_chat_packets_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	/////////////////////////////////////////////////////////////////////
	//send/recv packet functions below (for troubleshooting and research)
	/////////////////////////////////////////////////////////////////////
	//send_packet_method = (send_packet)DetourClassFunc((BYTE*)h2mod->GetBase() + serializePacketsOffset, (BYTE*)sendPacket, 8);
	//VirtualProtect(send_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	//receive_packet_method = (receive_packet)DetourClassFunc((BYTE*)h2mod->GetBase() + deserializePacketsOffset, (BYTE*)receivePacket, 11);
	//VirtualProtect(receive_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	//receive_data_from_socket_method = (receive_data_from_socket)DetourClassFunc((BYTE*)h2mod->GetBase() + 0x1BAFB5, (BYTE*)receiveDataFromSocket, 10);
	//VirtualProtect(receive_data_from_socket_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	applyConnectionPatches();
}