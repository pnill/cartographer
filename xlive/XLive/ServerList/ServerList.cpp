#include "stdafx.h"
#include "ServerList.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Accounts\Accounts.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

#include "H2MOD\Modules\Utils\Utils.h"

using namespace rapidjson;

extern unsigned short H2Config_base_port;

std::mutex ServerListRequestInProgress;

std::mutex ServerListRequestsMutex;
std::unordered_map<HANDLE, ServerList*> serverListRequests;

std::mutex ServerList::AddServerMutex;
std::mutex ServerList::RemoveServerMutex;
std::mutex ServerList::GetServerCountsMutex;

bool ServerList::CountResultsUpdated = false;
int ServerList::total_count;
int ServerList::total_public;
int ServerList::total_peer;
int ServerList::total_peer_gold;
int ServerList::total_public_gold;

HANDLE g_hXLocatorHandle = INVALID_HANDLE_VALUE;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void BadServer(ULONGLONG xuid, XLOCATOR_SEARCHRESULT* nResult, const char* log_catch)
{
	LOG_TRACE_GAME("BadServer - XUID: {0} - Log Catch: {1}", xuid, log_catch);
	SecureZeroMemory(nResult, sizeof(XLOCATOR_SEARCHRESULT));
}

DWORD ComputeXLocatorServerEnumeratorBufferSize(DWORD cItems, DWORD cRequiredPropertyIDs, DWORD* pRequredPropertiesIDs, DWORD* outStringBufferSize)
{
	unsigned int stringProperties = 0;

	unsigned int result = (DWORD)((sizeof(XLOCATOR_SEARCHRESULT) * cItems) + (sizeof(XUSER_PROPERTY) * cRequiredPropertyIDs * cItems));

	if (pRequredPropertiesIDs)
	{
		for (int i = 0; i < cRequiredPropertyIDs; i++)
		{
			if ((pRequredPropertiesIDs[i] & 0xF0000000) == 0x40000000) // check if we have string properties
				++stringProperties;
		}
	}

	// string buffers are max 64 wide strings characters (128 bytes) and the null character, so in total 65 characters (130 bytes)
	result += (X_PROPERTY_UNICODE_BUFFER_SIZE * cItems * stringProperties);

	if (outStringBufferSize)
	{
		*outStringBufferSize = (X_PROPERTY_UNICODE_BUFFER_SIZE * cItems * stringProperties);
		LOG_ERROR_XLIVE("{} : stringBufferSize: {}, stringBufferSize2: {}, cItems: {}, stringCount: {}", __FUNCTION__, *outStringBufferSize, (X_PROPERTY_UNICODE_BUFFER_SIZE * cItems * stringProperties), cItems, stringProperties);
	}

	return result;
}

void ServerList::QueryServerData(CURL* curl, ULONGLONG xuid, XLOCATOR_SEARCHRESULT* nResult, XUSER_PROPERTY** propertiesBuffer, WCHAR** stringBuffer)
{
	CURLcode res;
	std::string readBuffer;

	int strings = 0;

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

		nResult->cProperties = 0;
		nResult->pProperties = *propertiesBuffer;

		std::vector<DWORD> propertiesWritten;

		for (auto& property : doc["pProperties"].GetArray())
		{
			DWORD propertyId = property["dwPropertyId"].GetInt();

			bool propertyNeeded = false;

			for (int i = 0; i < cSearchPropertiesIDs; i++)
			{
				if (propertyId == pSearchPropertyIDs[i])
				{
					propertyNeeded = true;
					break;
				}
			}

			if (!propertyNeeded)
			{
				LOG_INFO_XLIVE("{} - unrequested property ID: 0x{:X}, skipping", __FUNCTION__, propertyId);
				continue;
			}

			XUSER_PROPERTY* userProperty = &nResult->pProperties[nResult->cProperties];

			ZeroMemory(userProperty, sizeof(XUSER_PROPERTY));
			userProperty->dwPropertyId = propertyId;
			userProperty->value.type = property["type"].GetInt();

			propertiesWritten.push_back(propertyId);

			const char* data = 0;
			int str_len = 0;

			std::wstring str;
			wchar_t* unicode_data = nullptr;
			GenericStringBuffer<UTF16<> > buffer;
			Writer<GenericStringBuffer<UTF16<> >, UTF8<>, UTF16<> > writer(buffer);
			switch (property["type"].GetInt())
			{
			case XUSER_DATA_TYPE_INT32:
				userProperty->value.nData = property["value"].GetInt();
				break;

			case XUSER_DATA_TYPE_UNICODE:
				unicode_data = *stringBuffer;
				writer.String(property["value"].GetString());

				str.append(buffer.GetString());
				str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
				str.erase(std::remove(str.begin(), str.end(), '\\'), str.end());

				SecureZeroMemory(unicode_data, X_PROPERTY_UNICODE_BUFFER_SIZE);

				wcscpy(*stringBuffer, str.c_str());

				userProperty->value.string.cbData = wcsnlen(unicode_data, 64) * sizeof(WCHAR) + 2;
				userProperty->value.string.pwszData = unicode_data;

				*stringBuffer = (WCHAR*)((BYTE*)(*stringBuffer) + X_PROPERTY_UNICODE_BUFFER_SIZE);
				strings++;

				break;

			case XUSER_DATA_TYPE_INT64:
				userProperty->value.i64Data = property["value"].GetInt64();
				break;

			case XUSER_DATA_TYPE_BINARY:
			case XUSER_DATA_TYPE_FLOAT:
			case XUSER_DATA_TYPE_DOUBLE:
			case XUSER_DATA_TYPE_DATETIME:
				//LOG_TRACE_XLIVE("{} - unimplemented data type");
				break;
			}

			nResult->cProperties++;

			(*propertiesBuffer)++;
		}

		for (auto property : propertiesWritten)
		{
			int i = 0;
			bool foundMatch = false;
			for (i; i < cSearchPropertiesIDs; i++)
			{
				if (pSearchPropertyIDs[i] == property)
				{
					foundMatch = true;
					break;
				}
			}

			if (!foundMatch)
			{
				LOG_ERROR_XLIVE("{} - couldn't find property: 0x{:X}", __FUNCTION__, pSearchPropertyIDs[i]);
			}
		}
	}

	//LOG_ERROR_XLIVE("{} : string count: {}, properties: {}", __FUNCTION__, strings, nResult->cProperties);

	++total_servers;
}

void ServerList::GetServersFromHttp(DWORD cbBuffer, CHAR* pvBuffer)
{
	auto cleanup = [this]()
	{
		ServerListRequestsMutex.lock();

		serverListRequests.erase(Handle);
		XCloseHandle(Handle);

		delete[] this->pSearchPropertyIDs;

		delete this;

		ServerListRequestsMutex.unlock();
	};

	// first we set the overlapped status to ERROR_IO_PENDING
	ovelapped->InternalLow = ERROR_IO_PENDING;

	while (!ServerListRequestInProgress.try_lock())
	{
		// while we try to lock the operation, check if we should also terminate the operation

		// then we check if the current pending operation hasn't been canceled by XCancelOverlapped, otherwise we abort the thread
		if (cancelOperation) { // TODO: implement XCancelOverlapped
			cleanup();
			return;
		}

		Sleep(10);
	}

	// we sucessfully locked ServerListRequestInProgress, let's get the server list

	addDebugText("Requesting server list");

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	this->total_servers = 0;

	// first we set the overlapped status to ERROR_IO_PENDING
	ovelapped->InternalLow = ERROR_IO_INCOMPLETE;

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

		DWORD outStringBufferSize = 0;

		if (ComputeXLocatorServerEnumeratorBufferSize(server_count, cSearchPropertiesIDs, pSearchPropertyIDs, &outStringBufferSize) > cbBuffer) 
		{
			curl_easy_cleanup(curl);

			this->ServersLeftInDocumentCount = 0;

			ovelapped->InternalLow = ERROR_INSUFFICIENT_BUFFER;
			ovelapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
			
			ServerListRequestInProgress.unlock();
			cleanup();

			LOG_ERROR_XLIVE("{} - insufficient buffer for I/O operation!", __FUNCTION__);
			return;
		}

		XLOCATOR_SEARCHRESULT* searchResults = reinterpret_cast<XLOCATOR_SEARCHRESULT*>(pvBuffer);

		XUSER_PROPERTY* propertiesBuffer = (XUSER_PROPERTY*)((BYTE*)searchResults + (sizeof(XLOCATOR_SEARCHRESULT) * server_count));
		WCHAR* stringBuffer = (WCHAR*)((BYTE*)propertiesBuffer + (sizeof(XUSER_PROPERTY) * cSearchPropertiesIDs * server_count));

		for (auto& server : document["servers"].GetArray())
		{
			ZeroMemory(&searchResults[this->GetTotalServers()], sizeof(XLOCATOR_SEARCHRESULT));
			LOG_ERROR_XLIVE("{} - search results ptr: 0x{:x}, properties buffer: 0x{:X}, stringBuffer: 0x{:X}", __FUNCTION__, (DWORD)searchResults, (DWORD)propertiesBuffer, (DWORD)stringBuffer);

			QueryServerData(curl, std::stoll(server.GetString()), &searchResults[this->GetTotalServers()], &propertiesBuffer, &stringBuffer);

			if (this->GetTotalServers() > 0)
			{
				ovelapped->InternalLow = ERROR_SUCCESS;
				ovelapped->InternalHigh = this->GetTotalServers();
				ovelapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_SUCCESS);
			}

			this->ServersLeftInDocumentCount--;
		}
		
		// check if we didn't find any servers
		if (this->GetTotalServers() == 0)
		{
			// if we didn't find any, let the game know
			ovelapped->InternalLow = ERROR_NO_MORE_FILES;
			ovelapped->InternalHigh = 0;
			ovelapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);
		}

		LOG_TRACE_XLIVE("{} - found a total of: {} servers", this->GetTotalServers());

		curl_easy_cleanup(curl);
	}

	ServerListRequestInProgress.unlock();
	cleanup();
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

DWORD ServerList::GetServers(HANDLE hHandle, DWORD cbBuffer, CHAR* pvBuffer, PXOVERLAPPED pOverlapped)
{
	// TODO: figure out why this crap is needed, because the other enumeration overlapped results are handled by another thread if it's the case

	std::lock_guard<std::mutex> lg(ServerListRequestsMutex);

	ServerList* serverQuery = nullptr;
	for (auto request : serverListRequests)
	{
		if (request.first == hHandle)
		{
			serverQuery = request.second;
			break;
		}
	}

	if (serverQuery == nullptr)
	{
		return ERROR_NOT_FOUND;
	}

	// check if the thread didn't start and if the I/O operation didn't execute
	if (!serverQuery->inProgress)
	{
		serverQuery->inProgress = true;
		serverQuery->ovelapped = pOverlapped;
		serverQuery->serv_thread = std::thread(&ServerList::GetServersFromHttp, serverQuery, cbBuffer, pvBuffer);
		serverQuery->serv_thread.detach();
	}

	// if there are no more servers to be read from the received document, return ERROR_NO_MORE_FILES
	if (serverQuery->ServersLeftInDocumentCount == 0)
	{
		// reset ServersLeftInDocumentCount to "unitialized state"
		serverQuery->ServersLeftInDocumentCount = -1;

		// set the ERROR_NO_MORE_FILES flag to tell the game XEnumerate is done searching
		if (serverQuery->GetTotalServers() > 0)
		{
			pOverlapped->InternalLow = ERROR_NO_MORE_FILES;
			pOverlapped->InternalHigh = serverQuery->GetTotalServers();
			pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);
		}
	}
	else
	{
		// otherwise tell the game we are still running I/O operations
		pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;
		pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);
	}

	return ERROR_IO_PENDING;
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

		document["pProperties"].PushBack(serv_name_property, document.GetAllocator());

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
		std::thread(&ServerList::AddServer, dwUserIndex, dwServerType, xnkid, xnkey, dwMaxPublicSlots, dwMaxPrivateSlots, dwFilledPublicSlots, dwFilledPrivateSlots, cProperties, pProperties, pOverlapped).detach();
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
		std::thread(&ServerList::RemoveServer, pOverlapped).detach();
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
	// some dumbass engineer at hired gun decided to keep the server counts in temporary heap memory
	// and if the async i/o operation is done and the menu is closed, it'll write in free'd memory most likely
	// so instead to write the data asynchronously, we store the properties in XLIVE memory, then pass it to the game when it needs it
	// and get the properties asynchronously

	if (userSignedOnline(dwUserIndex))
		std::thread(&ServerList::GetServerCounts, pOverlapped).detach();

	// we simply just give the game the results synchronously, if we have any
	// it'll query the data each 5 seconds
	if (ServerList::CountResultsUpdated)
	{
		pProperties[0].value.nData = ServerList::total_count;
		pProperties[1].value.nData = ServerList::total_public;
		pProperties[2].value.nData = ServerList::total_peer_gold;
		pProperties[3].value.nData = ServerList::total_peer;
	}

	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->InternalHigh = 0;
	pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_SUCCESS);

	return S_OK;
}

// 5234: ??
// TODO: implement filters
DWORD WINAPI XLocatorCreateServerEnumerator(int a1, DWORD cItems, DWORD cRequiredPropertyIDs, DWORD* pRequiredPropertyIDs, int a5, int a6, int a7, int a8, DWORD* pcbBuffer, PHANDLE phEnum)
{
	LOG_TRACE_XLIVE("XLocatorCreateServerEnumerator()");

	std::lock_guard<std::mutex> lg(ServerListRequestsMutex);

	ServerList* serverListRequest = new ServerList;

	*pcbBuffer = ComputeXLocatorServerEnumeratorBufferSize(cItems, cRequiredPropertyIDs, pRequiredPropertyIDs, nullptr);

	serverListRequest->cSearchPropertiesIDs = cRequiredPropertyIDs;
	serverListRequest->pSearchPropertyIDs = new DWORD[cRequiredPropertyIDs];
	memcpy(serverListRequest->pSearchPropertyIDs, pRequiredPropertyIDs, cRequiredPropertyIDs * sizeof(*pRequiredPropertyIDs));

	if (phEnum)
	{
		*phEnum = serverListRequest->Handle = CreateMutex(NULL, NULL, NULL);

		LOG_TRACE_XLIVE("- Handle = {:p}", (void*)*phEnum);

		serverListRequests.insert(std::make_pair(serverListRequest->Handle, serverListRequest));

		return ERROR_SUCCESS;
	}
	else
	{
		delete serverListRequest;
		return ERROR_INVALID_PARAMETER;
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

	if (phLocatorService) *phLocatorService = g_hXLocatorHandle = CreateMutex(NULL, NULL, NULL);

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

	g_hXLocatorHandle = INVALID_HANDLE_VALUE;

	return ERROR_SUCCESS;
}
