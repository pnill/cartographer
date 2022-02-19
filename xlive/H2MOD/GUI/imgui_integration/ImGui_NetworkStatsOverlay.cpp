#include "stdafx.h"

#include "imgui.h"
#include "ImGui_NetworkStatsOverlay.h"

#include "XLive/xnet/IpManagement/XnIp.h"

const char* network_bandwidth_measurement_unit[] =
{
	"byte(s)",
	"kbyte(s)"
};

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

		XnIp* localIp = gXnIp.GetLocalUserXn();

		if (localIp->bValid)
		{
			const XnIpPckTransportStats* local_user_net_metrics;
			localIp->PckGetStats(&local_user_net_metrics);
			
			localIp->pckStats.PckStatsUpdate();

			int currentSentPerSecIdx = local_user_net_metrics->pckCurrentSendPerSecIdx;
			int currentRecvdPerSecIdx = local_user_net_metrics->pckCurrentRecvdPerSecIdx;

			if (currentSentPerSecIdx != -1 && currentRecvdPerSecIdx != -1)
			{
				ImGui::Text("Network pck xmit:  %d pck/s", local_user_net_metrics->pckSentPerSec[currentSentPerSecIdx]);
				ImGui::Text("Network pck recvd: %d pck/s", local_user_net_metrics->pckRecvdPerSec[currentRecvdPerSecIdx]);

				bool use_kbyte_bytes_sent = local_user_net_metrics->pckBytesSentPerSec[currentSentPerSecIdx] > 1000;
				bool use_kbyte_bytes_recvd = local_user_net_metrics->pckBytesRecvdPerSec[currentRecvdPerSecIdx] > 1000;

				// TODO measurement unit improvements
				float xmit_per_sec_in_var_units = local_user_net_metrics->pckBytesSentPerSec[currentSentPerSecIdx];
				if (use_kbyte_bytes_sent)
					xmit_per_sec_in_var_units = (float)local_user_net_metrics->pckBytesSentPerSec[currentSentPerSecIdx] / 1000.f;

				float bytes_recvd_in_var_units = local_user_net_metrics->pckBytesRecvdPerSec[currentSentPerSecIdx];
				if (use_kbyte_bytes_recvd)
					bytes_recvd_in_var_units = (float)local_user_net_metrics->pckBytesRecvdPerSec[currentSentPerSecIdx] / 1000.f;

				ImGui::Text("Network pck bandwidth xmit:  %.3g %s/s", xmit_per_sec_in_var_units, network_bandwidth_measurement_unit[use_kbyte_bytes_sent]);
				ImGui::Text("Network pck bandwidth recvd: %.3g %s/s", bytes_recvd_in_var_units, network_bandwidth_measurement_unit[use_kbyte_bytes_recvd]);
			}
		}

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

	ImGui::End();
}
