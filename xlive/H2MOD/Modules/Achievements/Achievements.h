#pragma once

void AchievementUnlock(unsigned long long xuid, int achievement_id, XOVERLAPPED* pOverlapped);
void GetAchievements(unsigned long long xuid);

extern bool g_achievement_list[42];
