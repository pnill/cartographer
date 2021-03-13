#include "stdafx.h"
#include "NetworkStats.h"

int Packets = 0;
int LastUpdateTime = 0;
int CurrentTime = 0;
int ElapsedTime = 0;
//	****************************
int ElapsedTimeAvg = 0;
int LastUpdateTimeAvg = 0;
int TotalBytesSent = 0;
int TotalPacketsSent = 0;
bool NetworkStatistics = false;

char packet_info_str[255];
void updateSendToStatistics(int packetSize)
{
	if (NetworkStatistics) 
	{
		CurrentTime = timeGetTime();
		ElapsedTime = CurrentTime - LastUpdateTime;

		if (ElapsedTime >= 1000)
		{
			LastUpdateTime = CurrentTime;
			Packets = 0;
		}

		ElapsedTimeAvg = CurrentTime - LastUpdateTimeAvg;
		if (ElapsedTimeAvg >= 10000)
		{
			LastUpdateTimeAvg = CurrentTime;
			TotalBytesSent = 0;
			TotalPacketsSent = 0;
		}

		Packets++;
		TotalPacketsSent++;
		TotalBytesSent += packetSize;
	}
}