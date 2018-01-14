#include "Globals.h"
#include <CUser.h>

//original = 0x3DA8
#define MEMBERSHIP_PACKET_SIZE 0x3e28
#define MEMBERSHIP_PACKET_SIZE_RAW_BYTES 0x28, 0x3E

const char* getTextForEnum(int enumVal) {
	return packet_type_strings[enumVal];
}

//0xD1FA7
typedef void(__thiscall *data_decode_string)(void* thisx, int a2, int a3, int a4);
data_decode_string getDataDecodeStringMethod() {
	return (data_decode_string)(h2mod->GetBase() + (h2mod->Server ? 0xCE561 : 0xD1FA7));
}

//0xD1FFD
typedef int(__thiscall *data_decode_address)(int thisx, int a1, int a2);
data_decode_address getDataDecodeAddressMethod() {
	return (data_decode_address)(h2mod->GetBase() + (h2mod->Server ? 0xCE5B7 : 0xD1FFD));
}

//0xD1F95
typedef int(__thiscall *data_decode_id)(int thisx, int a1, int a2, int a3);
data_decode_id getDataDecodeId() {
	return (data_decode_id)(h2mod->GetBase() + (h2mod->Server ? 0xCE54F : 0xD1F95));
}

//0xD1EE5
typedef unsigned int(__thiscall *data_decode_integer)(int thisx, int a1, int a2);
data_decode_integer getDataDecodeIntegerMethod() {
	return (data_decode_integer)(h2mod->GetBase() + (h2mod->Server ? 0xCE49F : 0xD1EE5));
}

//0xD1F47
typedef bool(__thiscall *data_decode_bool)(int thisx, int a2);
data_decode_bool getDataDecodeBoolMethod() {
	return (data_decode_bool)(h2mod->GetBase() + (h2mod->Server ? 0xCE501 : 0xD1F47));
}

typedef int(__thiscall *data_encode_integer)(void *thisx, int name, unsigned int value, signed int a3);
data_encode_integer getDataEncodeIntegerMethod() {
	return (data_encode_integer)(h2mod->GetBase() + (h2mod->Server ? 0xCDD80 : 0xD17C6));
}

//0xD18DF
typedef int(__thiscall *data_encode_string)(void* thisx, int a2, int a3, int a4);
data_encode_string getDataEncodeStringMethod() {
	return (data_encode_string)(h2mod->GetBase() + (h2mod->Server ? 0xCDE99 : 0xD18DF));
}

//0xD18CD
typedef int(__thiscall *data_encode_id)(void* thisx, int a1, int a2, int a3);
data_encode_id getDataEncodeIdMethod() {
	return (data_encode_id)(h2mod->GetBase() + (h2mod->Server ? 0xCDE87 : 0xD18CD));
}

typedef BYTE*(__thiscall *data_encode_boolean)(int thisx, int a2, char a3);
data_encode_boolean getDataEncodeBooleanMethod() {
	return (data_encode_boolean)(h2mod->GetBase() + (h2mod->Server ? 0xCDE40 : 0xD1886));
}

typedef void(__cdecl *membership_update_write)(void* a1, int a2, int a3);
membership_update_write membership_update_write_method;

DWORD lengthOfMapName = 0x80;
std::string mapName("map-name");
//TODO: 
std::string isP2pSupported("p2p-supported");

void __cdecl membershipUpdateWrite(void* a1, int a2, int a3) {
	membership_update_write_method(a1, a2, a3);

	//write additional packet information below
	std::string mapFileName = mapManager->getCachedMapFilename();
	char* mapNameToWrite = (char*)(a3 + (MEMBERSHIP_PACKET_SIZE - lengthOfMapName));
	memcpy(mapNameToWrite, mapFileName.c_str(), lengthOfMapName);
	getDataEncodeStringMethod()(a1, (int)mapName.c_str(), (int)mapNameToWrite, lengthOfMapName);
	TRACE_GAME_N("[H2MOD-network] mapNameWritten=%s", mapNameToWrite);
}

// This variable holds the return address, it must be global!
DWORD retAddr = 0;
DWORD packet = 0;
DWORD targetData = 0;

void memberUpdateReadCave() {
	char* mapNameToRead = (char*)(targetData + (MEMBERSHIP_PACKET_SIZE - lengthOfMapName));
	getDataDecodeStringMethod()((void*)packet, (int)mapName.c_str(), (int)mapNameToRead, lengthOfMapName);
	mapManager->setClientMapFilename(std::string(mapNameToRead));
	TRACE_GAME_N("[H2MOD-network] mapNameRead=%s", mapNameToRead);
	TRACE_GAME("[h2mod-network] memberupdate code cave");
}

__declspec(naked) void membershipUpdateReadCave(void) {
	__asm {
		// The first thing we must do in our codecave is save 
		// the return address from the top of the stack
		pop retAddr

		// Remember that we need to preserve registers and the stack!
		PUSHAD
		PUSHFD

		//ecx gets reset after every data decode/encode call
		mov packet, esi
		mov targetData, edi
	}
	memberUpdateReadCave();
	__asm {
		// Restore everything to how it was before
		POPFD
		POPAD

		//rewrite overwritten bytes
		//mov[edi + 0x3DA0], eax
		pop edi
		pop esi
		pop ebp
		mov al, 1

		// The last thing we must do in our codecave is push 
		// the return address back onto the stack and then RET back
		push retAddr
		ret
	}
}

void afterMapsLoadedCave() {
	mapManager->reloadMapFilenames();
	gameManager->start();
}

__declspec(naked) void afterMapsLoadDedi(void) {

	__asm {
		// The first thing we must do in our codecave is save 
		// the return address from the top of the stack
		pop retAddr

		// Remember that we need to preserve registers and the stack!
		PUSHAD
		PUSHFD
	}
	afterMapsLoadedCave();
	__asm {
		// Restore everything to how it was before
		POPFD
		POPAD

		//rewrite overwritten bytes
		sub esp, 0x10
		mov ecx, [esp + 0x14]

		// The last thing we must do in our codecave is push 
		// the return address back onto the stack and then RET back
		push retAddr
		ret
	}
}

__declspec(naked) void afterMapsLoaded(void) {

	__asm {
		// The first thing we must do in our codecave is save 
		// the return address from the top of the stack
		pop retAddr

		// Remember that we need to preserve registers and the stack!
		PUSHAD
		PUSHFD
	}
	afterMapsLoadedCave();
	__asm {
		// Restore everything to how it was before
		POPFD
		POPAD

		//rewrite overwritten bytes
		pop edi
		pop esi
		pop ebx
		xor ecx, esp

		// The last thing we must do in our codecave is push 
		// the return address back onto the stack and then RET back
		push retAddr
		ret
	}
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
	
	int v1; // ebx@1
	int v2; // esi@1
	int v3; // edi@2
	char *v4; // ecx@5
	int v5; // eax@10
	int v6; // ebx@19
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
					//TRACE_GAME("CONDITION1, %d", *(BYTE *)(v3 - 1));
					if ((*(DWORD *)&v4[1856 * *(DWORD *)v3 + 128] == 7 && *(DWORD *)(v3 + 4) != *(DWORD *)(v2 + 112)))
					{
						TRACE_GAME("CONDITION2, firstArg=%d, secondArg=%d, thirdArg=%d", *(DWORD *)&v4[1856 * *(DWORD *)v3 + 128], *(DWORD *)(v3 + 4), *(DWORD *)(v2 + 112));
						if (sub_13FD95D(v4, *(DWORD *)(v2 + 20), *(DWORD *)v3, 0x19u))
						{
							TRACE_GAME("CONDITION3");
							sub_13FD9FA(*(void **)(v2 + 8), *(DWORD *)(v2 + 20), *(DWORD *)v3, 0x19u);
						}
						else if (*(DWORD *)(v2 + 29600) != 8)
						{
							TRACE_GAME("CONDITION4, arg=%d", *(DWORD *)(v2 + 29600));
							v5 = *(DWORD *)(v3 + 4);
							if (v5 == -1 || v5 != *(DWORD *)(v2 + 0x24E0)) {
								TRACE_GAME("CONDITION5");
								v10 |= 1 << v1;
							} 
							else {
								TRACE_GAME("CONDITION6");
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
			TRACE_GAME("CONDITION7");
			generate_membership_packet_data_method((DWORD *)v2, v2 + 112, 0, (int)&v13);
		}
		if (v11) {
			TRACE_GAME("CONDITION8");
			generate_membership_packet_data_method((DWORD *)v2, v2 + 112, v2 + 9440, (int)&a7);
		}
	}
	v6 = 0;
	if (*(DWORD *)(v2 + 132) > 0)
	{
		v7 = v2 + 29125;
		while (!((1 << v6) & v11))
		{
			//TRACE_GAME("CONDITION9");
			if (((1 << v6) & v10))
			{
				TRACE_GAME("CONDITION10");
				v8 = (int *)&v13;
			LABEL_25:
				*(DWORD *)(v7 + 7) = *(DWORD *)(v2 + 112);
				if (*(BYTE *)v7) {
					TRACE_GAME("CONDITION11");

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
	TRACE_GAME_N("[H2MOD-network] connection request write, identifier=%d, flags=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4));
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
	TRACE_GAME_N("[H2MOD-network] connection refuse read, remote-identifier=%d, reason=%d, isValid=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), isValid);
	return isValid;
}

int __cdecl establish_write(void* a1, int a2, int a3) {
	h2mod->securityPacketProcessing();

	getDataEncodeIntegerMethod()(a1, (int)(h2mod->GetBase() + 0x3C843C), *(DWORD *)a3, 32);
	int result = getDataEncodeIntegerMethod()(a1, (int)(h2mod->GetBase() + 0x3BDC64), *(DWORD *)(a3 + 4), 32);
	TRACE_GAME_N("[H2MOD-network] connection establish write, remote-identifier=%d, identifier=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4));
	return result;
}

bool __cdecl establish_read(int a1, int a2, int a3) {
	*(DWORD *)a3 = getDataDecodeIntegerMethod()(a1, (int)"remote-identifier", 32);
	*(DWORD *)(a3 + 4) = getDataDecodeIntegerMethod()(a1, (int)"identifier", 32);
	bool isValid = getIsValidPacketMethod()(a1) == 0;
	TRACE_GAME_N("[H2MOD-network] connection establish read, remote-identifier=%d, identifier=%d, isValid=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), isValid);
	return isValid;
}

int __cdecl closed_write(void* a1, int a2, int a3) {
	getDataEncodeIntegerMethod()(a1, (int)"remote-identifier", *(DWORD *)a3, 32);
	getDataEncodeIntegerMethod()(a1, (int)"identifier", *(DWORD *)(a3 + 4), 32);
	int result = getDataEncodeIntegerMethod()(a1, (int)"closure-reason", *(DWORD *)(a3 + 8), 5);
	TRACE_GAME_N("[H2MOD-network] connection closed write, remote-identifier=%d, identifier=%d, closureReason=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), *(DWORD *)(a3 + 8));
	return result;
}

bool __cdecl closed_read(int a1, int a2, int a3) {
	signed int v3; // edi@2
	bool result; // al@4

	*(DWORD *)a3 = getDataDecodeIntegerMethod()(a1, (int)"remote-identifier", 32);
	*(DWORD *)(a3 + 4) = getDataDecodeIntegerMethod()(a1, (int)"identifier", 32);
	*(DWORD *)(a3 + 8) = getDataDecodeIntegerMethod()(a1, (int)"closure-reason", 5);
	result = 0;
	bool isValid = getIsValidPacketMethod()(a1);
	if (!isValid)
	{
		v3 = *(DWORD *)(a3 + 8);
		if (v3 >= 0 && v3 < 18)
			result = 1;
	}
	TRACE_GAME_N("[H2MOD-network] connection closed read, remote-identifier=%d, reason=%d, closureReason=%d, isValid=%d, result=%d",
		*(DWORD *)a3, *(DWORD *)(a3 + 4), *(DWORD *)(a3 + 8), isValid, result);
	return result;
}

typedef char*(__cdecl *register_connection_packets)(void* a1);
register_connection_packets register_connection_packets_method;

std::string customPacketType = "customPacket";
std::string customPacketField = "specialint";
int __cdecl custom_packet_write(void* a1, int a2, int a3) {
	return getDataEncodeIntegerMethod()(a1, (int)customPacketField.c_str(), 67, 32);
}

bool __cdecl custom_packet_read(int a1, int a2, int a3) {
	int specialInt = getDataDecodeIntegerMethod()(a1, (int)customPacketField.c_str(), 32);
	bool isValid = getIsValidPacketMethod()(a1);
	return isValid;
}
char* __cdecl registerConnectionPackets(void* packetObject) {
	typedef char*(__thiscall *register_packet_type)(void *packetObject, int type, int name, int a4, int size1, int size2, int write_packet_method, int read_packet_method, int a9);
	register_packet_type register_packet = (register_packet_type)(h2mod->GetBase() + (h2mod->Server ? 0x1CA199 : 0x1E81D6));

	/*
	register_packet(a1, 4, (int)(h2mod->GetBase() + 0x3C8494), 0, 8, 8, (int)(h2mod->GetBase() + 0x1F1959), (int)(h2mod->GetBase() + 0x1F1989), 0); //request
	register_packet(a1, 5, (int)(h2mod->GetBase() + 0x3C8484), 0, 8, 8, (int)(h2mod->GetBase() + 0x1F19C3), (int)(h2mod->GetBase() + 0x1F19F3), 0); //refuse
	register_packet(a1, 6, (int)(h2mod->GetBase() + 0x3C8470), 0, 8, 8, (int)establish_write, (int)(h2mod->GetBase() + 0x1F1A5D), 0); //establish
	return register_packet(a1, 7, (int)(h2mod->GetBase() + 0x3C8460), 0, 12, 12, (int)(h2mod->GetBase() + 0x1F1A97), (int)(h2mod->GetBase() + 0x1F1AD9), 0); //closed
	*/
	register_packet(packetObject, 4, (int)"connect-request", 0, 8, 8, (int)request_write, (int)request_read, 0); //request
	register_packet(packetObject, 5, (int)"connect-refuse", 0, 8, 8, (int)refuse_write, (int)refuse_read, 0); //refuse
	register_packet(packetObject, 6, (int)"connect-establish", 0, 8, 8, (int)establish_write, (int)establish_read, 0); //establish

	//TODO: make work
	//registers custom packet type
	//register_packet(packetObject, 66, (int)customPacketType.c_str(), 0, 8, 8, (int)custom_packet_write, (int)custom_packet_read, 0);


	return register_packet(packetObject, 7, (int)"connect_closed", 0, 12, 12, (int)closed_write, (int)closed_read, 0); //closed
}

typedef char*(__cdecl *register_player_packets)(void* a1);
register_player_packets register_player_packets_method;

char* __cdecl registerPlayerPackets(void* packetObject) {
	typedef char*(__thiscall *register_packet_type)(void *packetObject, int type, int name, int a4, int size1, int size2, int write_packet_method, int read_packet_method, int a9);
	register_packet_type register_packet = (register_packet_type)(h2mod->GetBase() + (h2mod->Server ? 0x1CA199 : 0x1E81D6));

	register_packet(packetObject, 25, (int)"membership-update",	0, MEMBERSHIP_PACKET_SIZE, MEMBERSHIP_PACKET_SIZE, 
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D0072 : 0x1EF6B9)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D0496 : 0x1EFADD)), 0);

	register_packet(packetObject, 37, (int)"virtual-couch-update", 0, 216, 216, 
		(int)(h2mod->GetBase() + (h2mod->Server ?  0x1D09FB : 0x1F0042)),
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
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D110B : 0x1F0752)), 0);

	register_packet(packetObject, 30, (int)"player-refuse",	0, 20, 20, 
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D11D8 : 0x1F081F)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D1218 : 0x1F085F)),	0);

	register_packet(packetObject, 31, (int)"player-remove",	0, 12, 12, 
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D1275 : 0x1F08BC)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D12A3 : 0x1F08EA)),	0);

	return register_packet(packetObject, 32, (int)"player-properties", 0, 156, 156,
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D12EE : 0x1F0935)),
		(int)(h2mod->GetBase() + (h2mod->Server ? 0x1D1365 : 0x1F09AC)), 0);
}

typedef int(__stdcall *send_packet)(void* thisx, void *a2, unsigned int type, unsigned int size, int packet_data_obj);
send_packet send_packet_method;

int __stdcall sendPacket(void* thisx, void *a2, unsigned int type, unsigned int size, int packet_data_obj) {
	TRACE_GAME_N("[h2mod-network] send packet type=%d, typeName=%s, size=%d", type, getTextForEnum(type), size);
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
	TRACE_GAME_N("[h2mod-network] received packet decoded type=%d, typeName=%s, size=%d", *a3, getTextForEnum(*a3), *(DWORD *)a4);
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
	TRACE_GAME("[h2mod-network] received packet");
	v5 = (char *)thisx;
	typedef bool(__thiscall *decode_type_and_size)(void* thisx, int a2, signed int* a3, int a4);
	decode_type_and_size decode_type_and_size_method = (decode_type_and_size)(h2mod->GetBase() + (h2mod->Server ? 0x1CA1DA : 0x1E8217));
	if (decodePacketTypeAndSize(thisx, a2, (signed int *)packetType, (int)size))
	{
		TRACE_GAME("[h2mod-network] received packet succesfully decoded");
		v6 = (int)&v5[32 * *(DWORD *)packetType];
		memset(packet_obj, 0, *size);
		result = (*(int(__cdecl **)(int, unsigned int, void *))(v6 + 24))(a2, *size, packet_obj);// calls packet read/write method
	}
	else
	{
		TRACE_GAME("[h2mod-network] received packet was invalid");
		result = 0;
	}
	return result;
}

void patchBYTEs(BYTE* orig, BYTE* values, int size) {
	DWORD dwBack;
	VirtualProtect(orig, size, PAGE_EXECUTE_READWRITE, &dwBack);
	memcpy(orig, (BYTE*)values, size);
	VirtualProtect(orig, size, dwBack, NULL);
}

void CustomNetwork::applyNetworkHooks() {
	DWORD dwBack;
	DWORD triggerMembershipPacketOffset = 0x1CA3E0;
	DWORD generateMembershipDataPacketOffset = 0x1C3811;
	DWORD serializeMembershipDataPacketOffset = 0x1EF6B9;
	DWORD deserializeMembershipDataPacketCaveOffset = 0x1F0032;
	DWORD registerConnectionPacketsOffset = 0x1F1B36;
	DWORD registerPlayerPacketsOffset = 0x1F0A55;
	DWORD serializePacketsOffset = 0x1E8296;
	DWORD deserializePacketsOffset = 0x1E82E0;
	if (h2mod->Server) {
		triggerMembershipPacketOffset = 0x1A1968;
		generateMembershipDataPacketOffset = 0x19AEC9;
		serializeMembershipDataPacketOffset = 0x1D0072;
		deserializeMembershipDataPacketCaveOffset = 0x1D09EB;
		registerConnectionPacketsOffset = 0x1D24EF;
		registerPlayerPacketsOffset = 0x1D140E;
		serializePacketsOffset = 0x1CA259;
		deserializePacketsOffset = 0x1CA2A3;
	}

	////////////////////////////////////
	//member packet customizations below
	////////////////////////////////////
	BYTE bytes[5] = { 0x68, MEMBERSHIP_PACKET_SIZE_RAW_BYTES, 0x00, 0x00 };
	patchBYTEs((BYTE*)h2mod->GetBase() + generateMembershipDataPacketOffset, bytes, 5);

	trigger_membership_packet_method = (trigger_membership_packet)DetourClassFunc((BYTE*)h2mod->GetBase() + triggerMembershipPacketOffset, (BYTE*)triggerMembershipPacketData, 10);
	VirtualProtect(trigger_membership_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	membership_update_write_method = (membership_update_write)DetourFunc((BYTE*)h2mod->GetBase() + serializeMembershipDataPacketOffset, (BYTE*)membershipUpdateWrite, 5);
	VirtualProtect(membership_update_write_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	Codecave(h2mod->GetBase() + deserializeMembershipDataPacketCaveOffset, membershipUpdateReadCave, 0);

	//we hook the spot in dedi and peers where the main game data is loaded so we can do stuff with it after
	if (h2mod->Server) {
		Codecave(h2mod->GetBase() + 0x1A2F9, afterMapsLoadDedi, 2);
	} else {
		Codecave(h2mod->GetBase() + 0x4D311, afterMapsLoaded, 0);
	}
	
	register_connection_packets_method = (register_connection_packets)DetourFunc((BYTE*)h2mod->GetBase() + registerConnectionPacketsOffset, (BYTE*)registerConnectionPackets, 5);
	VirtualProtect(register_connection_packets_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	register_player_packets_method = (register_player_packets)DetourFunc((BYTE*)h2mod->GetBase() + registerPlayerPacketsOffset, (BYTE*)registerPlayerPackets, 5);
	VirtualProtect(register_player_packets_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	/////////////////////////////////////////////////////////////////////
	//send/recv packet functions below (for troubleshooting and research)
	/////////////////////////////////////////////////////////////////////
	//send_packet_method = (send_packet)DetourClassFunc((BYTE*)h2mod->GetBase() + serializePacketsOffset, (BYTE*)sendPacket, 8);
	//VirtualProtect(send_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	//receive_packet_method = (receive_packet)DetourClassFunc((BYTE*)h2mod->GetBase() + deserializePacketsOffset, (BYTE*)receivePacket, 11);
	//VirtualProtect(receive_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);
}