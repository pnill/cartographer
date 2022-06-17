#pragma once

void AchievementUnlock(unsigned long long xuid, int achievement_id, XOVERLAPPED* pOverlapped);
void GetAchievements(unsigned long long xuid);

extern std::map<DWORD, bool> achievementList;
extern std::unordered_map<std::string, bool> AchievementMap;
