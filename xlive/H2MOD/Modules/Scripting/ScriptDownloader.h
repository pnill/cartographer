#pragma once
#include "H2MOD\Modules\Networking\Memory\bitstream.h"
#include "H2MOD\Modules\Networking\CustomPackets\CustomPackets.h"
namespace sqScriptDownloader {
	void StartRequestThread();
	void  encode_sq_request_packet(bitstream* stream, int a2, s_request_script* data);
	bool decode_sq_request_packet(bitstream* stream, int a2, s_request_script* data);
	void encode_sq_send_packet(bitstream* stream, int a2, s_send_script* data);
	bool decode_sq_send_packet(bitstream* stream, int a2, s_send_script* data);
	void send_sq_request_packet();
	std::string sq_send_script_fill_data();
}
