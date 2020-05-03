#include "stdafx.h"
#include "Globals.h"
#include "ServerList.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Accounts\Accounts.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "curl/curl.h"

using namespace rapidjson;

ServerList serverList;
HANDLE ServerEnumHandle = INVALID_HANDLE_VALUE;
extern unsigned short H2Config_base_port;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void BadServer(ULONGLONG xuid, _XLOCATOR_SEARCHRESULT* nResult, const char* log_catch)
{
	LOG_TRACE_GAME("BadServer - XUID: {0} - Log Catch: {1}", xuid, log_catch);
	SecureZeroMemory(nResult, sizeof(_XLOCATOR_SEARCHRESULT));
}

void QueryServerData(CURL* curl, ULONGLONG xuid, _XLOCATOR_SEARCHRESULT* nResult)
{
	CURLcode res;
	std::string readBuffer;

	if (curl) {

		std::string server_url = std::string("http://cartographer.online/live/servers/");
		server_url.append(std::to_string(xuid).c_str());

		curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);

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

		XLocatorCreateKey(&nResult->xnkid, &nResult->xnkey);

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

				unicode_data = new wchar_t[str.size() + 1];
				SecureZeroMemory(unicode_data, str.size() + 1);

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

	++serverList.total_servers;
}

void GetServersFromHttp(ServerList* servptr, DWORD cbBuffer, CHAR* pvBuffer, PXOVERLAPPED pOverlapped)
{
	servptr->server_list_download_running = true;

	addDebugText("Requesting server list");

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	if (!userSignedOnline(0))
	{
		pOverlapped->InternalLow = ERROR_NO_MORE_FILES;
		pOverlapped->InternalHigh = HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);
		pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);
		servptr->servers_left = 0;

		servptr->server_list_download_running = false;
		return;
	}

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://cartographer.online/live/server_list.php");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);

		rapidjson::Document document;
		document.Parse(readBuffer.c_str());

		int server_count = document["servers"].Size();
		servptr->servers_left = server_count;

		if (server_count * sizeof(_XLOCATOR_SEARCHRESULT) > cbBuffer) {

			servptr->servers_left = -1;
			servptr->server_list_download_running = false;

			pOverlapped->InternalLow = ERROR_INSUFFICIENT_BUFFER;
			pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);

			return;
		}

		_XLOCATOR_SEARCHRESULT* server_buffer = reinterpret_cast<XLOCATOR_SEARCHRESULT*>(pvBuffer);

		for (auto& server : document["servers"].GetArray())
		{
			ULONGLONG xuid = std::stoll(server.GetString());
			QueryServerData(curl, xuid, &server_buffer[servptr->GetTotalServers()]);

			servptr->servers_left--;
			pOverlapped->InternalLow = ERROR_SUCCESS;
			pOverlapped->InternalHigh = servptr->GetTotalServers();
			pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_SUCCESS);
		}

		std::string debg_1 = "Server Count: " + std::to_string(servptr->GetTotalServers());
		addDebugText(debg_1.c_str());
		curl_easy_cleanup(curl);
	}

	servptr->server_list_download_running = false;
}

void ServerList::GetServerCounts()
{
	this->server_counts_download_running = true;
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
		}
	}
	this->server_counts_download_running = false;
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
	return server_list_download_running;
}

void ServerList::GetServers(DWORD cbBuffer, CHAR* pvBuffer, PXOVERLAPPED pOverlapped)
{
	// check if another thread isn't running and servers_left is in "unitialized state"
	if (!this->server_list_download_running && this->servers_left == -1)
	{
		this->total_servers = 0;
		this->serv_thread = std::thread(GetServersFromHttp, this, cbBuffer, pvBuffer, pOverlapped);
		this->serv_thread.detach();
	}

	if (this->servers_left == 0)
	{
		// reset servers_left to "unitialized state"
		this->servers_left = -1;

		// set the ERROR_NO_MORE_FILES flag to tell the game XEnumerate is done searching
		pOverlapped->InternalLow = ERROR_NO_MORE_FILES;
		pOverlapped->InternalHigh = this->GetTotalServers();
		pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);
	}
	else
	{
		pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;
		pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);
	}
}

void RemoveServer(PXOVERLAPPED pOverlapped)
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;
	pOverlapped->InternalHigh = 1;
	pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);

	curl = curl_easy_init();
	if (curl && userSignedOnline(0))
	{
		rapidjson::Document document;
		document.SetObject();
		document.AddMember("xuid", Value().SetUint64(usersSignInInfo[0].xuid), document.GetAllocator());

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

	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->InternalHigh = 0;
	pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_SUCCESS);
}

void AddServer(DWORD dwUserIndex, DWORD dwServerType, XNKID xnkid, XNKEY xnkey, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, DWORD dwFilledPublicSlots, DWORD dwFilledPrivateSlots, DWORD cProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped)
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;
	pOverlapped->InternalHigh = 1;
	pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);

	curl = curl_easy_init();
	if (curl) {


		rapidjson::Document document;
		document.SetObject();


		Value token(kStringType);
		if (H2CurrentAccountLoginToken)
			token.SetString(H2CurrentAccountLoginToken, document.GetAllocator());

		document.AddMember("token", token, document.GetAllocator());
		document.AddMember("xuid", Value().SetUint64(usersSignInInfo[dwUserIndex].xuid), document.GetAllocator());
		document.AddMember("dwServerType", Value().SetInt(dwServerType), document.GetAllocator());
		document.AddMember("dwMaxPublicSlots", Value().SetInt(dwMaxPublicSlots), document.GetAllocator());
		document.AddMember("dwFilledPublicSlots", Value().SetInt(dwFilledPublicSlots), document.GetAllocator());
		document.AddMember("dwMaxPrivateSlots", Value().SetInt(dwMaxPrivateSlots), document.GetAllocator());
		document.AddMember("dwMaxFilledPrivateSlots", Value().SetInt(dwFilledPrivateSlots), document.GetAllocator());
		document.AddMember("dwPort", Value().SetInt(H2Config_base_port), document.GetAllocator());
		document.AddMember("cProperties", Value().SetInt(cProperties + 3), document.GetAllocator());
		document.AddMember("pProperties", Value().SetArray(), document.GetAllocator());

		for (DWORD i = 0; i < cProperties; i++)
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
		if (h2mod->Server && strlen(H2Config_dedi_server_name) != 0)
			serv_name.AddMember("value", Value().SetString(H2Config_dedi_server_name, strlen(H2Config_dedi_server_name), document.GetAllocator()), document.GetAllocator());
		else
			serv_name.AddMember("value", Value().SetString(usersSignInInfo[dwUserIndex].szUserName, XUSER_NAME_SIZE, document.GetAllocator()), document.GetAllocator());

		document["pProperties"].PushBack(serv_name, document.GetAllocator());

		Value user_name(kObjectType);
		user_name.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_USERNAME_2), document.GetAllocator());
		user_name.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_UNICODE), document.GetAllocator());
		if (h2mod->Server && strlen(H2Config_dedi_server_name) != 0)
			user_name.AddMember("value", Value().SetString(H2Config_dedi_server_name, strlen(H2Config_dedi_server_name), document.GetAllocator()), document.GetAllocator());
		else
			user_name.AddMember("value", Value().SetString(usersSignInInfo[dwUserIndex].szUserName, XUSER_NAME_SIZE, document.GetAllocator()), document.GetAllocator());

		document["pProperties"].PushBack(user_name, document.GetAllocator());

		Value user_int(kObjectType);
		user_int.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_USER_INT), document.GetAllocator());
		user_int.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_INT32), document.GetAllocator());
		user_int.AddMember("value", Value().SetInt(2), document.GetAllocator());
		document["pProperties"].PushBack(user_int, document.GetAllocator());

		Value user_xuid(kObjectType);
		user_xuid.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_XUID), document.GetAllocator());
		user_xuid.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_INT64), document.GetAllocator());
		user_xuid.AddMember("value", Value().SetUint64(usersSignInInfo[dwUserIndex].xuid), document.GetAllocator());
		document["pProperties"].PushBack(user_xuid, document.GetAllocator());

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

	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->InternalHigh = 0;
	pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_SUCCESS);
}

DWORD WINAPI XLocatorServerAdvertise(DWORD dwUserIndex, DWORD dwServerType, XNKID xnkid, XNKEY xnkey, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, DWORD dwFilledPublicSlots, DWORD dwFilledPrivateSlots, DWORD cProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped)
{
	if (userSignedOnline(dwUserIndex))
	{
		std::thread(AddServer, dwUserIndex, dwServerType, xnkid, xnkey, dwMaxPublicSlots, dwMaxPrivateSlots, dwFilledPublicSlots, dwFilledPrivateSlots, cProperties, pProperties, pOverlapped).detach();
		return HRESULT_FROM_WIN32(ERROR_IO_PENDING);
	}
	else
		return -1;
}

DWORD WINAPI XLocatorServerUnAdvertise(DWORD dwUserIndex, PXOVERLAPPED pOverlapped)
{
	if (userSignedOnline(dwUserIndex))
	{
		std::thread(RemoveServer, pOverlapped).detach();
		return HRESULT_FROM_WIN32(ERROR_IO_PENDING);
	}
	else
		return -1;
}

// 5233: ??
DWORD WINAPI XLocatorGetServiceProperty(DWORD dwUserIndex, DWORD cNumProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped)
{
	// LOG_TRACE_XLIVE("XLocatorGetServiceProperty  (*** checkme ***) (dwUserIndex = {0:x}, cNumProperties = {1:x}, pProperties = {2:x}, pOverlapped = {3:x})",
	//		dwUserIndex, cNumProperties, pProperties, pOverlapped);

	if (!serverList.server_counts_download_running && userSignedOnline(dwUserIndex))
		std::thread(&ServerList::GetServerCounts, &serverList).detach();

	pProperties[0].value.nData = serverList.total_count;
	pProperties[1].value.nData = serverList.total_public;
	pProperties[2].value.nData = serverList.total_peer_gold + serverList.total_public_gold != -2 ? serverList.total_peer_gold + serverList.total_public_gold : -1;
	pProperties[3].value.nData = serverList.total_peer;

	return S_OK;
}


// 5234: ??
DWORD WINAPI XLocatorCreateServerEnumerator(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, DWORD* pcbBuffer, PHANDLE phEnum)
{
	LOG_TRACE_XLIVE("XLocatorCreateServerEnumerator");

	*pcbBuffer = (DWORD)(sizeof(_XLOCATOR_SEARCHRESULT) * 200); // 200 is the maximum XLocator could hold per title

	if (phEnum)
	{
		*phEnum = CreateMutex(NULL, NULL, NULL);

		LOG_TRACE_XLIVE("- Handle = {:p}", (void*)*phEnum);
		ServerEnumHandle = *phEnum;
	}

	// not done - error now
	return ERROR_SUCCESS;
}


// 5238: ??
DWORD WINAPI XLocatorCreateKey(XNKID* pxnkid, XNKEY* pxnkey)
{
	LOG_TRACE_XLIVE("XLocatorCreateKey");
	if (pxnkid && pxnkey) {
		//XNetCreateKey(pxnkid, pxnkey);
		
		memset(pxnkid, 0xAB, sizeof(XNKID));
		memset(pxnkey, 0xAA, sizeof(XNKEY));

		pxnkid->ab[0] &= ~XNET_XNKID_MASK;
		pxnkid->ab[0] |= XNET_XNKID_ONLINE_PEER;
	}

	return S_OK;
}


// 5235: ??
DWORD WINAPI XLocatorCreateServerEnumeratorByIDs(DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5, DWORD a6, DWORD a7, DWORD a8)
{
	LOG_TRACE_XLIVE("XLocatorCreateServerEnumeratorByIDs");
	// not done - error now
	return 0x57;
}


// 5236: ??
DWORD WINAPI XLocatorServiceInitialize(DWORD a1, PHANDLE phLocatorService)
{
	LOG_TRACE_XLIVE("XLocatorServiceInitialize  (a1 = {:x}, phLocatorService = {:p})",
		a1, (void*)phLocatorService);

	if (phLocatorService)
		*phLocatorService = CreateMutex(NULL, NULL, NULL);

	/*
	Lost Planet, Gears of War
	- LocatorV1.434307DE.RTP.
	*/

	return ERROR_SUCCESS;
}


// 5237: ??
DWORD WINAPI XLocatorServiceUnInitialize(HANDLE xlocatorhandle)
{
	LOG_TRACE_XLIVE("XLocatorServiceUnInitialize(a1 = {})", xlocatorhandle);
	CloseHandle(xlocatorhandle);
	return ERROR_SUCCESS;
}
