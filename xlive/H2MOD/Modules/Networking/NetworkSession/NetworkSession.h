#pragma once

#include "stdafx.h"
#include "..\Blam\Engine\Players\Players.h"

enum network_session_state : signed int
{
	_network_session_state_none = -1,
	_network_session_state_peer_joining = 1,
	_network_session_state_peer_join_abort = 2,
	_network_session_state_peer_established = 3,
	_network_session_state_leaving = 4,
	_network_session_state_session_host = 5,
	_network_session_state_host_disband = 6,
	_network_session_state_host_handoff = 7,
	_network_session_state_host_reestablish = 8,
	_network_session_state_election = 9,
	_network_session_state_unk_2 = 10
};

#pragma pack(push, 1)
struct network_address
{
	union
	{
		int ipv4;
		char ipv6[16];
	} address;
	short port;
	short address_type;
};

struct PeerInfo
{
	XNADDR address;
	BYTE gap_24[4];
	wchar_t peer_name[16];
	wchar_t peer_name_2[32];
	signed int field_88;
	signed int field_8C;
	signed int field_90;
	signed int field_94;
	signed int field_98;
	BYTE gap_9C[84];
	DWORD field_F0;
	DWORD field_F4;
	DWORD gets_incremented_unk;
	signed int player_information_index; // used to access the player_info array
	signed int field_100;
	signed int field_104;
	signed int field_108;
};
static_assert(sizeof(PeerInfo) == 268, "Invalid PeerNetworkInfo size");

struct VirtualCouch
{
	DWORD incremental_update_number;
	bool exists;
	XSESSION_INFO xsession_info;
	char pad[3];
	DWORD xuid_count;
	XUID xuid[16];
};
static_assert(sizeof(VirtualCouch) == 200, "Invalid virtual_couch size");

struct PlayerInformation
{
	XUID identifier; // -0xA
	DWORD peer_index; // -0x8
	signed __int16 peer_user_index; // -0x4
	signed __int16 unk; // -0x2
	WORD player_flags; // 0x0
	bool properties_valid;
	char pad[1];
	unsigned int controller_index;
	PlayerProperties player_properties;
	BYTE biped_index;
	PlayerProperties player_properties_1;
	BYTE biped_index2;
	unsigned int player_voice;
	unsigned int player_text_chat;
};
static_assert(sizeof(PlayerInformation) == 296, "Invalid player_info size");

struct unk_network_connection_info
{
	char field_0[4];
	signed int observer_index;
	signed int field_8;
	signed int field_C;
	signed int field_10;
	signed int field_14;
	signed int field_18;
};

struct Membership {
	DWORD update_number;
	DWORD session_leader_index; //this is not host index, but the player which controlls the party
	XUID dedicated_server_xuid;
	DWORD field_80;
	DWORD total_peers;
	PeerInfo peer_info[17];
	DWORD total_party_players;
	DWORD peer_bit_flags;
	PlayerInformation player_info[16];
};

struct network_session
{
	DWORD field_0;
	void *network_message_gateway_ptr;
	void *network_observer_ptr;
	DWORD session_manager_ptr;
	DWORD text_chat;
	int unk_index;
	int field_18;
	int network_mode;
	XNKID session_id;
	char field_28[32];
	char field_48; // 69
	XNKEY xnkey;
	char pad[3];
	int xnkey_index;
	signed int field_60;
	DWORD session_host_peer_index;
	int elected_host_peer_index;
	DWORD field_6C;
	Membership membership;
	DWORD unk_field;
	DWORD unk_field2;
	Membership membership_update_buffer;
	VirtualCouch v_couch_1;
	VirtualCouch v_couch_2;
	DWORD voting_information_1;
	int field_4AE4;
	int field_4AE8;
	int field_4AEC;
	BYTE gap_4AF0[56];
	__int16 field_4B28;
	BYTE gap_4B2A[62];
	int field_4B68;
	int field_4B6C;
	int field_4B70;
	__int16 field_4B74;
	WORD field_4B76;
	WORD field_4B78;
	BYTE gap_4B7A[2];
	int field_4B7C;
	DWORD voting_information;
	BYTE gap_4B84[220];
	DWORD parameters_2;
	BYTE gap_4C64[4];
	int field_4C68;
	DWORD gap_4C6C;
	DWORD system_language_id;
	bool dedicated_server;
	char pad_1[3];
	DWORD party_privacy;
	int max_peers;
	signed int max_party_players;
	int gap_4C84;
	int field_4C88;
	int field_4C8C;
	DWORD field_4C90;
	DWORD field_4C94;
	BYTE gap_4C98[4];
	char dedicated_server_state_valid[4];
	DWORD field_4CA0;
	DWORD field_4CA4;
	BYTE gap_4CA8[16];
	DWORD field_4CB8;
	wchar_t scenario_path[128];
	BYTE gap4C00[12];
	BYTE random_bytes[8];
	DWORD dead_beef_lol;
	DWORD field_4DD4;
	DWORD field_4DD8;
	wchar_t custom_map_name[48];
	DWORD field_4E3C;
	BYTE gap_4E40[704];
	DWORD field_5100;
	BYTE gap_5104[3508];
	DWORD field_5EB8;
	BYTE gap_5EBC[76];
	DWORD field_5F08;
	DWORD field_5F0C;
	DWORD parameters_1;
	BYTE gap_5F14[4780];
	DWORD local_peer_index;
	unk_network_connection_info unk_needs_reversing[17];
	network_session_state local_session_state;
	DWORD time_unk_2;
	DWORD time_unk_3;
	DWORD time_unk;
	DWORD possible_new_peer_host_index;
	DWORD field_73B4;
	DWORD field_73B8;
	int field_73BC;
	char field_73C0[4];
	DWORD time_new_session_host_found;
	int field_73C8;
	DWORD field_73CC;
	int field_73D0;
	int field_73D4;
	int field_73D8;
	int field_73DC;
	BYTE field_73E0[32];
	int field_7400;
	int field_7404;
	BYTE gap_7408[4];
	int field_740C;
	int field_7410;
	BYTE gap_7414[100];
	DWORD field_7478;
	DWORD field_747C;
	DWORD field_7480;
	DWORD field_7484;
	BYTE gap_7488[1132];
	int field_78F4;
	int field_78F8;
	BYTE field_78FC[36];
	int field_7920;
	DWORD field_7924;
	DWORD join_request_attempts_number;
	DWORD last_time_join_request_sent;
	DWORD field_7930;
	DWORD field_7934;
	DWORD field_7938;
	signed int field_793C;
	signed int field_7940;
	signed int field_7944;
	signed int field_7948;
	signed int field_794C;
	signed int field_7950;
	signed int field_7954;
	signed int field_7958;
	signed int field_795C;
	DWORD field_7960;
	DWORD field_7964;
	DWORD field_7968;
	char field_796C[4];
	int field_7970;
	DWORD field_7974;
	DWORD field_7978;
	BYTE gap_797C[508];
	DWORD c_network_message_gateway;
	char field_7B7C[12];
};
#pragma pack(pop)
static_assert(sizeof(network_session) == 31624, "Invalid network_session size");

namespace NetworkSession
{
	network_session* getNetworkSession();
	network_session* getCurrentNetworkSession();
	bool getCurrentNetworkSession(network_session** a1);
	bool localPeerIsSessionHost();
	signed int getPeerIndexFromNetworkAddress(network_address* addr);
	char getMapFileLocation(network_session* thisx, wchar_t* buffer, size_t szBuffer);
	void kick_player(int peerIndex);
}

