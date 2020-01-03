#include "stdafx.h"
#include "XAchievements.h"

#include "Globals.h"

int achieveinit = 0;

extern void Check_Overlapped(PXOVERLAPPED pOverlapped);
HANDLE g_dwFakeAchievementContent = (HANDLE)-2;


// #5278: XUserWriteAchievements
DWORD WINAPI XUserWriteAchievements(DWORD count, PXUSER_ACHIEVEMENT pAchievement, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XUserWriteAchievements  (count = {0:#x}, buffer = {1:p}, pOverlapped = {2:p})",
		count, (void*)pAchievement, (void*)pOverlapped);

	if (count > 0)
	{
		while (count > 0)
		{

			int AchievementID = pAchievement->dwAchievementId;

			LOG_TRACE_GAME("Achievement {0} unlock attempt by Player {1} - id2: {2}", pAchievement->dwAchievementId, pAchievement->dwUserIndex, AchievementID);

			if (achievementList[AchievementID] == 0)
			{
				achievementList[AchievementID] = 1;

				std::string AchievementData;

				switch (AchievementID)
				{
				case 1:
					AchievementData.append("Cairo Station|Complete Cairo Station.");
					break;

				case 2:
					AchievementData.append("Outskirts|Complete Outskirts.");
					break;

				case 3:
					AchievementData.append("Metropolis|Complete Metropolis.");
					break;

				case 4:
					AchievementData.append("The Arbiter|Complete The Arbiter.");
					break;

				case 5:
					AchievementData.append("Oracle|Complete The Oracle.");
					break;

				case 6:
					AchievementData.append("Delta Halo|Complete Delta Halo.");
					break;

				case 7:
					AchievementData.append("Regret|Complete Regret.");
					break;

				case 8:
					AchievementData.append("Sacred Icon|Complete Sacred Icon.");
					break;

				case 9:
					AchievementData.append("Quarantine Zone|Complete Quarantine Zone.");
					break;

				case 10:
					AchievementData.append("Gravemind|Complete Gravemind.");
					break;

				case 11:
					AchievementData.append("Uprising|Complete Uprising.");
					break;

				case 12:
					AchievementData.append("High Charity|Complete High Charity.");
					break;

				case 13:
					AchievementData.append("The Great Journey|Complete The Great Journey.");
					break;

				case 14:
					AchievementData.append("Warrior|Complete the game on Normal.");
					break;

				case 15:
					AchievementData.append("Hero|Complete the game on Heroic.");
					break;

				case 16:
					AchievementData.append("Legend|Complete the game on Legendary.");
					break;

				case 17:
					AchievementData.append("King of the Scarab|Acquire the Scarab Gun.");
					break;

				case 18:
					AchievementData.append("Silent But Deadly|Kill 7 opponents from behind.");
					break;

				case 19:
					AchievementData.append("Demon|Complete any level without dying.");
					break;

				case 20:
					AchievementData.append("Go Ape Shit|Kill an enraged, berserk Brute by melee.");
					break;

				case 21:
					AchievementData.append("Stick It|Stick someone!");
					break;

				case 22:
					AchievementData.append("Counterpoint|Kill the sword carrier.");
					break;

				case 23:
					AchievementData.append("Carjacking|Steal an occupied vehicle.");
					break;

				case 24:
					AchievementData.append("Violent Cartographer|Play every default map.");
					break;

				case 25:
					AchievementData.append("Rainman|Play every variant with at least 3 people.");
					break;

				case 26:
					AchievementData.append("Double Kill|Kill 2 opponents within 4 seconds.");
					break;

				case 27:
					AchievementData.append("Triple Kill|Kill 3 opponents within 4 seconds.");
					break;

				case 28:
					AchievementData.append("Killtacular|Kill 4 opponents within 4 seconds.");
					break;

				case 29:
					AchievementData.append("Killing Spree|Kill 5 opponents in a row.");
					break;

				case 30:
					AchievementData.append("Running Riot|Kill 10 opponents in a row.");
					break;

				case 31:
					AchievementData.append("Sniper Kill|Get a sniper kill.");
					break;

				case 32:
					AchievementData.append("Roadkill|Run over and kill an opponent.");
					break;

				case 33:
					AchievementData.append("Bonecracker|Kill an opponent with a melee.");
					break;

				case 34:
					AchievementData.append("Assassin|Melee an opponent from behind.");
					break;

				case 35:
					AchievementData.append("Skewer Stopper|Kill sword carrier on Spree.");
					break;

				case 36:
					AchievementData.append("Vigilante|Stop another player's Killing Spree.");
					break;

				case 37:
					AchievementData.append("Air Traffic Controller|Blow up a Banshee.");
					break;

				case 38:
					AchievementData.append("Decorated Soldier|Get awarded at least 8 medals in non-team game.");
					break;

				case 39:
					AchievementData.append("Ninja|In a non-team game, kill 5 people by melee, from behind.");
					break;

				case 40:
					AchievementData.append("Flaming Ninja|Kill an opponent that has the Ninja achievement.");
					break;

				case 41:
					AchievementData.append("Hired Gun|Kill an opponent who has the Legend achievement.");
					break;

				default:
					AchievementData.append("Unknown|We're honestly not sure wtf? :");
					AchievementData.append(std::to_string(AchievementID));
					break;
				}

				h2mod->AchievementMap[AchievementData.c_str()] = false;

				std::thread(AchievementUnlock, xFakeXuid[0], AchievementID).detach();
			}
			else {
				LOG_TRACE_GAME("Achievement {} was already unlocked", AchievementID);
			}

			pAchievement++;
			count--;
		}
	}

	if (pOverlapped)
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->InternalHigh = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_SUCCESS);
		return ERROR_IO_PENDING;
	}

	return ERROR_SUCCESS;
}

// #5280: XUserCreateAchievementEnumerator
DWORD WINAPI XUserCreateAchievementEnumerator(DWORD dwTitleId, DWORD dwUserIndex, XUID xuid, DWORD dwDetailFlags, DWORD dwStartingIndex, DWORD MaxEnumerator, PDWORD pchBuffer, PHANDLE phEnum)
{
	LOG_TRACE_XLIVE("XUserCreateAchievementEnumerator (dwStartingIndex = {0}, MaxEnumerator = {1})", dwStartingIndex, MaxEnumerator);


	if (pchBuffer) *pchBuffer = MaxEnumerator * sizeof(XACHIEVEMENT_DETAILS);
	if (phEnum) *phEnum = g_dwFakeAchievementContent = CreateMutex(NULL, NULL, NULL);


	achieveinit = 0;


	LOG_TRACE_XLIVE("- Handle = {0:p}, pchBuffer = {1}", (void*)g_dwFakeAchievementContent, *pchBuffer);

	return ERROR_SUCCESS;
}
