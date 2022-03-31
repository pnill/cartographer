#pragma once

#define BASE_IMAGE_ADDRESS_HALO2 0x00400000
#define BASE_IMAGE_ADDRESS_H2SERVER 0x00400000

enum H2Type
{
	H2Game,
	H2Server,
	Invalid
};

class ProcessInfo
{
public:
	HMODULE base;
	H2Type process_type = H2Type::Invalid;
};

class Memory
{
public:
	static void Initialize();

	static void SetBaseAddress(DWORD base, bool isDedicatedServer)
	{
		baseAddress = base;
		dedicatedServer = isDedicatedServer;
	}
	
	// gets base address
	static DWORD GetAddress()
	{
		return baseAddress;
	}

	static DWORD GetAddress(DWORD client, DWORD server = 0)
	{
		return baseAddress + (dedicatedServer ? server : client);
	}

	template <typename T = void*>
	static T GetAddress(DWORD client, DWORD server = 0)
	{
		return reinterpret_cast<T>(baseAddress + (dedicatedServer ? server : client));
	}

	/*
		Gets the memory address from address relative to executable base image address
		Set your IDB base address to BASE_IMAGE_ADDRESS_HALO2 (0x00400000) and you can copy the file offset directly 
		Should ease the job of getting addresses
	*/
	static DWORD GetAddressRelative(DWORD client, DWORD server = 0)
	{
		return baseAddress + (dedicatedServer ? (server - BASE_IMAGE_ADDRESS_H2SERVER) : (client - BASE_IMAGE_ADDRESS_HALO2));
	}

	template <typename T = void*>
	static T GetAddressRelative(DWORD client, DWORD server = 0)
	{
		return reinterpret_cast<T>(baseAddress + (dedicatedServer ? (server - BASE_IMAGE_ADDRESS_H2SERVER) : (client - BASE_IMAGE_ADDRESS_HALO2)));
	}

	static bool IsDedicatedServer() { return dedicatedServer; }

	static DWORD baseAddress;
	static bool dedicatedServer;
};