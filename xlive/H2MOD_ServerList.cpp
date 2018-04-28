#include <Winsock2.h>
#include <windows.h>
#include "H2MOD.h"
#include "H2MOD_ServerList.h"
#include "xlivedefs.h"
#include <string>
#include "H2OnscreenDebugLog.h"
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include <sstream>
#include <algorithm>

#include "xliveless.h"
using namespace rapidjson;

extern CHAR g_szUserName[4][16 + 1];
extern unsigned short H2Config_base_port;

using namespace std;

extern XUID xFakeXuid[4];

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void QueryServerData(ULONGLONG xuid,_XLOCATOR_SEARCHRESULT* nResult)
{	
	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl) {

		std::string server_url = std::string("http://home.thedefaced.org/servers/");
		server_url.append(std::to_string(xuid).c_str());

		curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		rapidjson::Document doc;
		doc.Parse(readBuffer.c_str());

		nResult->dwMaxPublicSlots = doc["dwMaxPublicSlots"].GetInt();
		nResult->dwFilledPublicSlots = doc["dwFilledPublicSlots"].GetInt();
		nResult->dwMaxPrivateSlots = doc["dwMaxPrivateSlots"].GetInt();
		nResult->dwFilledPrivateSlots = doc["dwMaxFilledPrivateSlots"].GetInt();
		nResult->dwServerType = doc["dwServerType"].GetInt();
		
		nResult->serverAddress.inaOnline.s_addr = doc["saddr"].GetInt();
		nResult->serverAddress.ina.s_addr = htonl(doc["xnaddr"].GetInt());

		nResult->serverAddress.wPortOnline = htons(doc["dwPort"].GetInt());

		const char* abEnet = doc["abenet"].GetString();
		for (int i = 0; i < 6; i++) {
			sscanf(&abEnet[i * 2], "%2hhx", &nResult->serverAddress.abEnet[i]);
		}
		
		const char* abOnline = doc["abonline"].GetString();
		for (int i = 0; i < 20; i++) {
			sscanf(&abOnline[i * 2], "%2hhx", &nResult->serverAddress.abOnline[i]);
		}
	
		nResult->serverID = doc["xuid"].GetInt64();

		memset(&nResult->xnkid, 0xAB, sizeof(XNKID));
		memset(&nResult->xnkey, 0xAA, sizeof(XNKEY));

		nResult->cProperties = doc["pProperties"].GetArray().Size();
		nResult->pProperties = new XUSER_PROPERTY[doc["pProperties"].GetArray().Size()];

		int current_property = 0;


		for (auto& property : doc["pProperties"].GetArray())
		{
			nResult->pProperties[current_property].dwPropertyId = property["dwPropertyId"].GetInt();
			nResult->pProperties[current_property].value.type = property["type"].GetInt();
			
			const char* data = 0;
			wchar_t *unicode_data = 0;
			int str_len = 0;
			

			GenericStringBuffer<UTF16<> > buffer;
			Writer<GenericStringBuffer<UTF16<> >, UTF8<>, UTF16<> > writer(buffer);
			std::wstring str;
			switch (property["type"].GetInt())
			{
				case XUSER_DATA_TYPE_INT32:
					nResult->pProperties[current_property].value.nData = property["value"].GetInt();
				break;

				case XUSER_DATA_TYPE_UNICODE:
					writer.String(property["value"].GetString());
					
					str.append(buffer.GetString());
					
					str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
					str.erase(std::remove(str.begin(), str.end(), '\\'), str.end());

					unicode_data = new wchar_t[str.size()+1];
					ZeroMemory(unicode_data, str.size()+1);

					wcscpy(unicode_data, str.c_str());
					
					nResult->pProperties[current_property].value.string.cbData = buffer.GetSize();
					nResult->pProperties[current_property].value.string.pwszData = unicode_data;


				break;

				case XUSER_DATA_TYPE_INT64:
					nResult->pProperties[current_property].value.i64Data = property["value"].GetInt64();
				break;
			}
			
			current_property++;
		}
		
	}
}




void GetServersFromHttp(ServerList* servptr)
{
	servptr->running = true;
	
	addDebugText("Requesting server list");

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://home.thedefaced.org/server_list.php");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		rapidjson::Document document;
		document.Parse(readBuffer.c_str());
		
		int server_count = document["servers"].Size();
		servptr->servers_left = server_count;

		servptr->servers = new _XLOCATOR_SEARCHRESULT[server_count];

		for (auto& server : document["servers"].GetArray())
		{
			ULONGLONG xuid = std::stoll(server.GetString());
			QueryServerData(xuid, &servptr->servers[servptr->servers_left - 1]);
			
			servptr->servers_left--;
		}

		servptr->total_servers = server_count;


	}
	addDebugText("Server Count:");
	addDebugText(std::to_string(servptr->total_servers).c_str());

	servptr->running = false;
	
}

int ServerList::GetServersLeft()
{
	return servers_left;
}

int ServerList::GetTotalServers()
{
	return total_servers;
}

bool ServerList::GetRunning()
{
	return running;
}

void ServerList::GetServers()
{
	if (!running)
	{
		total_servers = 0;
		this->serv_thread = std::thread(GetServersFromHttp, this);
		this->serv_thread.detach();
	}
}

DWORD WINAPI XLocatorServerAdvertise(DWORD dwUserIndex, DWORD dwServerType, XNKID *xnid, DWORD a4, DWORD a5, DWORD a6, DWORD xnaddr1, DWORD xnaddr2, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, DWORD dwFilledPublicSlots, DWORD dwFilledPrivateSlots, DWORD cProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped)
{
	//TRACE("XLocatorServerAdvertise  (*** checkme ***)");
	

	CURL *curl;
	CURLcode res;
	std::string readBuffer;


	curl = curl_easy_init();
	if (curl) {
	

		rapidjson::Document document;
		document.SetObject();
	
		document.AddMember("xuid", Value().SetUint64(xFakeXuid[0]), document.GetAllocator());
		document.AddMember("dwServerType", Value().SetInt(dwServerType), document.GetAllocator());
		document.AddMember("dwMaxPublicSlots", Value().SetInt(dwMaxPublicSlots), document.GetAllocator());
		document.AddMember("dwFilledPublicSlots", Value().SetInt(dwFilledPublicSlots), document.GetAllocator());
		document.AddMember("dwMaxPrivateSlots", Value().SetInt(dwMaxPrivateSlots), document.GetAllocator());
		document.AddMember("dwMaxFilledPrivateSlots", Value().SetInt(dwFilledPrivateSlots), document.GetAllocator());
		document.AddMember("dwPort", Value().SetInt(H2Config_base_port), document.GetAllocator());
		document.AddMember("cProperties", Value().SetInt(cProperties+3), document.GetAllocator());
		document.AddMember("pProperties", Value().SetArray(), document.GetAllocator());
		
		for (int i = 0; i < cProperties; i++)
		{
			Value property(kObjectType);
			property.AddMember("dwPropertyId", Value().SetInt(pProperties[i].dwPropertyId), document.GetAllocator());
			property.AddMember("type", Value().SetInt(pProperties[i].value.type), document.GetAllocator());

			switch (pProperties[i].value.type)
			{
				case XUSER_DATA_TYPE_INT32:
					property.AddMember("value", Value().SetInt(pProperties[i].value.nData), document.GetAllocator());
				break;

				case XUSER_DATA_TYPE_INT64:
					property.AddMember("value", Value().SetInt64(pProperties[i].value.i64Data), document.GetAllocator());
				break;
				

				case XUSER_DATA_TYPE_UNICODE:
					StringBuffer uni_input;
					Writer< StringBuffer, UTF16<> > writerUTF16(uni_input); // UTF-16 input
					writerUTF16.String(pProperties[i].value.string.pwszData);

					property.AddMember("value", Value().SetString(uni_input.GetString(),document.GetAllocator()), document.GetAllocator());
				break;

			}
			document["pProperties"].PushBack(property, document.GetAllocator());
			

		}

		/* For whatever reason the game is currently refusing to send the servername or player profile name so we're going to send it ourselves.*/
		Value serv_name(kObjectType);
		serv_name.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_SERVER_NAME), document.GetAllocator());
		serv_name.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_UNICODE), document.GetAllocator());
		serv_name.AddMember("value", Value().SetString(g_szUserName[0],strlen(g_szUserName[0]),document.GetAllocator()), document.GetAllocator());

		document["pProperties"].PushBack(serv_name, document.GetAllocator());

		Value user_name(kObjectType);
		user_name.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_USERNAME_2), document.GetAllocator());
		user_name.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_UNICODE), document.GetAllocator());
		user_name.AddMember("value", Value().SetString(g_szUserName[0],strlen(g_szUserName[0]),document.GetAllocator()), document.GetAllocator());

		document["pProperties"].PushBack(user_name, document.GetAllocator());

		Value user_int(kObjectType);
		user_int.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_USER_INT), document.GetAllocator());
		user_int.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_INT32), document.GetAllocator());
		user_int.AddMember("value", Value().SetInt(2), document.GetAllocator());

		document["pProperties"].PushBack(user_int, document.GetAllocator());

		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		document.Accept(writer);
		addDebugText(buffer.GetString());

		
		curl_easy_setopt(curl, CURLOPT_URL, "http://home.thedefaced.org/add_server.php");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer.GetString());
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		
	}
	// not done - error now
	return S_OK;
}