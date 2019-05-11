#pragma once

extern int Packets;
extern int LastUpdateTime;
extern int CurrentTime;
extern int ElapsedTime;

extern int ElapsedTimeAvg;
extern int LastUpdateTimeAvg;
extern int TotalBytesSent;
extern int TotalPacketsSent;
extern bool NetworkStatistics;
extern char packet_info_str[255];

void updateSendToStatistics(int packetSize);