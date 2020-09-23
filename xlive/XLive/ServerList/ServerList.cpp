#include "stdafx.h"
#include "ServerList.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Accounts\Accounts.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "curl/curl.h"

#include "H2MOD\Modules\Utils\Utils.h"

using namespace rapidjson;

ServerList serverList;
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

		std::string server_url = std::string(cartographerURL + "/live/servers/");
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

#pragma region Xbox Network Address Reading
		// TODO: this is the LAN address, may be useful in the future
		nResult->serverAddress.ina.s_addr = 0; // currently we set it to 0

		if (!doc.HasMember("xnaddr") || !doc["xnaddr"].IsUint())
		{
			BadServer(xuid, nResult, "Missing Member: xnaddr");
			return;
		}
		nResult->serverAddress.inaOnline.s_addr = htonl(doc["xnaddr"].GetUint());

		if (!doc.HasMember("dwPort"))
		{
			BadServer(xuid, nResult, "Missing Member: dwPort");
			return;
		}
		nResult->serverAddress.wPortOnline = htons(doc["dwPort"].GetUint());

		if (!doc.HasMember("abenet"))
		{
			BadServer(xuid, nResult, "Missing Member: abEnet");
			return;
		}
		const char* abEnet_str = doc["abenet"].GetString();
		if (abEnet_str == NULL)
		{
			BadServer(xuid, nResult, "abEnet == NULL");
			return;
		}
		HexStrToBytes(abEnet_str, nResult->serverAddress.abEnet, sizeof(XNADDR::abEnet));

		if (!doc.HasMember("abonline"))
		{
			BadServer(xuid, nResult, "Missing Member: abOnline");
			return;
		}
		const char* abOnline_str = doc["abonline"].GetString();
		if (abOnline_str == NULL)
		{
			BadServer(xuid, nResult, "abOnline == NULL");
			return;
		}
		HexStrToBytes(abOnline_str, nResult->serverAddress.abOnline, sizeof(XNADDR::abOnline));
#pragma endregion
		
#pragma region Xbox Transport Security Keys Reading
		if (!doc.HasMember("xnkid"))
		{
			BadServer(xuid, nResult, "Missing Member: xnkid");
			return;
		}
		const char* xnkid_str = doc["xnkid"].GetString();
		if (xnkid_str == NULL)
		{
			BadServer(xuid, nResult, "xnkid == NULL");
			return;
		}
		HexStrToBytes(xnkid_str, nResult->xnkid.ab, sizeof(XNKID));

		if (!doc.HasMember("xnkey"))
		{
			BadServer(xuid, nResult, "Missing Member: xnkey");
			return;
		}
		const char* xnkey_str = doc["xnkey"].GetString();
		if (xnkey_str == NULL)
		{
			BadServer(xuid, nResult, "xnkey == NULL");
			return;
		}
		HexStrToBytes(xnkey_str, nResult->xnkey.ab, sizeof(XNKEY));
#pragma endregion

		if (!doc.HasMember("xuid"))
		{
			BadServer(xuid, nResult, "Missing Member: xuid");
			return;
		}
		nResult->serverID = doc["xuid"].GetUint64();

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

void ServerList::GetServersFromHttp(DWORD cbBuffer, CHAR* pvBuffer, PXOVERLAPPED pOverlapped)
{
	this->ServerListDownloadRunning = true;

	addDebugText("Requesting server list");

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	if (!userSignedOnline(0))
	{
		pOverlapped->InternalLow = ERROR_NO_MORE_FILES;
		pOverlapped->InternalHigh = 0;
		pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);
		this->ServersLeftInDocumentCount = 0;

		this->ServerListDownloadRunning = false;
		return;
	}

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, std::string(cartographerURL + "/live/server_list.php"));
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);

		rapidjson::Document document;
		document.Parse(readBuffer.c_str());

		int server_count = document["servers"].Size();
		this->ServersLeftInDocumentCount = server_count;

		if (server_count * sizeof(_XLOCATOR_SEARCHRESULT) > cbBuffer) {

			this->ServersLeftInDocumentCount = -1;
			this->ServerListDownloadRunning = false;

			pOverlapped->InternalLow = ERROR_INSUFFICIENT_BUFFER;
			pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);

			return;
		}

		_XLOCATOR_SEARCHRESULT* server_buffer = reinterpret_cast<XLOCATOR_SEARCHRESULT*>(pvBuffer);

		for (auto& server : document["servers"].GetArray())
		{
			ZeroMemory(&server_buffer[this->GetTotalServers()], sizeof(XLOCATOR_SEARCHRESULT));
			QueryServerData(curl, std::stoll(server.GetString()), &server_buffer[this->GetTotalServers()]);

			this->ServersLeftInDocumentCount--;
			if (this->GetTotalServers() > 0)
			{
				pOverlapped->InternalLow = ERROR_SUCCESS;
				pOverlapped->InternalHigh = this->GetTotalServers();
				pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_SUCCESS);
			}
		}
		
		// check if we didn't find any servers
		if (this->GetTotalServers() == 0)
		{
			// if we didn't find any, let the game know
			pOverlapped->InternalLow = ERROR_SUCCESS;
			pOverlapped->InternalHigh = 0;
			pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_SUCCESS);
		}

		std::string debg_1 = "Server Count: " + std::to_string(this->GetTotalServers());
		addDebugText(debg_1.c_str());
		curl_easy_cleanup(curl);
	}

	this->ServerListDownloadRunning = false;
}

void ServerList::GetServerCounts(PXOVERLAPPED pOverlapped)
{
	std::lock_guard<std::mutex> lg(GetServerCountsMutex);
	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, std::string(cartographerURL + "/live/dedicount.php"));
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

			// we updated the results, they can be used just fine
			CountResultsUpdated = true;
		}
	}
}

int ServerList::GetServersLeft()
{
	return ServersLeftInDocumentCount;
}

int ServerList::GetTotalServers()
{
	return total_servers;
}

bool ServerList::GetRunning()
{
	return ServerListDownloadRunning;
}

void ServerList::GetServers(DWORD cbBuffer, CHAR* pvBuffer, PXOVERLAPPED pOverlapped)
{
	// check if another thread isn't running and ServersLeftInDocumentCount is in "unitialized state"
	if (!this->ServerListDownloadRunning && this->ServersLeftInDocumentCount == -1)
	{
		this->total_servers = 0;
		this->serv_thread = std::thread(&ServerList::GetServersFromHttp, this, cbBuffer, pvBuffer, pOverlapped);
		this->serv_thread.detach();
	}

	// if there are no more servers to be read from the received document, return ERROR_NO_MORE_FILES
	if (this->ServersLeftInDocumentCount == 0)
	{
		// reset ServersLeftInDocumentCount to "unitialized state"
		this->ServersLeftInDocumentCount = -1;

		// set the ERROR_NO_MORE_FILES flag to tell the game XEnumerate is done searching
		pOverlapped->InternalLow = ERROR_NO_MORE_FILES;
		pOverlapped->InternalHigh = this->GetTotalServers();
		pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);
	}
	// otherwise tell the game we are still running I/O operations
	else
	{
		pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;
		pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);
	}
}

void ServerList::RemoveServer(PXOVERLAPPED pOverlapped)
{
	std::lock_guard<std::mutex> lk(RemoveServerMutex);

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;
	pOverlapped->InternalHigh = 1;
	pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);

	curl = curl_easy_init();
	if (curl)
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

		curl_easy_setopt(curl, CURLOPT_URL, std::string(cartographerURL + "/live/del_server.php"));
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

void ServerList::AddServer(DWORD dwUserIndex, DWORD dwServerType, XNKID xnkid, XNKEY xnkey, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, DWORD dwFilledPublicSlots, DWORD dwFilledPrivateSlots, DWORD cProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped)
{
	std::lock_guard<std::mutex> lk(AddServerMutex);

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

		Value xnkid_val(kStringType);
		xnkid_val.SetString(ByteToHexStr(xnkid.ab, sizeof(XNKID)).c_str(), document.GetAllocator());

		Value xnkey_val(kStringType);
		xnkey_val.SetString(ByteToHexStr(xnkey.ab, sizeof(XNKEY)).c_str(), document.GetAllocator());

		document.AddMember("token", token, document.GetAllocator());
		document.AddMember("xuid", Value().SetUint64(usersSignInInfo[dwUserIndex].xuid), document.GetAllocator());
		document.AddMember("dwServerType", Value().SetInt(dwServerType), document.GetAllocator());
		document.AddMember("dwMaxPublicSlots", Value().SetInt(dwMaxPublicSlots), document.GetAllocator());
		document.AddMember("dwFilledPublicSlots", Value().SetInt(dwFilledPublicSlots), document.GetAllocator());
		document.AddMember("dwMaxPrivateSlots", Value().SetInt(dwMaxPrivateSlots), document.GetAllocator());
		document.AddMember("dwMaxFilledPrivateSlots", Value().SetInt(dwFilledPrivateSlots), document.GetAllocator());
		document.AddMember("dwPort", Value().SetInt(H2Config_base_port), document.GetAllocator());
		document.AddMember("xnkid", xnkid_val, document.GetAllocator());
		document.AddMember("xnkey", xnkey_val, document.GetAllocator());
		document.AddMember("cProperties", Value().SetInt(cProperties + 3), document.GetAllocator());
		document.AddMember("pProperties", Value().SetArray(), document.GetAllocator());

		for (DWORD i = 0; i < cProperties; i++)
		{
			if (pProperties[i].dwPropertyId == XUSER_PROPERTY_USERNAME_2) // this data isn't set right by the game
				continue;

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

		char* name = strnlen_s(H2Config_dedi_server_name, XUSER_MAX_NAME_LENGTH) > 0 ? H2Config_dedi_server_name : usersSignInInfo[dwUserIndex].szUserName;

		/* For whatever reason the game is currently refusing to send the servername or player profile name so we're going to send it ourselves.*/
		Value serv_name_property(kObjectType);
		serv_name_property.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_SERVER_NAME), document.GetAllocator());
		serv_name_property.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_UNICODE), document.GetAllocator());
		serv_name_property.AddMember("value", Value().SetString(name, strnlen_s(name, XUSER_MAX_NAME_LENGTH), document.GetAllocator()), document.GetAllocator());

		Value user_name_property(kObjectType);
		user_name_property.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_USERNAME_2), document.GetAllocator());
		user_name_property.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_UNICODE), document.GetAllocator());
		user_name_property.AddMember("value", Value().SetString(name, strnlen_s(name, XUSER_MAX_NAME_LENGTH), document.GetAllocator()), document.GetAllocator());

		document["pProperties"].PushBack(user_name_property, document.GetAllocator());
		document["pProperties"].PushBack(serv_name_property, document.GetAllocator());

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

		curl_easy_setopt(curl, CURLOPT_URL, std::string(cartographerURL + "/live/add_server.php"));
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
		std::thread(&ServerList::AddServer, &serverList, dwUserIndex, dwServerType, xnkid, xnkey, dwMaxPublicSlots, dwMaxPrivateSlots, dwFilledPublicSlots, dwFilledPrivateSlots, cProperties, pProperties, pOverlapped).detach();
		return HRESULT_FROM_WIN32(ERROR_IO_PENDING);
	}
	else
		return -1;
}

DWORD WINAPI XLocatorServerUnAdvertise(DWORD dwUserIndex, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XLocatorServerUnAdvertise()");
	if (userSignedOnline(dwUserIndex))
	{
		std::thread(&ServerList::RemoveServer, &serverList, pOverlapped).detach();
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

	// hacky but does the job, because the memory we are writing to is not global (at least in H2v)
	if (userSignedOnline(dwUserIndex))
		std::thread(&ServerList::GetServerCounts, &serverList, pOverlapped).detach();

	// we simply just give the game the results synchronously, if we have any
	// it'll query the data each 5 seconds
	if (serverList.CountResultsUpdated)
	{
		pProperties[0].value.nData = serverList.total_count;
		pProperties[1].value.nData = serverList.total_public;
		pProperties[2].value.nData = serverList.total_peer_gold;
		pProperties[3].value.nData = serverList.total_peer;
	}

	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->InternalHigh = 0;
	pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_SUCCESS);

	return S_OK;
}


// 5234: ??
DWORD WINAPI XLocatorCreateServerEnumerator(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, DWORD* pcbBuffer, PHANDLE phEnum)
{
	LOG_TRACE_XLIVE("XLocatorCreateServerEnumerator()");

	*pcbBuffer = (DWORD)(sizeof(_XLOCATOR_SEARCHRESULT) * 200); // 200 is the maximum XLocator could hold per title

	if (phEnum)
	{
		serverList.Handle = *phEnum = CreateMutex(NULL, NULL, NULL);

		LOG_TRACE_XLIVE("- Handle = {:p}", (void*)*phEnum);
	}

	// not done - error now
	return ERROR_SUCCESS;
}


// 5238: ??
DWORD WINAPI XLocatorCreateKey(XNKID* pxnkid, XNKEY* pxnkey)
{
	LOG_TRACE_XLIVE("XLocatorCreateKey()");
	if (pxnkid && pxnkey) {
		XNetCreateKey(pxnkid, pxnkey);

		pxnkid->ab[0] &= ~XNET_XNKID_MASK;
		pxnkid->ab[0] |= XNET_XNKID_ONLINE_PEER;
	}

	return S_OK;
}


// 5235: ??
DWORD WINAPI XLocatorCreateServerEnumeratorByIDs(DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5, DWORD a6, DWORD a7, DWORD a8)
{
	LOG_TRACE_XLIVE("XLocatorCreateServerEnumeratorByIDs()");
	// not done - error now
	return ERROR_INVALID_PARAMETER;
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
