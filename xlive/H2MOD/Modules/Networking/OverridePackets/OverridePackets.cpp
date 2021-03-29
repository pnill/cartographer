#include  "OverridePackets.h"
#include "H2MOD/Modules/Config/Config.h"
#include "H2MOD/Modules/Networking/CustomPackets/CustomPackets.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/Networking/Memory/bitstream.h"

#define SYNCRONOUS_UPDATE_DEFAULT_SIZE 15320
//(SYNCRONOUS_UPDATE_DEFAULT_SIZE + (6 * 8 * 16));
#define SYNCRONOUS_UPDATE_NEW_SIZE 16088
namespace OverridePackets
{

	typedef void(__cdecl p_register_synchronous_simulation_packets)(void* packetObject);
	p_register_synchronous_simulation_packets o_register_synchronous_simulation_packets;

	void __cdecl register_syncronous_simulation_ackets(void* packetObject)
	{
		register_packet_impl(packetObject, 42, "syncronous-update", 0, SYNCRONOUS_UPDATE_NEW_SIZE, SYNCRONOUS_UPDATE_NEW_SIZE,
			h2mod->GetAddress<void*>(0, 0), h2mod->GetAddress<void*>(0, 0), h2mod->GetAddress<void*>(0, 0));
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
		auto p_deserialize_player_add = h2mod->GetAddress<deserialize_player_add*>(0x1F0752, 0x1D110B);

		return p_deserialize_player_add(a1, a2, a3);
	}

	typedef void(__cdecl *register_player_packets)(void* a1);
	register_player_packets register_player_packets_method;

	void __cdecl registerPlayerPackets(void* packetObject) {

		register_packet_impl(packetObject, 25, "membership-update", 0, 15784, 15784,
			h2mod->GetAddress<void*>(0x1EF6B9, 0x1D0072),
			h2mod->GetAddress<void*>(0x1EFADD, 0x1D0496), 0);

		register_packet_impl(packetObject, 37, "virtual-couch-update", 0, 216, 216,
			h2mod->GetAddress<void*>(0x1F0042, 0x1D09FB),
			h2mod->GetAddress<void*>(0x1F0143, 0x1D0AFC), 0);

		register_packet_impl(packetObject, 38, "virtual-couch-request", 0, 208, 208,
			h2mod->GetAddress<void*>(0x1F0264, 0x1D0C1D),
			h2mod->GetAddress<void*>(0x1F031D, 0x1D0CD6), 0);

		register_packet_impl(packetObject, 26, "peer-properties", 0, 208, 208,
			h2mod->GetAddress<void*>(0x1F03F5, 0x1D0DAE),
			h2mod->GetAddress<void*>(0x1F04E0, 0x1D0E99), 0);

		register_packet_impl(packetObject, 27, "delegate-leadership", 0, 44, 44,
			h2mod->GetAddress<void*>(0x1F05EE, 0x1D0FA7),
			h2mod->GetAddress<void*>(0x1F061A, 0x1D0FD3), 0);

		register_packet_impl(packetObject, 28, "boot-machine", 0, 44, 44,
			h2mod->GetAddress<void*>(0x1F0652, 0x1D100B),
			h2mod->GetAddress<void*>(0x1F067E, 0x1D1037), 0);

		register_packet_impl(packetObject, 29, "player-add", 0, 168, 168,
			h2mod->GetAddress<void*>(0x1F06B6, 0x1D106F),
			(void*)deserializePlayerAdd, 0);

		register_packet_impl(packetObject, 30, "player-refuse", 0, 20, 20,
			h2mod->GetAddress<void*>(0x1F081F, 0x1D11D8),
			h2mod->GetAddress<void*>(0x1F085F, 0x1D1218), 0);

		register_packet_impl(packetObject, 31, "player-remove", 0, 12, 12,
			h2mod->GetAddress<void*>(0x1F08BC, 0x1D1275),
			h2mod->GetAddress<void*>(0x1F08EA, 0x1D12A3), 0);

		register_packet_impl(packetObject, 32, "player-properties", 0, 156, 156,
			h2mod->GetAddress<void*>(0x1F0935, 0x1D12EE),
			h2mod->GetAddress<void*>(0x1F09AC, 0x1D1365), 0);
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
		register_connection_packets_method = (register_connection_packets)DetourFunc(h2mod->GetAddress<BYTE*>(0x1F1B36, 0x1D24EF), (BYTE*)registerConnectionPackets, 5);

		//use for debugging
		//register_player_packets_method = (register_player_packets)DetourFunc(h2mod->GetAddress<BYTE*>(0x1F0A55, 0x1D140E), (BYTE*)registerPlayerPackets, 5);

		//serialize_parameters_update_packet_method = (serialize_parameters_update_packet)DetourFunc((BYTE*)h2mod->GetAddress(0x1F03F5, 0x1CE5FA), (BYTE*)serializeParametersUpdatePacket, 5);

		/////////////////////////////////////////////////////////////////////
		//send/recv packet functions below (for troubleshooting and research)
		/////////////////////////////////////////////////////////////////////
		//send_packet_method = (send_packet)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1E8296, 0x1CA259), (BYTE*)sendPacket, 8);

		//receive_packet_method = (receive_packet)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1E82E0, 0x1CA2A3), (BYTE*)receivePacket, 11);

		if (h2mod->Server) {
			p_decode_text_chat_packet = (decode_text_chat_packet_)DetourFunc(h2mod->GetAddress<BYTE*>(0, 0x1CD8A4), (BYTE*)decode_text_chat_packet, 12);
		}
	}
}
