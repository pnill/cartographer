#include "Squirrel.h"
#include "ScriptDownloader.h"
#include "H2MOD/Modules/Networking/Networking.h"
#include "H2MOD/Modules/Networking/NetworkObserver/NetworkObserver.h"
#include "xliveless.h"
#include <iostream>
#include <limits>

#undef max // windows.h defines max.

bool g_sqScriptDownloaded = false;

namespace sqScriptDownloader {

	void RequestThread()
	{
		while (!g_sqScriptDownloaded)
		{
			send_sq_request_packet();
			Sleep(3000);
		}

	}

	void StartRequestThread()
	{
		std::thread t(RequestThread);
		t.detach();		
	}

	void send_sq_request_packet()
	{
		network_session* session = NetworkSession::getCurrentNetworkSession();
		if (session->local_session_state == network_session_state_peer_established)
		{
			s_request_script data;
			XUserGetXUID(0, &data.user_identifier);

			network_observer* observer = session->network_observer_ptr;
			peer_observer_channel* observer_channel = NetworkSession::getPeerObserverChannel(session->session_host_peer_index);

			if (observer_channel->field_1)
			{
				observer->sendNetworkMessage(session->unk_index, observer_channel->observer_index, false, request_sq_script, sizeof(s_request_script), &data);
			}
		}
	}

	std::string sq_send_script_fill_data()
	{
			std::ifstream file;
			file.open(ScriptEngine::loaded_script.c_str(), std::ios::in |std::ios::binary);
			if (file.is_open())
			{
				std::ostringstream sstr;
				sstr << file.rdbuf();
				std::string script_data = sstr.str();

				return sstr.str();
			}
			else 
			{
				LOG_CRITICAL_FUNC("Failed to open script, script: {}", ScriptEngine::loaded_script.c_str());
				return std::string("");
			}
	}

	void sq_process_script(char* script_data)
	{
		// process script data received
	}

	void  encode_sq_request_packet(bitstream* stream, int a2, s_request_script* data)
	{
		LOG_INFO_FUNC(" user_identifier: {}", data->user_identifier);
		stream->data_encode_integer("user-identifier", data->user_identifier, 64);
	}

	bool decode_sq_request_packet(bitstream* stream, int a2, s_request_script* data)
	{
		data->user_identifier = stream->data_decode_integer("user-identifier", 64);
		
		LOG_INFO_FUNC(" user_idenitfier: {}", data->user_identifier);

		return stream->packet_is_valid() == false;
	}

	void encode_sq_send_packet(bitstream* stream, int a2, s_send_script* data)
	{
		LOG_INFO_FUNC("data->script_size: {}",data->script_size);
		LOG_INFO_FUNC("data->script: {}",(char*)&data->script_data);

		stream->data_encode_integer("sqScriptSize", data->script_size, 32);
		stream->data_encode_bits("sqScriptData", &data->script_data, data->script_size * CHAR_BIT);
	}

	bool decode_sq_send_packet(bitstream* stream, int a2, s_send_script* data)
	{
		data->script_size = stream->data_decode_integer("sqScriptSize", 32);
		stream->data_decode_bits("sqScriptData", (int)&data->script_data, data->script_size * CHAR_BIT);
		
		LOG_INFO_FUNC("data->script_size: {}", data->script_size);
		LOG_INFO_FUNC("data->data: {}", (char*)&data->script_data);
		g_sqScriptDownloaded = true;

		return stream->packet_is_valid() == false;
	}





}
