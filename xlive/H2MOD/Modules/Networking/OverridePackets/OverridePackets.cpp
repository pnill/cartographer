#include "stdafx.h"

#include "OverridePackets.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "Blam/Engine/Networking/NetworkMessageTypeCollection.h"
#include "Blam/Engine/memory/bitstream.h"
#include "Util/Hooks/Hook.h"

#define SYNCRONOUS_UPDATE_DEFAULT_SIZE 15320
//(SYNCRONOUS_UPDATE_DEFAULT_SIZE + (6 * 8 * 16));
#define SYNCRONOUS_UPDATE_NEW_SIZE 16088

#define SYNCORNOUS_ACTIONS_DEFAULT_SIZE 400
//(SYNCRONOUS_ACTIONS_DEFAULT_SIZE + (6 * 8 * 4));
#define SYNCORNOUS_ACTIONS_NEW_SIZE 592


namespace OverridePackets
{

	void __cdecl request_write(bitstream* stream, int a2, int a3) {
		stream->data_encode_integer("identifier", *(DWORD *)a3, 32);
		stream->data_encode_integer("flags", *(DWORD *)(a3 + 4), 8);
		LOG_TRACE_NETWORK("[H2MOD-network] connection request write, identifier={0}, flags={1}", *(DWORD *)a3, *(DWORD *)(a3 + 4));
	}

	bool __cdecl request_read(bitstream* stream, int a2, int a3) {
		*(DWORD *)a3 = stream->data_decode_integer("identifier", 32);
		*(DWORD *)(a3 + 4) = stream->data_decode_integer("flags", 8);
		return stream->error_occured() == false;
	}

	void __cdecl refuse_write(bitstream* stream, int a2, int a3) {
		stream->data_encode_integer("remote-identifier", *(DWORD *)a3, 32);
		stream->data_encode_integer("reason", *(DWORD *)(a3 + 4), 3);
	}

	bool __cdecl refuse_read(bitstream* stream, int a2, int a3) {
		*(DWORD *)a3 = stream->data_decode_integer("remote-identifier", 32);
		*(DWORD *)(a3 + 4) = stream->data_decode_integer("reason", 3);
		bool isValid = stream->error_occured() == false;
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
		bool isValid = stream->error_occured() == false;
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

		if (stream->overflow())
			return result;

		closure_reason = *(DWORD *)(a3 + 8);
		if (closure_reason >= 0 && closure_reason < 18)
			result = true;

		//LOG_TRACE_NETWORK_N("[H2MOD-network] connection closed read, remote-identifier=%d, reason=%d, closureReason=%d, isValid=%d, result=%d",
		//	*(DWORD *)a3, *(DWORD *)(a3 + 4), *(DWORD *)(a3 + 8), isValid, result);
		return result;
	}

	typedef void(__cdecl *register_connection_packets_t)(void* a1);
	register_connection_packets_t p_register_connection_packets;

	void __cdecl registerConnectionPackets(void* packetObject) {
		register_network_message(packetObject, 4, "connect-request", 0, 8, 8, request_write, request_read, 0); //request
		register_network_message(packetObject, 5, "connect-refuse", 0, 8, 8, refuse_write, refuse_read, 0); //refuse
		register_network_message(packetObject, 6, "connect-establish", 0, 8, 8, establish_write, establish_read, 0); //establish
		register_network_message(packetObject, 7, "connect-closed", 0, 12, 12, closed_write, closed_read, 0); //closed
	}

	bool __cdecl deserializePlayerAdd(void* a1, int a2, int a3) {
		typedef bool(__cdecl deserialize_player_add)(void* a1, int a2, int a3);
		auto p_deserialize_player_add = Memory::GetAddress<deserialize_player_add*>(0x1F0752, 0x1D110B);

		return p_deserialize_player_add(a1, a2, a3);
	}

	//typedef void(__cdecl t_encode_player_properties)(bitstream *a1, Player::Properties *a2);
	//t_encode_player_properties* p_encode_player_properties;

	//void __cdecl encode_player_properties_impl(bitstream* stream, Player::Properties *a2)
	//{
	//	stream->data_encode_integer("biped-variant", 10, (CHAR_BIT * sizeof(byte)));

	//	p_encode_player_properties(stream, a2);
	//}

	//typedef bool(__cdecl t_decode_player_properties)(bitstream* stream, Player::Properties *a2);
	//t_decode_player_properties* p_decode_player_properties;

	//bool __cdecl decode_player_properties_impl(bitstream* stream, Player::Properties *a2)
	//{
	//	LOG_INFO_GAME("[{}] {}", __FUNCTION__, a2->unk2);
	//	a2->unk2 = stream->data_decode_integer("biped-variant", (CHAR_BIT * sizeof(byte)));
	//	auto res = p_decode_player_properties(stream, a2);
	//	return res;
	//}

	//bool __cdecl decode_test(bitstream* stream, Player::Properties *a2)
	//{
	//	typedef bool(__cdecl t_decode_player_profile)(bitstream* stream, Player::Properties::PlayerProfile profile);
	//	t_decode_player_profile* p_decode_player_profile = Memory::GetAddress<t_decode_player_profile*>(0x1E36E7);

	//	stream->data_decode_string_wide("player-name", a2->player_name, 32);
	//	bool profie_valid = p_decode_player_profile(stream, a2->profile);
	//	if(stream->data_decode_bool("clan-identifier-valid"))
	//	{
	//		stream->data_decode_bits("clan-identifier", &a2->clan_identifiers, 96);
	//		stream->data_decode_string_wide("clan-name", a2->clan_name, 16);
	//	}
	//}

	typedef void(__cdecl *register_player_network_messages_t)(void* a1);
	register_player_network_messages_t p_register_player_network_messages;

	void __cdecl registerPlayerPackets(void* packetObject) {

		register_network_message(packetObject, 25, "membership-update", 0, 15784, 15784,
			Memory::GetAddress<void*>(0x1EF6B9, 0x1D0072),
			Memory::GetAddress<void*>(0x1EFADD, 0x1D0496), NULL);

		register_network_message(packetObject, 37, "virtual-couch-update", 0, 216, 216,
			Memory::GetAddress<void*>(0x1F0042, 0x1D09FB),
			Memory::GetAddress<void*>(0x1F0143, 0x1D0AFC), NULL);

		register_network_message(packetObject, 38, "virtual-couch-request", 0, 208, 208,
			Memory::GetAddress<void*>(0x1F0264, 0x1D0C1D),
			Memory::GetAddress<void*>(0x1F031D, 0x1D0CD6), NULL);

		register_network_message(packetObject, 26, "peer-properties", 0, 208, 208,
			Memory::GetAddress<void*>(0x1F03F5, 0x1D0DAE),
			Memory::GetAddress<void*>(0x1F04E0, 0x1D0E99), NULL);

		register_network_message(packetObject, 27, "delegate-leadership", 0, 44, 44,
			Memory::GetAddress<void*>(0x1F05EE, 0x1D0FA7),
			Memory::GetAddress<void*>(0x1F061A, 0x1D0FD3), NULL);

		register_network_message(packetObject, 28, "boot-machine", 0, 44, 44,
			Memory::GetAddress<void*>(0x1F0652, 0x1D100B),
			Memory::GetAddress<void*>(0x1F067E, 0x1D1037), NULL);

		register_network_message(packetObject, 29, "player-add", 0, 168, 168,
			Memory::GetAddress<void*>(0x1F06B6, 0x1D106F),
			Memory::GetAddress<void*>(0x1F0752, 0x1D110B), NULL);
			// (void*)deserializePlayerAdd, 0);

		register_network_message(packetObject, 30, "player-refuse", 0, 20, 20,
			Memory::GetAddress<void*>(0x1F081F, 0x1D11D8),
			Memory::GetAddress<void*>(0x1F085F, 0x1D1218), NULL);

		register_network_message(packetObject, 31, "player-remove", 0, 12, 12,
			Memory::GetAddress<void*>(0x1F08BC, 0x1D1275),
			Memory::GetAddress<void*>(0x1F08EA, 0x1D12A3), NULL);

		register_network_message(packetObject, 32, "player-properties", 0, 156, 156,
			Memory::GetAddress<void*>(0x1F0935, 0x1D12EE),
			Memory::GetAddress<void*>(0x1F09AC, 0x1D1365), NULL);
	}

	typedef bool(__cdecl* decode_text_chat_packet_t)(bitstream* stream, int a2, s_text_chat* data);
	decode_text_chat_packet_t p_decode_text_chat_packet;

	bool __cdecl decode_text_chat_packet(bitstream* stream, int a2, s_text_chat* data)
	{
		bool ret = p_decode_text_chat_packet(stream, a2, data);
		return ret;
	}

	void ApplyGamePatches()
	{
		p_register_connection_packets = (register_connection_packets_t)DetourFunc(Memory::GetAddress<BYTE*>(0x1F1B36, 0x1D24EF), (BYTE*)registerConnectionPackets, 5);
		//use for debugging

		/*p_encode_player_properties = Memory::GetAddress<t_encode_player_properties*>(0x1E42BB);
		PatchCall(Memory::GetAddress(0x1F071D), encode_player_properties_impl);
		PatchCall(Memory::GetAddress(0x1F0977), encode_player_properties_impl);

		p_decode_player_properties = Memory::GetAddress<t_decode_player_properties*>(0x1E440A);
		PatchCall(Memory::GetAddress(0x1F07B7), decode_player_properties_impl);
		PatchCall(Memory::GetAddress(0x1F09ED), decode_player_properties_impl);*/

		if (Memory::IsDedicatedServer()) {
			p_decode_text_chat_packet = (decode_text_chat_packet_t)DetourFunc(Memory::GetAddress<BYTE*>(0, 0x1CD8A4), (BYTE*)decode_text_chat_packet, 12);
		}
	}
}
