#include "stdafx.h"
#include "ServerList.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Accounts\Accounts.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

#include "..\xnet\IpManagement\XnIp.h"

#include "H2MOD\Modules\Utils\Utils.h"

using namespace rapidjson;

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

void BadServer(ULONGLONG xuid, const char* log_catch)
{
	LOG_ERROR_XLIVE("{} - XUID: {} - Log Catch: {}", __FUNCTION__, xuid, log_catch);
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
		LOG_INFO_XLIVE("{} : stringBufferSize: {}, stringBufferSize2: {}, cItems: {}, stringCount: {}", __FUNCTION__, *outStringBufferSize, (X_PROPERTY_UNICODE_BUFFER_SIZE * cItems * stringProperties), cItems, stringProperties);
	}

	return result;
}

void ServerList::QueryServerData(CURL* curl, ULONGLONG xuid, XLOCATOR_SEARCHRESULT* nResult, XUSER_PROPERTY** propertiesBuffer, WCHAR** stringBuffer)
{
	CURLcode res;
	std::string readBuffer;

	int strings = 0;

	XLOCATOR_SEARCHRESULT tSearchResults;
	ZeroMemory(&tSearchResults, sizeof(XLOCATOR_SEARCHRESULT));

	if (curl) {

		std::string server_url = std::string(cartographerURL + "/live/servers/" + std::to_string(xuid));

		curl_easy_setopt(curl, CURLOPT_URL, server_url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);

		rapidjson::Document doc;
		doc.Parse(readBuffer.c_str());

		if (!doc.HasMember("dwMaxPublicSlots")) {
			BadServer(xuid, "Missing Member: dwMaxPublicSlots");
			return;
		}
		tSearchResults.dwMaxPublicSlots = doc["dwMaxPublicSlots"].GetUint();

		if (!doc.HasMember("dwFilledPublicSlots"))
		{
			BadServer(xuid, "Missing Member: dwFilledPublicSlots");
			return;
		}
		tSearchResults.dwFilledPublicSlots = doc["dwFilledPublicSlots"].GetUint();

		if (!doc.HasMember("dwMaxPrivateSlots"))
		{
			BadServer(xuid, "Missing Member: dwFilledPublicSlots");
			return;
		}
		tSearchResults.dwMaxPrivateSlots = doc["dwMaxPrivateSlots"].GetUint();

		if (!doc.HasMember("dwMaxFilledPrivateSlots"))
		{
			BadServer(xuid, "Missing Member: dwMaxFilledPrivateSlots");
			return;
		}
		tSearchResults.dwFilledPrivateSlots = doc["dwMaxFilledPrivateSlots"].GetUint();

		if (!doc.HasMember("dwServerType"))
		{
			BadServer(xuid, "Missing Member: dwServerType");
			return;
		}
		tSearchResults.dwServerType = doc["dwServerType"].GetUint();

#pragma region Xbox Network Address Reading
		if (!doc.HasMember("lanaddr") || !doc["lanaddr"].IsUint())
		{
			BadServer(xuid, "Missing Member: lanaddr");
			return;
		}
		tSearchResults.serverAddress.ina.s_addr = doc["lanaddr"].GetUint();

		if (!doc.HasMember("xnaddr") || !doc["xnaddr"].IsUint())
		{
			BadServer(xuid, "Missing Member: xnaddr");
			return;
		}
		tSearchResults.serverAddress.inaOnline.s_addr = htonl(doc["xnaddr"].GetUint());

		if (!doc.HasMember("dwPort"))
		{
			BadServer(xuid, "Missing Member: dwPort");
			return;
		}
		tSearchResults.serverAddress.wPortOnline = htons(doc["dwPort"].GetUint());

		if (!doc.HasMember("abenet"))
		{
			BadServer(xuid, "Missing Member: abEnet");
			return;
		}
		const char* abEnet_str = doc["abenet"].GetString();
		if (abEnet_str == NULL)
		{
			BadServer(xuid, "abEnet == NULL");
			return;
		}
		HexStrToBytes(abEnet_str, tSearchResults.serverAddress.abEnet, sizeof(XNADDR::abEnet));

		if (!doc.HasMember("abonline"))
		{
			BadServer(xuid, "Missing Member: abOnline");
			return;
		}
		const char* abOnline_str = doc["abonline"].GetString();
		if (abOnline_str == NULL)
		{
			BadServer(xuid, "abOnline == NULL");
			return;
		}
		HexStrToBytes(abOnline_str, tSearchResults.serverAddress.abOnline, sizeof(XNADDR::abOnline));
#pragma endregion
		
#pragma region Xbox Transport Security Keys Reading
		if (!doc.HasMember("xnkid"))
		{
			BadServer(xuid, "Missing Member: xnkid");
			return;
		}
		const char* xnkid_str = doc["xnkid"].GetString();
		if (xnkid_str == NULL)
		{
			BadServer(xuid, "xnkid == NULL");
			return;
		}
		HexStrToBytes(xnkid_str, tSearchResults.xnkid.ab, sizeof(XNKID));

		if (!doc.HasMember("xnkey"))
		{
			BadServer(xuid, "Missing Member: xnkey");
			return;
		}
		const char* xnkey_str = doc["xnkey"].GetString();
		if (xnkey_str == NULL)
		{
			BadServer(xuid, "xnkey == NULL");
			return;
		}
		HexStrToBytes(xnkey_str, tSearchResults.xnkey.ab, sizeof(XNKEY));
#pragma endregion

		if (!doc.HasMember("xuid"))
		{
			BadServer(xuid, "Missing Member: xuid");
			return;
		}
		tSearchResults.serverID = doc["xuid"].GetUint64();

		if (!doc.HasMember("pProperties") || !doc.HasMember("cProperties"))
		{
			BadServer(xuid, "Missing Member: cProperties or pProperties");
			return;
		}

		tSearchResults.cProperties = 0;
		tSearchResults.pProperties = *propertiesBuffer;

		std::vector<DWORD> propertiesWritten;

		for (auto& property : doc["pProperties"].GetArray())
		{
			bool propertyNeeded = false;
			DWORD propertyId = property["dwPropertyId"].GetInt();

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
				//LOG_INFO_XLIVE("{} - unrequested property ID: 0x{:X}, skipping", __FUNCTION__, propertyId);
				continue;
			}

			// temporary memory
			XUSER_PROPERTY tProperty;
			ZeroMemory(&tProperty, sizeof(XUSER_PROPERTY));

			tProperty.dwPropertyId = propertyId;
			tProperty.value.type = property["type"].GetInt();

			propertiesWritten.push_back(propertyId);

			const char* data = 0;
			int str_len = 0;

			std::wstring str;
			GenericStringBuffer<UTF16<> > buffer;
			Writer<GenericStringBuffer<UTF16<> >, UTF8<>, UTF16<> > writer(buffer);

			switch (property["type"].GetInt())
			{
			case XUSER_DATA_TYPE_INT32:
				tProperty.value.nData = property["value"].GetInt();
				break;

			case XUSER_DATA_TYPE_INT64:
				tProperty.value.i64Data = property["value"].GetInt64();
				break;

			case XUSER_DATA_TYPE_UNICODE:
				writer.String(property["value"].GetString());

				str.append(buffer.GetString());
				str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
				str.erase(std::remove(str.begin(), str.end(), '\\'), str.end());

				if (!cancelOperation)
				{
					SecureZeroMemory(*stringBuffer, X_PROPERTY_UNICODE_BUFFER_SIZE);

					wcscpy(*stringBuffer, str.c_str());

					tProperty.value.string.cbData = wcsnlen(*stringBuffer, 64) * sizeof(WCHAR) + 2;
					tProperty.value.string.pwszData = *stringBuffer;

					*stringBuffer = (WCHAR*)((BYTE*)(*stringBuffer) + X_PROPERTY_UNICODE_BUFFER_SIZE);
				}
				
				strings++;

				break;

			case XUSER_DATA_TYPE_BINARY:
			case XUSER_DATA_TYPE_FLOAT:
			case XUSER_DATA_TYPE_DOUBLE:
			case XUSER_DATA_TYPE_DATETIME:
				//LOG_TRACE_XLIVE("{} - unimplemented data type");
				break;
			}

			if (!cancelOperation)
			{
				tSearchResults.pProperties[tSearchResults.cProperties] = tProperty;
				tSearchResults.cProperties++;
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
	}

	if (!cancelOperation)
	{
		*nResult = tSearchResults;
	}

	//LOG_ERROR_XLIVE("{} : string count: {}, properties: {}", __FUNCTION__, strings, nResult->cProperties);

	++total_servers;
}

void ServerList::GetServersFromHttp(DWORD cbBuffer, CHAR* pvBuffer)
{
	auto cleanup = [this]()
	{

		// FIXME: fix logic
		// for now keep the way we handled serverlist request before
		ServerListRequestsMutex.lock();
		this->operationState = OperationFinished;

		//serverListRequests.erase(this->Handle);
		//XCloseHandle(this->Handle);

		//delete[] this->pSearchPropertyIDs;

		//delete this;

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

	ovelapped->InternalLow = ERROR_IO_INCOMPLETE;

	addDebugText("Requesting server list");

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	this->total_servers = 0;

	curl = curl_interface_init_no_ssl();
	if (curl) {
		std::string url(cartographerURL + "/live/server_list.php");

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
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
			if (cancelOperation)
				break;

			ZeroMemory(&searchResults[this->GetTotalServers()], sizeof(XLOCATOR_SEARCHRESULT));
			QueryServerData(curl, std::stoll(server.GetString()), &searchResults[this->GetTotalServers()], &propertiesBuffer, &stringBuffer);

			if (this->GetTotalServers() > 0)
			{
				ovelapped->InternalLow = ERROR_SUCCESS;
				ovelapped->InternalHigh = this->GetTotalServers();
				ovelapped->dwExtendedError = 0;
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

		LOG_TRACE_XLIVE("{} - found a total of: {} servers", __FUNCTION__, this->GetTotalServers());

		addDebugText(L"Found %d servers", this->GetTotalServers());

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

	curl = curl_interface_init_no_ssl();
	if (curl) {
		std::string url(cartographerURL + "/live/dedicount.php");

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
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

	switch (serverQuery->operationState)
	{
	case OperationPending:
		serverQuery->operationState = OperationIncomplete;
		serverQuery->ovelapped = pOverlapped;
		serverQuery->serv_thread = std::thread(&ServerList::GetServersFromHttp, serverQuery, cbBuffer, pvBuffer);
		serverQuery->serv_thread.detach();
		break;

	case OperationIncomplete:
		// otherwise tell the game we are still running I/O operations
		pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;
		pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);
		break;

	case OperationFinished:
		// check if we didn't find any servers
		if (serverQuery->GetTotalServers() > 0)
		{
			// if we didn't find any, let the game know
			serverQuery->ovelapped->InternalLow = ERROR_NO_MORE_FILES;
			serverQuery->ovelapped->InternalHigh = 0;
			serverQuery->ovelapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);
		}

		serverListRequests.erase(serverQuery->Handle);
		XCloseHandle(serverQuery->Handle);

		delete[] serverQuery->pSearchPropertyIDs;

		delete serverQuery;
		break;

	default:
		return ERROR_NOT_FOUND;
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
	pOverlapped->InternalHigh = 0;
	pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);

	curl = curl_interface_init_no_ssl();
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

		std::string url(cartographerURL + "/live/del_server.php");

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer.GetString());
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->InternalHigh = 0;
	pOverlapped->dwExtendedError = 0;
}

void ServerList::AddServer(DWORD dwUserIndex, DWORD dwServerType, XNKID xnkid, XNKEY xnkey, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, DWORD dwFilledPublicSlots, DWORD dwFilledPrivateSlots, DWORD cProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped)
{
	std::lock_guard<std::mutex> lk(AddServerMutex);

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;
	pOverlapped->InternalHigh = 0; // this shouldn't even be checked by game's code, but for some reason it gets in Halo 2, InternalHIgh is used for enumerating data, where it holds how many elemets were retreived
	pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);

	curl = curl_interface_init_no_ssl();
	if (curl) {
		rapidjson::Document document;
		document.SetObject();

		Value token(kStringType);
		if (H2CurrentAccountLoginToken)
			token.SetString(H2CurrentAccountLoginToken, document.GetAllocator());

		Value xnkid_val(kStringType);
		xnkid_val.SetString(ByteToHexStr(xnkid.ab, sizeof(xnkid.ab)).c_str(), document.GetAllocator());

		Value xnkey_val(kStringType);
		xnkey_val.SetString(ByteToHexStr(xnkey.ab, sizeof(xnkey.ab)).c_str(), document.GetAllocator());

		XnIp* localUser = gXnIp.GetLocalUserXn();

		document.AddMember("token", token, document.GetAllocator());
		document.AddMember("xuid", Value().SetUint64(usersSignInInfo[dwUserIndex].xuid), document.GetAllocator());
		document.AddMember("dwServerType", Value().SetInt(dwServerType), document.GetAllocator());
		document.AddMember("dwMaxPublicSlots", Value().SetInt(dwMaxPublicSlots), document.GetAllocator());
		document.AddMember("dwFilledPublicSlots", Value().SetInt(dwFilledPublicSlots), document.GetAllocator());
		document.AddMember("dwMaxPrivateSlots", Value().SetInt(dwMaxPrivateSlots), document.GetAllocator());
		document.AddMember("dwMaxFilledPrivateSlots", Value().SetInt(dwFilledPrivateSlots), document.GetAllocator());
		document.AddMember("dwPort", Value().SetInt(H2Config_base_port), document.GetAllocator());
		if (localUser)
		{
			document.AddMember("lanaddr", Value().SetUint(localUser->xnaddr.ina.s_addr), document.GetAllocator());
		}
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

		std::string url(cartographerURL + "/live/add_server.php");

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer.GetString());
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->InternalHigh = 0;
	pOverlapped->dwExtendedError = 0;
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
	pOverlapped->dwExtendedError = 0;

	return S_OK;
}

// TODO: implement filters
DWORD WINAPI XLocatorCreateServerEnumerator(int a1, DWORD cItems, DWORD cRequiredPropertyIDs, DWORD* pRequiredPropertyIDs, int a5, int a6, int a7, int a8, DWORD* pcbBuffer, PHANDLE phEnum)
{
	LOG_TRACE_XLIVE("XLocatorCreateServerEnumerator()");

	std::lock_guard<std::mutex> lg(ServerListRequestsMutex);

	ServerList* serverListRequest = nullptr;

	// FIXME: for now we keep only 1 serverlist request, just update the handle
	if (serverListRequests.empty())
	{
		serverListRequest = new ServerList(cRequiredPropertyIDs, pRequiredPropertyIDs);
	}
	else
	{
		serverListRequest = serverListRequests.begin()->second; // get the first and the only element for now
		serverListRequests.erase(serverListRequest->Handle);
		XCloseHandle(serverListRequest->Handle);
	}

	*pcbBuffer = ComputeXLocatorServerEnumeratorBufferSize(cItems, cRequiredPropertyIDs, pRequiredPropertyIDs, nullptr);
	
	if (phEnum)
	{
		*phEnum = serverListRequest->Handle = CreateMutex(NULL, NULL, NULL);

		LOG_TRACE_XLIVE("- Handle = {:p}", (void*)*phEnum);
		
		serverListRequests.insert(std::make_pair(serverListRequest->Handle, serverListRequest));

		return ERROR_SUCCESS;
	}
	else
	{
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
	std::lock_guard<std::mutex> lg(ServerListRequestsMutex);

	LOG_TRACE_XLIVE("XLocatorServiceUnInitialize(a1 = {})", xlocatorhandle);
	CloseHandle(xlocatorhandle);

	for (auto request : serverListRequests)
	{
		request.second->CancelOperation();
	}

	g_hXLocatorHandle = INVALID_HANDLE_VALUE;

	return ERROR_SUCCESS;
}
