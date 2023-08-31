#include "stdafx.h"

#include "imgui.h"
#include "ImGui_NetworkStatsOverlay.h"

#include "Blam/Engine/Networking/Session/NetworkSession.h"

#include "XLive/xnet/IpManagement/XnIp.h"

// converts the ammount of bytes to the appropriate measurement unit and gets the units description string
net_bandwidth_display_data get_net_bandwidth_display_data(unsigned int bytes_count)
{
	static const char* network_bandwidth_measurement_unit[] =
	{
		"byte(s)",
		"kbyte(s)",
		"mbytes(s)",
		"gbytes(s)",
		"<unknown>",
	};

	net_bandwidth_display_data result = { (float)bytes_count, network_bandwidth_measurement_unit[0] };

	int bandwidth_measurement_unit_idx = 0;
	while (result.val > 1000)
	{
		if (bandwidth_measurement_unit_idx >= 5)
			break;

		result.val = (float)result.val / 1000.f;
		result.unit_str = network_bandwidth_measurement_unit[++bandwidth_measurement_unit_idx];
	}

	return result;
}

void ShowNetworkStatsOverlay(bool* p_open)
{
	if (p_open == nullptr 
		|| !*p_open)
		return;

	static int corner = 0;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoFocusOnAppearing
		| ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoMouseInputs
		;

	if (corner != -1)
	{
		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
		window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		window_flags |= 0
			| ImGuiWindowFlags_NoMove
			;
	}

	ImGui::SetNextWindowBgAlpha(0.40f); // Transparent background
	if (ImGui::Begin("##Network Status", p_open, window_flags))
	{
		ImGui::Text("Network stats overlay\n");
		ImGui::Separator();

		s_network_session* session;
		if (NetworkSession::GetActiveNetworkSession(&session))
		{
			ImGui::Text("Network protocol: %s", session->get_game_network_protocol());
		}
		else
		{
			ImGui::Text("Network protocol: <disconnected>");
		}

		unsigned long long network_time = *Memory::GetAddressRelative<unsigned long long*>(0x879E98);

		ImGui::Text("Network time: %llu msec", network_time);

		XnIp* localIp = gXnIpMgr.GetLocalUserXn();

		if (localIp->m_valid)
		{
			const XnIpPckTransportStats* local_user_net_metrics;
			localIp->PckGetStats(&local_user_net_metrics);
			
			localIp->m_pckStats.PckDataSampleUpdate();

			int currentSentPerSecIdx = local_user_net_metrics->pckCurrentSendPerSecIdx;
			int currentRecvdPerSecIdx = local_user_net_metrics->pckCurrentRecvdPerSecIdx;

			if (currentSentPerSecIdx != -1)
			{
				net_bandwidth_display_data bandwidth_usage = get_net_bandwidth_display_data(local_user_net_metrics->pckBytesSentPerSec[currentSentPerSecIdx]);

				ImGui::Text("Network pck xmit:  %d pck/s", 
					local_user_net_metrics->pckSentPerSec[currentSentPerSecIdx]);
				ImGui::Text("Network pck bandwidth xmit:  %.3g %s/s", 
					bandwidth_usage.val, bandwidth_usage.unit_str);
			}

			if (currentRecvdPerSecIdx != -1)
			{
				net_bandwidth_display_data bandwidth_usage = get_net_bandwidth_display_data(local_user_net_metrics->pckBytesRecvdPerSec[currentRecvdPerSecIdx]);

				ImGui::Text("Network pck recvd: %d pck/s", 
					local_user_net_metrics->pckRecvdPerSec[currentRecvdPerSecIdx]);
				ImGui::Text("Network pck bandwidth recvd: %.3g %s/s", 
					bandwidth_usage.val, bandwidth_usage.unit_str);
			}
		}

		if (window_flags & ImGuiWindowFlags_NoMouseInputs)
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
				if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
				if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
				if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
				if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
				if (p_open && ImGui::MenuItem("Close")) *p_open = false;
				ImGui::EndPopup();
			}
		}
	}

	ImGui::End();
}
