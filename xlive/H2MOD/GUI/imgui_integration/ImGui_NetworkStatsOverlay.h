#pragma once

struct net_bandwidth_display_data
{
	float		val;
	const char* unit_str;
};

void ShowNetworkStatsOverlay(bool* p_open);
