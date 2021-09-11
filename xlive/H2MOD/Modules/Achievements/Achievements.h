#pragma once

void AchievementUnlock(XUID xuid, int achievement_id, XOVERLAPPED* pOverlapped);
void GetAchievements(XUID xuid);

extern std::map<DWORD, bool> achievementList;
extern std::unordered_map<std::string, bool> AchievementMap;
