#include "stdafx.h"
#include "Achievements.h"

#include "cartographer/config/endpoints.h"
#include "networking/network_event.h"

#include "H2MOD/Modules/Accounts/Accounts.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

/* constants */

static const char k_cartographer_unlock_url[] = k_cartographer_url_https"/achievement-api/unlock.php";
static const char k_cartographer_achivement_list_url[] = k_cartographer_url_https"/achievement-api/achievement_list.php?xuid=";

bool g_achievement_list[42];

using namespace rapidjson;


std::unordered_map<std::string, bool> AchievementMap;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void AchievementUnlock(unsigned long long xuid, int achievement_id, XOVERLAPPED* pOverlapped)
{
	event(_event_status, "h2mod:achievements: - Unlocking achievement ID: %d", achievement_id);

	CURL *curl;
	CURLcode res;
	std::string readBuffer;


	curl = curl_interface_init_no_verify();
	if (curl) {
		rapidjson::Document document;
		document.SetObject();


		Value token(kStringType);
		token.SetString(H2CurrentAccountLoginToken, document.GetAllocator());
		document.AddMember("token", token, document.GetAllocator());
		document.AddMember("id", achievement_id, document.GetAllocator());
		document.AddMember("xuid", Value().SetUint64(xuid), document.GetAllocator());


		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		document.Accept(writer);

		curl_easy_setopt(curl, CURLOPT_URL, k_cartographer_unlock_url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer.GetString());
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	return;
}

// TODO: add extra verification for json returned from dedi
void GetAchievements(unsigned long long xuid)
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_interface_init_no_verify();
	if (curl)
	{
		c_static_string<512> server_url(k_cartographer_achivement_list_url);
		server_url.append(std::to_string(xuid).c_str());
		curl_easy_setopt(curl, CURLOPT_URL, server_url.get_string());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		rapidjson::Document document;
		document.Parse(readBuffer.c_str());

		csmemset(g_achievement_list, false, sizeof(g_achievement_list));
		if (!document.HasParseError() && document.HasMember("achievements"))
		{
			for (rapidjson::Value& achievement : document["achievements"].GetArray())
			{
				// TODO: remove string conversion once master is updated and just do achievement.GetUint() 
				if (achievement.IsString() || achievement.IsUint())
				{
					const uint32 id = achievement.IsString() ? (uint32)std::stoll(achievement.GetString()) : achievement.GetUint();
					if (VALID_INDEX(id, NUMBEROF(g_achievement_list)))
					{
						g_achievement_list[id] = true;
					}
					else
					{
						error(_error_immediate, "invalid achievement index provided: %d", id);
					}
				}
				else
				{
					error(_error_immediate, "invalid data type in achievements array");
				}
			}
		}
	}

	return;
}