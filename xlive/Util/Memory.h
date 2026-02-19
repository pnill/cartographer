#pragma once

#include "cseries/cseries.h"

/* constants */

enum
{
	k_base_image_address_halo2 = 0x00400000,
	k_base_image_address_h2server = 0x00400000,
};

// Invokes a function
// ADDR_CLIENT: file offset in halo2.exe
// ADDR_SERVER: file offset in h2server.exe
// TYPE: function
// __VA_ARGS__: arguments for the function we want to invoke
// NOTE:
// if server or client is equal to 0 then we pass the same address for both (ONLY ON RELEASE BUILDS)
// REASON:
// optimizes out a cmov or related instruction when building release whenever we call a function that doesn't have a dedi address specified
#ifdef NDEBUG
#define INVOKE_BY_TYPE(_addr_client, _addr_server, _type, ...)	\
Memory::GetAddress<_type>(_addr_client == 0 ? _addr_server : _addr_client, _addr_server == 0 ? _addr_client : _addr_server)(__VA_ARGS__)
#else
#define INVOKE_BY_TYPE(_addr_client, _addr_server, _type, ...)	\
Memory::GetAddress<_type>(_addr_client, _addr_server)(__VA_ARGS__)
#endif

#define INVOKE(_addr_client, _addr_server, _fn_decl, ...) INVOKE_BY_TYPE(_addr_client, _addr_server, decltype(_fn_decl)*, __VA_ARGS__)
// ### TODO find better name
#define INVOKE_TYPE(_addr_client, _addr_server, _type, ...) INVOKE_BY_TYPE(_addr_client, _addr_server, _type, __VA_ARGS__)

#define INVOKE_VFPTR_FN(_get_table_fn, _index, _type, ...) \
	(this->**_get_table_fn<_type>(_index))(__VA_ARGS__)

#define INVOKE_CLASS_FN(classobj, functionPtr)  ((classobj)->*(functionPtr))

#ifdef LLVM
#define CLASS_HOOK_DECLARE_LABEL(name, member) static auto name = &member
#else
#define CLASS_HOOK_DECLARE_LABEL(name, member)
#endif

#ifdef LLVM
#define CLASS_HOOK_JMP(name, member) __asm jmp (name)
#else
#define CLASS_HOOK_JMP(name, member) __asm jmp (member)
#endif

/* enums */

enum e_h2_type
{
	_h2_type_none = NONE,
	_h2_type_unsupported,
	_h2_type_game,
	_h2_type_server,

	_h2_type_ek_sapien,
	_h2_type_ek_tool,
	_h2_type_ek_guerilla
};

class Memory
{
public:
	static e_h2_type Initialize(void);

	static uintptr_t GetAddress()
	{
		return GetBaseAddress();
	}

	static uintptr_t GetAddress(uintptr_t client)
	{
		ASSERT(client != 0);
		return GetBaseAddress() + client;
	}

	static uintptr_t GetAddress(uintptr_t client, uintptr_t server)
	{
		ASSERT(g_memory_is_dedicated_server || client != 0);
		ASSERT(!g_memory_is_dedicated_server || server != 0);
		
		const uintptr_t address = g_memory_is_dedicated_server ? server : client;
		return GetBaseAddress() + address;
	}
	
	template <typename T = void*>
	static T GetAddress(uintptr_t client)
	{
		return reinterpret_cast<T>((uintptr_t)GetAddress(client));
	}

	template <typename T = void*>
	static T GetAddress(uintptr_t client, uintptr_t server)
	{
		return reinterpret_cast<T>((uintptr_t)GetAddress(client, server));
	}

	static uintptr_t GetAddressRelative(uintptr_t client, uintptr_t server = 0)
	{
		return GetAddress(client - k_base_image_address_halo2, server - k_base_image_address_h2server);
	}

	template <typename T = void*>
	static T GetAddressRelative(uintptr_t client, uintptr_t server = 0)
	{
		return reinterpret_cast<T>((uintptr_t)GetAddress(client - k_base_image_address_halo2, server - k_base_image_address_h2server));
	}

	static void SetBaseAddress(uintptr_t base, bool isDedicatedServer)
	{
		baseAddress = base;
		g_memory_is_dedicated_server = isDedicatedServer;
	}

	// gets base address
	static uintptr_t GetBaseAddress() { return baseAddress; }

	static uintptr_t baseAddress;
	static bool g_memory_is_dedicated_server;
};

