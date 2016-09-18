#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "Network.h"
#include "H2MOD.h"
#include "xliveless.h"

namespace Network
{
	namespace bitstreamptr
	{
	
			//DWORD write_uint = 0xD17C6;
			DWORD read_int			= 0xCE49F;
			DWORD read_uint			= 0xCE49F;
			DWORD read_byte			= 0xCE501;
			DWORD read_char			= 0xCE561;
			DWORD read_ulonglong	= 0xCE5B7;
			DWORD read_block		= 0xCE54F;

			DWORD write_uint		= 0xCDD80;


	};

	enum packet_type
	{
		ping,
		pong,
		broadcast_search,
		broadcast_reply,
		connect_request,
		connect_refuse,
		connect_establish,
		connect_closed,
		join_request,
		join_abort,
		join_refuse,
		leave_session,
		leave_acknowledge,
		session_disband,
		session_boot,
		host_handoff,
		peer_handoff,
		host_transition,
		host_reestablish,
		host_decline,
		peer_reestablish,
		peer_establish,
		election,
		election_refuse,
		time_synchronize,
		membership_update,
		peer_properties,
		delegate_leadership,
		boot_machine,
		player_add,
		player_refuse,
		player_remove,
		player_properties,
		parameters_update,
		parameters_request,
		countdown_timer,
		mode_acknowledge,
		virtual_couch_update,
		virtual_couch_request,
		vote_update,
		view_establishment,
		player_acknowledge,
		synchronous_update,
		synchronous_actions,
		synchronous_join,
		synchronous_gamestate,
		game_results,
		text_chat,
		test
	};

	char* GetPacketType(unsigned int typeval);
}


char* Network::GetPacketType(unsigned int typeval)
{

	switch (typeval)
	{
		
		case Network::packet_type::ping:
			return "ping";
		break;

		case Network::packet_type::pong:
			return "pong";
		break;

		case Network::packet_type::broadcast_reply:
			return "broadcast-reply";
		break;

		case Network::packet_type::broadcast_search:
			return "broadcast-search";
		break;	

		case Network::packet_type::connect_request:
			return "connect-request";
		break;

		case Network::packet_type::connect_refuse:
			return "connect-refuse";
		break;

		case Network::packet_type::connect_closed:
			return "connect-closed";
		break;

		case Network::packet_type::join_request:
			return "join-request";
		break;

		case Network::packet_type::join_abort:
			return "join-abort";
		break;

		case Network::packet_type::join_refuse:
			return "join-refuse";
		break;

		case Network::packet_type::leave_session:
			return "leave-session";
		break;

		case Network::packet_type::leave_acknowledge:
			return "leave-acknowledge";
		break;

		case Network::packet_type::session_disband:
			return "session-disband";
		break;

		case Network::packet_type::session_boot:
			return "session-boot";
		break;

		case Network::packet_type::host_handoff:
			return "host-handoff";
		break;

		case Network::packet_type::peer_handoff:
			return "peer-handoff";
		break;

		case Network::packet_type::host_transition:
			return "host-transition";
		break;

		case Network::packet_type::host_reestablish:
			return "host-restablish";
		break;

		case Network::packet_type::host_decline:
			return "host-decline";
		break;

		case Network::packet_type::peer_reestablish:
			return "peer-reestablish";
		break;

		case Network::packet_type::election:
			return "election";
		break;

		case Network::packet_type::election_refuse:
			return "election-refuse";
		break;

		case Network::packet_type::time_synchronize:
			return "time-synchronize";
		break;

		case Network::packet_type::membership_update:
			return "membership-update";
		break;

		case Network::packet_type::peer_properties:
			return "peer-properties";
		break;

		case Network::packet_type::delegate_leadership:
			return "delegate-leadership";
		break;

		case Network::packet_type::boot_machine:
			return "boot-machine";
		break;

		case Network::packet_type::player_add:
			return "player-add";
		break;

		case Network::packet_type::player_refuse:
			return "player-refuse";
		break;
		
		case Network::packet_type::player_remove:
			return "player-remove";
		break;

		case Network::packet_type::player_properties:
			return "player-properties";
		break;

		case Network::packet_type::parameters_update:
			return "parameters-update";
		break;

		case Network::packet_type::parameters_request:
			return "parameters-request";
		break;
		
		case Network::packet_type::countdown_timer:
			return "countdown-timer";
		break;

		case Network::packet_type::mode_acknowledge:
			return "mode-acknowledge";
		break;

		case Network::packet_type::virtual_couch_update:
			return "virtual-couch-update";
		break;

		case Network::packet_type::virtual_couch_request:
			return "virtual-couch-request";
		break;

		case Network::packet_type::vote_update:
			return "vote-update";
		break;

		case Network::packet_type::view_establishment:
			return "view-establishment";
		break;

		case Network::packet_type::player_acknowledge:
			return "player-acknowledge";
		break;

		case Network::packet_type::synchronous_update:
			return "synchronous-update";
		break;

		case Network::packet_type::synchronous_actions:
			return "synchronous-actions";
		break;
		
		case Network::packet_type::synchronous_join:
			return "synchronous-join";
		break;

		case Network::packet_type::synchronous_gamestate:
			return "synchronous-gamestate";
		break;

		case Network::packet_type::game_results:
			return "game-results";
		break;

		case Network::packet_type::text_chat:
			return "text-chat";
		break;

		case Network::packet_type::test:
			return "test";
		break;
	}

	return "unknown";
}

typedef int(__stdcall *tWrite_uint)(DWORD *, char*, unsigned int, signed int);
tWrite_uint pWrite_uint;

//typedef int(__stdcall *tRead_block)(DWORD*, char*, char*, signed int);
//tRead_block pRead_block;



int __stdcall write_uint(DWORD *thisptr, char* valuestr, unsigned int value, signed int maxval)
{


	if ( memcmp(valuestr, "type", strlen(valuestr)) )
		TRACE_GAME_N("bitstream::write_uint - string: %s, value: %i", valuestr, value);
	else
		TRACE_GAME_NETWORK("bitstream::write_uint::packet - %s",Network::GetPacketType(value));


	return pWrite_uint(thisptr, valuestr, value, maxval);
}
/**
int __stdcall read_block(DWORD* thisptr, char* valuestr, char* blockptr, signed int bitsize)
{
		TRACE_GAME_N("bitstream::read_block - string: %s, ptr: %08X, bitsize: %i", valuestr, blockptr, bitsize);

		int ret = pRead_block(thisptr, valuestr, blockptr, bitsize);

		char* blockhex = { 0 };

		signed int bytesize = bitsize / 8;

		blockhex = (char*)malloc(bytesize * 2 + 1);


		for (int i = 0; i < bytesize; i++)
		{
			_snprintf(&blockhex[i * 2], 2, "%02X", blockptr[i]);
		}

		TRACE_GAME_N("bitstream::read_block - hex: %s", blockhex);

		blockhex[sizeof(blockhex)] = { 0 };

		free(blockhex);

	return ret;
}*/

/*
	Sloppy CodeCave for write_uint until I can find a better way to hook.
*/
DWORD read_uint_retAddr = 0;
char* read_uint_string  = 0;

int read_uint_MaxValue = 0;
int read_uint_value = 0;
DWORD thisptr = 0;
DWORD subfunc = 0xCDC35;

void read_uint_display()
{
	if (memcmp(read_uint_string, "type", strlen(read_uint_string)))
		TRACE_GAME_N("bitstream::read_uint - string: %s value: %i", read_uint_string, read_uint_value);
	else
		TRACE_GAME_NETWORK("bitstream::read_uint::packet - %s", Network::GetPacketType(read_uint_value));

	if (!memcmp(read_uint_string, "bungienet-user-role", strlen(read_uint_string)))
	{
		read_uint_value = 1;
	}

}

/* The actual caved function */
__declspec(naked) void read_bitstream_uint(void)
{
	
	__asm
	{
			pop read_uint_retAddr;
			mov thisptr, ecx
			mov eax, dword ptr [esp+0x04]
			mov read_uint_string,eax
			mov eax, dword ptr[esp+0x08]
			mov read_uint_MaxValue, eax

			push eax
			mov eax, subfunc
			call eax
			
			mov read_uint_value, eax

	}

			read_uint_display();

	__asm
	{
			mov ecx,thisptr
			mov eax,read_uint_value
			mov	[esp+0x08],eax
			push read_uint_retAddr
			ret 
	}

}


void Network::Initialize()
{

	if (h2mod->Server)
	{
		Network::bitstreamptr::read_uint += h2mod->GetBase();
		Network::bitstreamptr::write_uint += h2mod->GetBase();
		Network::bitstreamptr::read_block += h2mod->GetBase();
	
		subfunc += h2mod->GetBase();
		DWORD dwBack;



		TRACE_GAME("H2MOD::Network::bitstream - Writing uint(%08X) hook", Network::bitstreamptr::read_uint);
		Codecave(Network::bitstreamptr::read_uint, read_bitstream_uint, 5);

		TRACE_GAME("H2MOD::Network::bistream - Writing write_uint(%08X) hook", Network::bitstreamptr::write_uint);
		pWrite_uint = (tWrite_uint)DetourClassFunc((BYTE*)Network::bitstreamptr::write_uint, (BYTE*)write_uint, 11);
		VirtualProtect(pWrite_uint, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		/*
		TRACE_GAME("H2MOD::Network::bitstream - Writing read_block(%08X) hook", Network::bitstreamptr::read_block);
		pRead_block = (tRead_block)DetourClassFunc((BYTE*)Network::bitstreamptr::read_block, (BYTE*)read_block, 8);
		VirtualProtect(pRead_block, 4, PAGE_EXECUTE_READWRITE, &dwBack);*/

		TRACE_GAME("H2MOD::Network - Initialized\n");
	}

}