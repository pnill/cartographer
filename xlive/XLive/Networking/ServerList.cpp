#include <Winsock2.h>
#include <windows.h>
#include "H2MOD.h"
#include "XLive\Networking\ServerList.h"
#include "xlivedefs.h"
#include <string>
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include <sstream>
#include <algorithm>
#include "Globals.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Accounts\Accounts.h"
#include "XLive\XUser\XUserContext.h"
#include "XLive\XUSer\XUserProperty.h"
#include "XLive\XUser\XUser.h"
#include "xliveless.h"

using namespace rapidjson;

extern CHAR g_szUserName[4][16];
extern unsigned short H2Config_base_port;

using namespace std;

HANDLE ServerEnum = NULL;
bool ServerEnumRan = false;
ServerList LiveManager;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void BadServer(ULONGLONG xuid, _XLOCATOR_SEARCHRESULT* nResult,const char* log_catch)
{
	if (H2Config_debug_log)
		TRACE_GAME_N("BadServer - XUID: %llu - Log Catch: %s", xuid,log_catch);

	ZeroMemory(nResult, sizeof(_XLOCATOR_SEARCHRESULT));
}
void QueryServerData(ULONGLONG xuid,_XLOCATOR_SEARCHRESULT* nResult)
{	
	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl) {

		std::string server_url = std::string("http://cartographer.online/live/servers/");
		server_url.append(std::to_string(xuid).c_str());

		curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		rapidjson::Document doc;
		doc.Parse(readBuffer.c_str());

		if (!doc.HasMember("dwMaxPublicSlots")) {
			BadServer(xuid, nResult, "Missing Member: dwMaxPublicSlots");
			return;
		}

		nResult->dwMaxPublicSlots = doc["dwMaxPublicSlots"].GetUint();

		if (!doc.HasMember("dwFilledPublicSlots"))
		{
			BadServer(xuid, nResult, "Missing Member: dwFilledPublicSlots");
			return;
		}

		nResult->dwFilledPublicSlots = doc["dwFilledPublicSlots"].GetUint();

		if (!doc.HasMember("dwMaxPrivateSlots"))
		{
			BadServer(xuid, nResult, "Missing Member: dwFilledPublicSlots");
			return;
		}

		nResult->dwMaxPrivateSlots = doc["dwMaxPrivateSlots"].GetUint();


		if (!doc.HasMember("dwMaxFilledPrivateSlots"))
		{
			BadServer(xuid, nResult, "Missing Member: dwMaxFilledPrivateSlots");
			return;
		}
		nResult->dwFilledPrivateSlots = doc["dwMaxFilledPrivateSlots"].GetUint();

		if (!doc.HasMember("dwServerType"))
		{
			BadServer(xuid, nResult, "Missing Member: dwServerType");
			return;
		}
		nResult->dwServerType = doc["dwServerType"].GetUint();


		if (!doc.HasMember("saddr") || !doc["saddr"].IsUint())
		{
			BadServer(xuid, nResult, "Missing Member: saddr");
			return;
		}
		nResult->serverAddress.inaOnline.s_addr = doc["saddr"].GetUint();

		if (!doc.HasMember("xnaddr") || !doc["xnaddr"].IsUint())
		{
			BadServer(xuid, nResult, "Missing Member: xnaddr");
			return;
		}
		nResult->serverAddress.ina.s_addr = htonl(doc["xnaddr"].GetUint());

		if (!doc.HasMember("dwPort"))
		{
			BadServer(xuid, nResult, "Missing Member: dwPort");
			return;
		}
		nResult->serverAddress.wPortOnline = htons(doc["dwPort"].GetUint());


		if (!doc.HasMember("abenet"))
		{
			BadServer(xuid, nResult, "Missing Member: abenet");
			return;
		}

		const char* abEnet = doc["abenet"].GetString();
		if (abEnet == NULL)
		{
			BadServer(xuid, nResult, "abEnet = NULL");
			return;
		}

		for (int i = 0; i < 6; i++) {
			sscanf(&abEnet[i * 2], "%2hhx", &nResult->serverAddress.abEnet[i]);
		}
		

		if (!doc.HasMember("abonline"))
		{
			BadServer(xuid, nResult, "Missing Member: abOnline");
				return;
		}

		const char* abOnline = doc["abonline"].GetString();
		if (abOnline == NULL)
		{
			BadServer(xuid, nResult, "abonline == NULL");
			return;
		}

		for (int i = 0; i < 20; i++) {
			sscanf(&abOnline[i * 2], "%2hhx", &nResult->serverAddress.abOnline[i]);
		}
	
		if (!doc.HasMember("xuid"))
		{
			BadServer(xuid, nResult, "Missing Member: xuid");
			return;
		}
		nResult->serverID = doc["xuid"].GetUint64();

		memset(&nResult->xnkid, 0xAB, sizeof(XNKID));
		memset(&nResult->xnkey, 0xAA, sizeof(XNKEY));

		if (!doc.HasMember("pProperties") || !doc.HasMember("cProperties"))
		{
			BadServer(xuid, nResult, "Missing Member: cProperties or pProperties");
			return;
		}

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




void GetServersFromHttp(ServerList* servptr,PXOVERLAPPED pOverlapped, char* pvBuffer)
{
	servptr->running = true;
	
	addDebugText("Requesting server list");

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://cartographer.online/live/server_list.php");
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

	/* Copy all the servers into the buffer originally passed in XEnumerate */
	for (int i = 0; i < servptr->total_servers; i++)
	{
		memcpy(pvBuffer + (sizeof(_XLOCATOR_SEARCHRESULT) * i), &servptr->servers[i], sizeof(_XLOCATOR_SEARCHRESULT));
	}

	delete[] servptr->servers;

	pOverlapped->InternalHigh = servptr->total_servers;
	pOverlapped->InternalLow = ERROR_SUCCESS;

	servptr->running = false;
	
}


bool ServerList::GetServerCounts()
{

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://cartographer.online/live/dedicount.php");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		rapidjson::Document document;
		document.Parse(readBuffer.c_str());


		if (document.HasMember("public_count"))
		{
			total_count = document["total"].GetInt();
			total_peer = document["peer_count"].GetInt();
			total_peer_gold = document["peer_gold"].GetInt();
			total_public = document["public_count"].GetInt();
			total_public_gold = document["public_gold"].GetInt();

			return true;
		}

		return false;
	}

	return false;
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

void ServerList::GetServers(PXOVERLAPPED pOverlapped, char* pvBuffer)
{
	if (!running)
	{

		/* Set the overlapped routing to ERROR_IO_COMPLETE while we start the operation. */
		pOverlapped->InternalHigh = ERROR_IO_INCOMPLETE;
		pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;

		total_servers = 0;
		this->serv_thread = std::thread(GetServersFromHttp, this,pOverlapped,pvBuffer);
		this->serv_thread.detach();
	}
}


void RemoveServer()
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl)
	{
		rapidjson::Document document;
		document.SetObject();
		document.AddMember("xuid", Value().SetUint64(xFakeXuid[0]), document.GetAllocator());

		Value token(kStringType);
		token.SetString(H2CurrentAccountLoginToken, document.GetAllocator());
		document.AddMember("token", token, document.GetAllocator());


		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		document.Accept(writer);

		curl_easy_setopt(curl, CURLOPT_URL, "http://cartographer.online/live/del_server.php");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer.GetString());
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

	}
}

DWORD WINAPI XLocatorServerUnAdvertise(DWORD a1, DWORD a2)
{
	std::thread(RemoveServer).detach();

	return S_OK;
}

void ServerList::RemoveServer()
{
	XLocatorServerUnAdvertise(1, 1);
}

void AddServer(DWORD dwUserIndex, DWORD dwServerType, XNKID *xnid, DWORD a4, DWORD a5, DWORD a6, DWORD xnaddr1, DWORD xnaddr2, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, DWORD dwFilledPublicSlots, DWORD dwFilledPrivateSlots, DWORD cProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped)
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;


	curl = curl_easy_init();
	if (curl) {


		rapidjson::Document document;
		document.SetObject();


		Value token(kStringType);
		token.SetString(H2CurrentAccountLoginToken, document.GetAllocator());

		document.AddMember("token", token, document.GetAllocator());
		document.AddMember("xuid", Value().SetUint64(xFakeXuid[0]), document.GetAllocator());
		document.AddMember("dwServerType", Value().SetInt(dwServerType), document.GetAllocator());
		document.AddMember("dwMaxPublicSlots", Value().SetInt(dwMaxPublicSlots), document.GetAllocator());
		document.AddMember("dwFilledPublicSlots", Value().SetInt(dwFilledPublicSlots), document.GetAllocator());
		document.AddMember("dwMaxPrivateSlots", Value().SetInt(dwMaxPrivateSlots), document.GetAllocator());
		document.AddMember("dwMaxFilledPrivateSlots", Value().SetInt(dwFilledPrivateSlots), document.GetAllocator());
		document.AddMember("dwPort", Value().SetInt(H2Config_base_port), document.GetAllocator());
		document.AddMember("cProperties", Value().SetInt(cProperties + 3), document.GetAllocator());
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

				property.AddMember("value", Value().SetString(uni_input.GetString(), document.GetAllocator()), document.GetAllocator());
				break;

			}
			document["pProperties"].PushBack(property, document.GetAllocator());


		}

		/* For whatever reason the game is currently refusing to send the servername or player profile name so we're going to send it ourselves.*/
		Value serv_name(kObjectType);
		serv_name.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_SERVER_NAME), document.GetAllocator());
		serv_name.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_UNICODE), document.GetAllocator());
		serv_name.AddMember("value", Value().SetString(g_szUserName[0], strlen(g_szUserName[0]), document.GetAllocator()), document.GetAllocator());

		document["pProperties"].PushBack(serv_name, document.GetAllocator());

		Value user_name(kObjectType);
		user_name.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_USERNAME_2), document.GetAllocator());
		user_name.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_UNICODE), document.GetAllocator());
		user_name.AddMember("value", Value().SetString(g_szUserName[0], strlen(g_szUserName[0]), document.GetAllocator()), document.GetAllocator());

		document["pProperties"].PushBack(user_name, document.GetAllocator());

		Value user_int(kObjectType);
		user_int.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_USER_INT), document.GetAllocator());
		user_int.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_INT32), document.GetAllocator());
		user_int.AddMember("value", Value().SetInt(2), document.GetAllocator());

		document["pProperties"].PushBack(user_int, document.GetAllocator());

		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		document.Accept(writer);

		curl_easy_setopt(curl, CURLOPT_URL, "http://cartographer.online/live/add_server.php");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer.GetString());
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

	}
}

DWORD WINAPI XLocatorServerAdvertise(DWORD dwUserIndex, DWORD dwServerType, XNKID *xnid, DWORD a4, DWORD a5, DWORD a6, DWORD xnaddr1, DWORD xnaddr2, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, DWORD dwFilledPublicSlots, DWORD dwFilledPrivateSlots, DWORD cProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped)
{
	std::thread(AddServer, dwUserIndex, dwServerType, xnid, a4, a5, a6, xnaddr1, xnaddr2, dwMaxPublicSlots, dwMaxPrivateSlots, dwFilledPublicSlots, dwFilledPrivateSlots, cProperties, pProperties, pOverlapped).detach();

	
	// not done - error now
	return S_OK;
}

// 5233: ??
DWORD WINAPI XLocatorGetServiceProperty(DWORD dwUserIndex, DWORD cNumProperties, PXUSER_PROPERTY pProperties, DWORD pOverlapped)
{
	// TRACE("XLocatorGetServiceProperty  (*** checkme ***) (dwUserIndex = %X, cNumProperties = %X, pProperties = %X, pOverlapped = %X)",
	//		dwUserIndex, cNumProperties, pProperties, pOverlapped);

	if (LiveManager.GetServerCounts())
	{
		pProperties[0].value.nData = LiveManager.total_count;
		pProperties[1].value.nData = LiveManager.total_public;
		pProperties[2].value.nData = LiveManager.total_peer_gold + LiveManager.total_public_gold;
		pProperties[3].value.nData = LiveManager.total_peer;

		return S_OK;
	}

	pProperties[0].value.nData = 1;
	pProperties[1].value.nData = 3;
	pProperties[2].value.nData = 3;
	pProperties[3].value.nData = 7;

	return S_OK;
}


// 5234: ??
DWORD WINAPI XLocatorCreateServerEnumerator(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, DWORD* pcbBuffer, PHANDLE phEnum)
{
	TRACE("XLocatorCreateServerEnumerator");


	*pcbBuffer = (DWORD)(sizeof(_XLOCATOR_SEARCHRESULT) * (LiveManager.total_count + 10));

	if (phEnum)
	{
		*phEnum = CreateMutex(NULL, NULL, NULL);

		TRACE("- Handle = %X", *phEnum);
		ServerEnum = *phEnum;
	}

	//PopulateList();

	// not done - error now
	return ERROR_SUCCESS;
}


// 5231: ??




// 5235: ??
DWORD WINAPI XLocatorCreateServerEnumeratorByIDs(DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5, DWORD a6, DWORD a7, DWORD a8)
{
	TRACE("XLocatorCreateServerEnumeratorByIDs");


	// not done - error now
	return 0x57;
}


// 5236: ??
DWORD WINAPI XLocatorServiceInitialize(DWORD a1, DWORD a2)
{
	TRACE("XLocatorServiceInitialize  (a1 = %X, a2 = %X)",
		a1, a2);


	/*
	Lost Planet, Gears of War
	- LocatorV1.434307DE.RTP.
	*/

#if 0
	while (1)
		Sleep(1);
#endif


	// GFWL offline
	return 0;
}


// 5237: ??
DWORD WINAPI XLocatorServiceUnInitialize(DWORD a1)
{
	TRACE("XLocatorServiceUnInitialize(a1 = %X)", a1);


	// not done - error now
	return 0x80004001;
}