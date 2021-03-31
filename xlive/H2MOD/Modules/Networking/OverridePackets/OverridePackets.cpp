#include  "OverridePackets.h"
#include "H2MOD/Modules/Config/Config.h"
#include "H2MOD/Modules/Networking/CustomPackets/CustomPackets.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/Networking/Memory/bitstream.h"
#include "Blam/Engine/Players/PlayerControls.h"

#define PRECISION_INCREASE 1

#define SYNCRONOUS_UPDATE_DEFAULT_SIZE 15320
//(SYNCRONOUS_UPDATE_DEFAULT_SIZE + (6 * 8 * 16));
#define SYNCRONOUS_UPDATE_NEW_SIZE 16088

#define SYNCORNOUS_ACTIONS_DEFAULT_SIZE 400
//(SYNCRONOUS_ACTIONS_DEFAULT_SIZE + (6 * 8 * 4));
#define SYNCORNOUS_ACTIONS_NEW_SIZE 592


namespace OverridePackets
{


	typedef void(__cdecl p_register_synchronous_simulation_packets)(void* packetObject);
	p_register_synchronous_simulation_packets* o_register_synchronous_simulation_packets;

	void __cdecl register_syncronous_simulation_packets(void* packetObject)
	{
#if PRECISION_INCREASE == 1
		register_packet_impl(packetObject, 42, "syncronous-update", 0, 
			SYNCRONOUS_UPDATE_NEW_SIZE, SYNCRONOUS_UPDATE_NEW_SIZE,
			Memory::GetAddress<void*>(0x1ED084, 0x1CDA3D), 
			Memory::GetAddress<void*>(0x1ED097, 0x1CDA50), 
			Memory::GetAddress<void*>(0x1ED0C6, 0x1CDA7F));

		register_packet_impl(packetObject, 43, "syncronous-actions", 0,
			SYNCORNOUS_ACTIONS_NEW_SIZE, SYNCORNOUS_ACTIONS_NEW_SIZE,
			Memory::GetAddress<void*>(0x1ED0D9, 0x1CDA92),
			Memory::GetAddress<void*>(0x1ED15D, 0x1CDB16),
			Memory::GetAddress<void*>(0x1ED217, 0x1CDBD0));
#else
		register_packet_impl(packetObject, 42, "syncronous-update", 0,
			SYNCRONOUS_UPDATE_DEFAULT_SIZE, SYNCRONOUS_UPDATE_DEFAULT_SIZE,
			Memory::GetAddress<void*>(0x1ED084, 0x1CDA3D),
			Memory::GetAddress<void*>(0x1ED097, 0x1CDA50),
			Memory::GetAddress<void*>(0x1ED0C6, 0x1CDA7F));

		register_packet_impl(packetObject, 43, "syncronous-actions", 0,
			SYNCORNOUS_ACTIONS_DEFAULT_SIZE, SYNCORNOUS_ACTIONS_DEFAULT_SIZE,
			Memory::GetAddress<void*>(0x1ED0D9, 0x1CDA92),
			Memory::GetAddress<void*>(0x1ED15D, 0x1CDB16),
			Memory::GetAddress<void*>(0x1ED217, 0x1CDBD0));
#endif

		register_packet_impl(packetObject, 44, "syncronous-join", 0,
			4, 4,
			Memory::GetAddress<void*>(0x1ED274, 0x1CDC2D),
			Memory::GetAddress<void*>(0x1ED28C, 0x1CDC45),
			NULL);

		register_packet_impl(packetObject, 45, "syncronous-gamestate", 1,
			8, 0xFFFF,
			Memory::GetAddress<void*>(0x1ED2C0, 0x1CDC79),
			Memory::GetAddress<void*>(0x1ED30F, 0x1CDCC8),
			NULL);
	}

	typedef int (__cdecl p_encode_player_actions)(bitstream *pThis, int a2);
	p_encode_player_actions* o_encode_player_actions;

	int __cdecl encode_player_actions(bitstream *pThis, int a2)
	{
		int v2; // esi
		unsigned int v3; // eax
		unsigned int v4; // eax
		int result; // eax
		int v6; // esi
		float v7; // [esp+20h] [ebp-4h]
		float a2a; // [esp+2Ch] [ebp+8h]

		v2 = a2;
		pThis->data_encode_flags("control-flags", *(long long *)a2, 34);
		pThis->data_encode_integer("action-flags", *(DWORD *)(a2 + 32), 23);
#if PRECISION_INCREASE == 1
		pThis->data_encode_bits("yaw", (float *)(a2 + 8), sizeof(float) * CHAR_BIT);
		pThis->data_encode_bits("pitch", (float *)(a2 + 12), sizeof(float) * CHAR_BIT);
		pThis->data_encode_bits("throttle-x", (float *)(a2 + 16), sizeof(float) * CHAR_BIT);
		pThis->data_encode_bits("throttle-y", (float *)(a2 + 20), sizeof(float) * CHAR_BIT);
		pThis->data_encode_bits("trigger", (float *)(a2 + 24), sizeof(float) * CHAR_BIT);
		pThis->data_encode_bits("secondary-trigger", (float *)(a2 + 28), sizeof(float) * CHAR_BIT);
#else
		pThis->data_encode_quantized_real("yaw", *(float *)(a2 + 8), 0.0f, 6.2831855f, 13, 0);
		pThis->data_encode_quantized_real("pitch", *(float *)(a2 + 12), -3.1415927, 3.1415927, 12, 0);
		pThis->data_encode_quantized_real("throttle-x", *(float *)(a2 + 16), -1.0, 1.0, 5, 1);
		pThis->data_encode_quantized_real("throttle-y", *(float *)(a2 + 20), -1.0, 1.0, 5, 1);
		pThis->data_encode_quantized_real("trigger", *(float *)(a2 + 24), 0.0, 1.0, 5, 0);
		pThis->data_encode_quantized_real("secondary-trigger", *(float *)(a2 + 28), 0.0, 1.0, 5, 0);
#endif
		pThis->data_encode_integer("weapon-set-identifier", *(__int16 *)(a2 + 36) + 1, 5);
		pThis->data_encode_integer("primary-weapon-index", *(char *)(a2 + 38) + 1, 3);
		pThis->data_encode_integer("secondary-weapon-index", *(char *)(a2 + 39) + 1, 3);
		pThis->data_encode_integer("grenade-index", *(__int16 *)(a2 + 40) + 1, 2);
		pThis->data_encode_integer("zoom-level", *(__int16 *)(a2 + 42) + 1, 2);
		pThis->data_encode_integer("interaction-type", *(__int16 *)(a2 + 44), 4);
		if (*(WORD *)(a2 + 44))
		{
			pThis->data_encode_integer("interaction-object", *(DWORD *)(a2 + 48), 32);
			switch (*(__int16 *)(a2 + 44))
			{
			case 1:
			case 7:
				pThis->data_encode_integer("interaction-pickup-flags", *(unsigned __int8 *)(a2 + 46), 4);
				break;
			case 4:
			case 5:
			case 6:
				pThis->data_encode_integer("interaction-seat-index", *(__int16 *)(a2 + 46), 5);
				break;
			default:
				break;
			}
		}
		pThis->data_encode_bool("melee-target-exists", *(DWORD *)(a2 + 52) != -1);
		v3 = *(DWORD *)(a2 + 52);
		if (v3 != -1)
			pThis->data_encode_integer("melee-target-unit", v3, 32);
		if (*(float *)(a2 + 84) > 0.0 || *(float *)(a2 + 88) > 0.0 || *(BYTE *)(a2 + 80) & 1 || *(DWORD *)(a2 + 56) != -1)
		{
			pThis->data_encode_bool("autoaim-exists", 1);
			if (*(float *)(a2 + 84) <= 0.0)
				a2a = 0.0;
			else
				a2a = *(float *)(a2 + 84);
			if (*(float *)(v2 + 88) <= 0.0)
				v7 = 0.0;
			else
				v7 = *(float *)(v2 + 88);
			pThis->data_encode_quantized_real("primary-autoaim-level", a2a, 0.0, 1.0, 4, 0);
			pThis->data_encode_quantized_real("secondary-autoaim-level", v7, 0.0, 1.0, 4, 0);
			pThis->data_encode_integer("flags", *(__int16 *)(v2 + 80), 2);
			pThis->data_encode_integer("target-object", *(DWORD *)(v2 + 56), 32);
			if (*(DWORD *)(v2 + 56) != -1)
			{
				pThis->data_encode_bool("model-target-exists", *(DWORD *)(v2 + 60) != -1);
				v4 = *(DWORD *)(v2 + 60);
				if (v4 != -1)
					pThis->data_encode_integer("model-target", v4, 5);
			}
		}
		else
		{
			pThis->data_encode_bool("autoaim-exists", 0);
		}
		pThis->data_encode_bool("target-player-exists", *(DWORD *)(v2 + 64) != -1);
		v6 = *(DWORD *)(v2 + 64);
		if (v6 != -1)
			pThis->data_encode_integer("target-player", (unsigned __int16)v6, 4);
		return result;
	}

	typedef bool (__cdecl p_decode_player_actions)(bitstream *pThis, int a1);
	p_decode_player_actions* o_decode_player_actions;

	bool __cdecl decode_player_actions(bitstream *pThis, int a1)
	{
		__int16 v2; // ax
		unsigned int v3; // eax
		int v4; // eax

		typedef int(__cdecl p_player_actions_new)(int *a1);
		p_player_actions_new* player_actions_new = Memory::GetAddress<p_player_actions_new*>(0x512F8, 0x5981B);

		typedef int(__cdecl p_get_player_datum_index)(int a1);
		p_get_player_datum_index* get_player_datum_index = Memory::GetAddress<p_get_player_datum_index*>(0x513F3, 0x59916);

		typedef bool(__cdecl p_player_actions_valid)(int *a1);
		p_player_actions_valid* player_actions_valid = Memory::GetAddress<p_player_actions_valid*>(0x53210, 0x5B708);

		player_actions_new((int*)a1);
		*(unsigned long long *)a1 = pThis->data_decode_flags("control-flags", 34);
		*(DWORD *)(a1 + 32) = pThis->data_decode_integer("action-flags", 23);
#if (PRECISION_INCREASE == 1)
		pThis->data_decode_bits("yaw", (float *)(a1 + 8), sizeof(float) * CHAR_BIT);
		pThis->data_decode_bits("pitch", (float *)(a1 + 12), sizeof(float) * CHAR_BIT);
		pThis->data_decode_bits("throttle-x", (float *)(a1 + 16), sizeof(float) * CHAR_BIT);
		pThis->data_decode_bits("throttle-y", (float *)(a1 + 20), sizeof(float) * CHAR_BIT);
		pThis->data_decode_bits("trigger", (float *)(a1 + 24), sizeof(float) * CHAR_BIT);
		pThis->data_decode_bits("secondary-trigger", (float *)(a1 + 28), sizeof(float) * CHAR_BIT);
#else
		*(float *)(a1 + 8) = pThis->data_decode_quantized_real("yaw", 0.0, 6.2831855, 13, 0);
		*(float *)(a1 + 12) = pThis->data_decode_quantized_real("pitch", -3.1415927, 3.1415927, 12, 0);
		*(float *)(a1 + 16) = pThis->data_decode_quantized_real("throttle-x", -1.0, 1.0, 5, 1);
		*(float *)(a1 + 20) = pThis->data_decode_quantized_real("throttle-y", -1.0, 1.0, 5, 1);
		*(float *)(a1 + 24) = pThis->data_decode_quantized_real("trigger", 0.0, 1.0, 5, 0);
		*(float *)(a1 + 28) = pThis->data_decode_quantized_real("secondary-trigger", 0.0, 1.0, 5, 0);
#endif

		*(WORD *)(a1 + 36) = pThis->data_decode_integer("weapon-set-identifier", 5) - 1;
		*(BYTE *)(a1 + 38) = pThis->data_decode_integer("primary-weapon-index", 3) - 1;
		*(BYTE *)(a1 + 39) = pThis->data_decode_integer("secondary-weapon-index", 3) - 1;
		*(WORD *)(a1 + 40) = pThis->data_decode_integer("grenade-index", 2) - 1;
		*(WORD *)(a1 + 42) = pThis->data_decode_integer("zoom-level", 2) - 1;
		v2 = pThis->data_decode_integer("interaction-type", 4);
		*(WORD *)(a1 + 44) = v2;
		if (v2)
		{
			*(DWORD *)(a1 + 48) = pThis->data_decode_integer("interaction-object", 32);
			switch (*(__int16 *)(a1 + 44))
			{
			case 1:
			case 7:
				*(BYTE *)(a1 + 46) = pThis->data_decode_integer("interaction-pickup-flags", 4);
				break;
			case 4:
			case 5:
			case 6:
				*(WORD *)(a1 + 46) = pThis->data_decode_integer("interaction-seat-index", 5);
				break;
			default:
				break;
			}
		}
		else
		{
			*(DWORD *)(a1 + 48) = -1;
		}
		if (pThis->data_decode_bool("melee-target-exists"))
			*(DWORD *)(a1 + 52) = pThis->data_decode_integer("melee-target-unit", 32);
		else
			*(DWORD *)(a1 + 52) = -1;
		if (pThis->data_decode_bool("autoaim-exists"))
		{
			*(float *)(a1 + 84) = pThis->data_decode_quantized_real("primary-autoaim-level", 0.0, 1.0, 4, 0);
			*(float *)(a1 + 88) = pThis->data_decode_quantized_real("secondary-autoaim-level", 0.0, 1.0, 4, 0);
			*(WORD *)(a1 + 80) = pThis->data_decode_integer("flags", 2);
			v3 = pThis->data_decode_integer("target-object", 32);
			*(DWORD *)(a1 + 56) = v3;
			if (v3 != -1 && pThis->data_decode_bool("model-target-exists"))
				*(DWORD *)(a1 + 60) = pThis->data_decode_integer("model-target", 5);
		}
		if (pThis->data_decode_bool("target-player-exists"))
		{
			v4 = pThis->data_decode_integer("target-player", 4);
			*(DWORD *)(a1 + 64) = get_player_datum_index(v4);
		}
		return player_actions_valid((int *)a1) != 0;
	}

	void __cdecl request_write(bitstream* stream, int a2, int a3) {
		stream->data_encode_integer("identifier", *(DWORD *)a3, 32);
		stream->data_encode_integer("flags", *(DWORD *)(a3 + 4), 8);
		LOG_TRACE_NETWORK("[H2MOD-network] connection request write, identifier={0}, flags={1}", *(DWORD *)a3, *(DWORD *)(a3 + 4));
	}

	bool __cdecl request_read(bitstream* stream, int a2, int a3) {
		*(DWORD *)a3 = stream->data_decode_integer("identifier", 32);
		*(DWORD *)(a3 + 4) = stream->data_decode_integer("flags", 8);
		return stream->packet_is_valid() == false;
	}

	void __cdecl refuse_write(bitstream* stream, int a2, int a3) {
		stream->data_encode_integer("remote-identifier", *(DWORD *)a3, 32);
		stream->data_encode_integer("reason", *(DWORD *)(a3 + 4), 3);
	}

	bool __cdecl refuse_read(bitstream* stream, int a2, int a3) {
		*(DWORD *)a3 = stream->data_decode_integer("remote-identifier", 32);
		*(DWORD *)(a3 + 4) = stream->data_decode_integer("reason", 3);
		bool isValid = stream->packet_is_valid() == false;
		//LOG_TRACE_NETWORK_N("[H2MOD-network] connection refuse read, remote-identifier={}, reason=%d, isValid=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), isValid);
		return isValid;
	}

	void __cdecl establish_write(bitstream* stream, int a2, int a3) {
		stream->data_encode_integer("remote-identifier", *(DWORD *)a3, 32);
		stream->data_encode_integer("reason", *(DWORD *)(a3 + 4), 32);
		//LOG_TRACE_NETWORK_N("[H2MOD-network] connection establish write, remote-identifier=%d, identifier=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4));
	}

	bool __cdecl establish_read(bitstream* stream, int a2, int a3) {
		*(DWORD *)a3 = stream->data_decode_integer("remote-identifier", 32);
		*(DWORD *)(a3 + 4) = stream->data_decode_integer("identifier", 32);
		bool isValid = stream->packet_is_valid() == false;
		//LOG_TRACE_NETWORK_N("[H2MOD-network] connection establish read, remote-identifier=%d, identifier=%d, isValid=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), isValid);
		return isValid;
	}

	void __cdecl closed_write(bitstream* stream, int a2, int a3) {
		stream->data_encode_integer("remote-identifier", *(DWORD *)a3, 32);
		stream->data_encode_integer("identifier", *(DWORD *)(a3 + 4), 32);
		stream->data_encode_integer("closure-reason", *(DWORD *)(a3 + 8), 5);
		//LOG_TRACE_NETWORK_N("[H2MOD-network] connection closed write, remote-identifier=%d, identifier=%d, closureReason=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), *(DWORD *)(a3 + 8));
	}

	bool __cdecl closed_read(bitstream* stream, int a2, int a3) {
		bool result = false;
		signed int closure_reason;

		*(DWORD *)a3 = stream->data_decode_integer("remote-identifier", 32);
		*(DWORD *)(a3 + 4) = stream->data_decode_integer("identifier", 32);
		*(DWORD *)(a3 + 8) = stream->data_decode_integer("closure-reason", 5);

		if (stream->packet_is_valid())
			return result;

		closure_reason = *(DWORD *)(a3 + 8);
		if (closure_reason >= 0 && closure_reason < 18)
			result = true;

		//LOG_TRACE_NETWORK_N("[H2MOD-network] connection closed read, remote-identifier=%d, reason=%d, closureReason=%d, isValid=%d, result=%d",
		//	*(DWORD *)a3, *(DWORD *)(a3 + 4), *(DWORD *)(a3 + 8), isValid, result);
		return result;
	}

	typedef void(__cdecl *register_connection_packets)(void* a1);
	register_connection_packets register_connection_packets_method;

	void __cdecl registerConnectionPackets(void* packetObject) {
		register_packet_impl(packetObject, 4, "connect-request", 0, 8, 8, request_write, request_read, 0); //request
		register_packet_impl(packetObject, 5, "connect-refuse", 0, 8, 8, refuse_write, refuse_read, 0); //refuse
		register_packet_impl(packetObject, 6, "connect-establish", 0, 8, 8, establish_write, establish_read, 0); //establish
		register_packet_impl(packetObject, 7, "connect-closed", 0, 12, 12, closed_write, closed_read, 0); //closed
	}

	bool __cdecl deserializePlayerAdd(void* a1, int a2, int a3) {
		typedef bool(__cdecl deserialize_player_add)(void* a1, int a2, int a3);
		auto p_deserialize_player_add = Memory::GetAddress<deserialize_player_add*>(0x1F0752, 0x1D110B);

		return p_deserialize_player_add(a1, a2, a3);
	}

	typedef void(__cdecl *register_player_packets)(void* a1);
	register_player_packets register_player_packets_method;

	void __cdecl registerPlayerPackets(void* packetObject) {

		register_packet_impl(packetObject, 25, "membership-update", 0, 15784, 15784,
			Memory::GetAddress<void*>(0x1EF6B9, 0x1D0072),
			Memory::GetAddress<void*>(0x1EFADD, 0x1D0496), 0);

		register_packet_impl(packetObject, 37, "virtual-couch-update", 0, 216, 216,
			Memory::GetAddress<void*>(0x1F0042, 0x1D09FB),
			Memory::GetAddress<void*>(0x1F0143, 0x1D0AFC), 0);

		register_packet_impl(packetObject, 38, "virtual-couch-request", 0, 208, 208,
			Memory::GetAddress<void*>(0x1F0264, 0x1D0C1D),
			Memory::GetAddress<void*>(0x1F031D, 0x1D0CD6), 0);

		register_packet_impl(packetObject, 26, "peer-properties", 0, 208, 208,
			Memory::GetAddress<void*>(0x1F03F5, 0x1D0DAE),
			Memory::GetAddress<void*>(0x1F04E0, 0x1D0E99), 0);

		register_packet_impl(packetObject, 27, "delegate-leadership", 0, 44, 44,
			Memory::GetAddress<void*>(0x1F05EE, 0x1D0FA7),
			Memory::GetAddress<void*>(0x1F061A, 0x1D0FD3), 0);

		register_packet_impl(packetObject, 28, "boot-machine", 0, 44, 44,
			Memory::GetAddress<void*>(0x1F0652, 0x1D100B),
			Memory::GetAddress<void*>(0x1F067E, 0x1D1037), 0);

		register_packet_impl(packetObject, 29, "player-add", 0, 168, 168,
			Memory::GetAddress<void*>(0x1F06B6, 0x1D106F),
			(void*)deserializePlayerAdd, 0);

		register_packet_impl(packetObject, 30, "player-refuse", 0, 20, 20,
			Memory::GetAddress<void*>(0x1F081F, 0x1D11D8),
			Memory::GetAddress<void*>(0x1F085F, 0x1D1218), 0);

		register_packet_impl(packetObject, 31, "player-remove", 0, 12, 12,
			Memory::GetAddress<void*>(0x1F08BC, 0x1D1275),
			Memory::GetAddress<void*>(0x1F08EA, 0x1D12A3), 0);

		register_packet_impl(packetObject, 32, "player-properties", 0, 156, 156,
			Memory::GetAddress<void*>(0x1F0935, 0x1D12EE),
			Memory::GetAddress<void*>(0x1F09AC, 0x1D1365), 0);
	}

	typedef bool(__cdecl* decode_text_chat_packet_)(bitstream* container, int a2, s_text_chat* data);
	decode_text_chat_packet_ p_decode_text_chat_packet;

	bool __cdecl decode_text_chat_packet(bitstream* container, int a2, s_text_chat* data)
	{
		bool ret = p_decode_text_chat_packet(container, a2, data);

		return ret;
	}

	void ApplyGamePatches()
	{
		register_connection_packets_method = (register_connection_packets)DetourFunc(Memory::GetAddress<BYTE*>(0x1F1B36, 0x1D24EF), (BYTE*)registerConnectionPackets, 5);
		o_register_synchronous_simulation_packets = (p_register_synchronous_simulation_packets*)DetourFunc(Memory::GetAddress<BYTE*>(0x1ED397, 0x1CDD50), (BYTE*)register_syncronous_simulation_packets, 6);
		o_encode_player_actions = (p_encode_player_actions*)DetourFunc(Memory::GetAddress<BYTE*>(0x1DFE4C, 0x1C730C), (BYTE*)encode_player_actions, 6);
		o_decode_player_actions = (p_decode_player_actions*)DetourFunc(Memory::GetAddress<BYTE*>(0x1E01CB, 0x1C7965), (BYTE*)decode_player_actions, 5);
		//use for debugging
		//register_player_packets_method = (register_player_packets)DetourFunc(Memory::GetAddress<BYTE*>(0x1F0A55, 0x1D140E), (BYTE*)registerPlayerPackets, 5);

		//serialize_parameters_update_packet_method = (serialize_parameters_update_packet)DetourFunc((BYTE*)Memory::GetAddress(0x1F03F5, 0x1CE5FA), (BYTE*)serializeParametersUpdatePacket, 5);

		/////////////////////////////////////////////////////////////////////
		//send/recv packet functions below (for troubleshooting and research)
		/////////////////////////////////////////////////////////////////////
		//send_packet_method = (send_packet)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1E8296, 0x1CA259), (BYTE*)sendPacket, 8);

		//receive_packet_method = (receive_packet)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1E82E0, 0x1CA2A3), (BYTE*)receivePacket, 11);

		if (h2mod->Server) {
			p_decode_text_chat_packet = (decode_text_chat_packet_)DetourFunc(Memory::GetAddress<BYTE*>(0, 0x1CD8A4), (BYTE*)decode_text_chat_packet, 12);
		}
	}
}
