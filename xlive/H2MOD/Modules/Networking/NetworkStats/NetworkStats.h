#pragma once

extern bool NetworkStatistics;
extern int packetSizeAvg;
extern int packetsPerSecondAvg;
extern char packet_info_str[255];

void updateSendToStatistics(int packetsSent, int packetSize);