#include "CustomVariantSettings.h"
#include "H2MOD/Modules/Networking/Memory/bitstream.h"
#include "H2MOD/Modules/Networking/NetworkSession/NetworkSession.h"
#include "H2MOD.h"
#include "H2MOD/Modules/Networking/CustomPackets/CustomPackets.h"
#include "H2MOD/Modules/EventHandler/EventHandler.h"
#include "H2MOD/Modules/Utils/Utils.h"
#include "Util/Hooks/Hook.h"
#include "Blam/Engine/Game/PhysicsConstants.h"
#include "H2MOD/Modules/Stats/StatsHandler.h"

struct s_weapon_group_definition;
std::map<std::wstring, CustomVariantSettings::s_variantSettings> CustomVariantSettingsMap;
CustomVariantSettings::s_variantSettings CurrentVariantSettings;
namespace CustomVariantSettings
{
	float* defaultGravity;
	void __cdecl EncodeVariantSettings(bitstream* stream, int a2, s_variantSettings* data)
	{
		stream->data_encode_bits("gravity", &data->Gravity, sizeof(data->Gravity) * CHAR_BIT); //16
		stream->data_encode_bool("Infinite Ammo", data->InfiniteAmmo);
		stream->data_encode_bool("Explosion Physics", data->ExplosionPhysics);
		stream->data_encode_integer("Hill Rotation", (byte)data->HillRotation, 8);
	}
	bool __cdecl DecodeVariantSettings(bitstream* stream, int a2, s_variantSettings* data)
	{
		double gravity;
		stream->data_decode_bits("gravity", &gravity, sizeof(gravity) * CHAR_BIT);
		data->Gravity = gravity;
		data->InfiniteAmmo = stream->data_decode_bool("Infinite Ammo");
		data->ExplosionPhysics = stream->data_decode_bool("Explosion Physics");
		data->HillRotation = (e_hill_rotation)stream->data_decode_integer("Hill Rotation", 8);
		return stream->packet_is_valid() == false;
	}

	void RecieveCustomVariantSettings(s_variantSettings* data)
	{
		CurrentVariantSettings.Gravity = data->Gravity;
		CurrentVariantSettings.InfiniteAmmo = data->InfiniteAmmo;
		CurrentVariantSettings.ExplosionPhysics = data->ExplosionPhysics;
		CurrentVariantSettings.HillRotation = data->HillRotation;
	}

	void SendCustomVariantSettings()
	{
		network_session* session = NetworkSession::getCurrentNetworkSession();
		if (NetworkSession::localPeerIsSessionHost())
		{
			//TODO: Find and mapout struct with current variant information.
			auto VariantName = std::wstring(Memory::GetAddress<wchar_t*>(0, 0x534A18));
			if (CustomVariantSettingsMap.count(VariantName) > 0)
			{
				CurrentVariantSettings = CustomVariantSettingsMap.at(VariantName);
				network_observer* observer = session->network_observer_ptr;
				for (auto i = 0; i < NetworkSession::getPeerCount(); i++) {
					peer_observer_channel* observer_channel = NetworkSession::getPeerObserverChannel(i);
					if (i != -1 && i != session->local_peer_index && observer_channel->field_1)
					{
						observer->sendNetworkMessage(session->session_index, observer_channel->observer_index,
				            network_observer::e_network_message_send_protocol::in_band, custom_variant_settings,
                            CustomVariantSettingsPacketSize, &CustomVariantSettingsMap.at(VariantName));
					}
				}
			}
		}
	}
	void ResetSettings()
	{
		CurrentVariantSettings = s_variantSettings();
	}

	void OnIngame()
	{
		//
		//Anything to be done on host and client goes here.
		//
		physics_constants::get()->gravity = CurrentVariantSettings.Gravity;

		//mov [ecx+6], ax
		static BYTE InfiniteAmmoMagazineASM[] = { 0x66, 0x89, 0x41, 0x06 };
		//movss [edi+00000184],xmm0
		static BYTE InfiniteAmmoBatteryASM[] = { 0xF3, 0x0F, 0x11, 0x87, 0x84, 0x01, 0x00, 0x00 };
		if (CurrentVariantSettings.InfiniteAmmo)
		{
			//Nop remove ammo from clips
			NopFill(Memory::GetAddress(0x15F3EA, 0x1436AA), 4);
			//Nop remove energy from battery.
			NopFill(Memory::GetAddress(0x15f7c6, 0x143A86), 8);
		}
		else
		{
			WriteBytes(Memory::GetAddress(0x15F3EA, 0x1436AA), InfiniteAmmoMagazineASM, 4);
			WriteBytes(Memory::GetAddress(0x15f7c6, 0x143A86), InfiniteAmmoBatteryASM, 8);
		}

		if (!Memory::isDedicatedServer()) {
			if (CurrentVariantSettings.ExplosionPhysics)
				WriteValue(Memory::GetAddress(0x17a44b), (BYTE)0x1e);
			else
				WriteValue(Memory::GetAddress(0x17a44b), (BYTE)0);
		}
		EventHandler::registerGameStateCallback({ "ResetSettingsPre", life_cycle_pre_game, ResetSettings, true }, false);
	}

	void OnMatchCountdown()
	{
		//
		//Anything host related before the game starts goes here.
		//
		SendCustomVariantSettings();
	}

	typedef int(__cdecl* getnexthillindex)(int previousHill);
	getnexthillindex p_get_next_hill_index;
	signed int __cdecl get_next_hill_index(int previousHill)
	{
		int hillCount = *Memory::GetAddress<int*>(0x4dd0a8, 0x5008e8);
		//The game stores the hills in an array that correlates them to which Netgame Flag option
		auto hillMap = Memory::GetAddress<__int16*>(0x4DD0B0, 0x5008F0);

		//Return -1 to tell the engine there is no koth hills on the map.
		if (hillCount <= 0)
			return -1;
		switch(CurrentVariantSettings.HillRotation)
		{
			case sequential:
				if (previousHill + 1 >= hillCount)
					return hillMap[0];
				return hillMap[previousHill + 1];
				break;
			case reverse:
				if (previousHill - 1 <= 0)
					return hillMap[hillCount];
				return hillMap[previousHill - 1];
				break;
			default:
			case random:
				//This is a cleaned up version of the default behavior of the engine
				auto random_seed = (unsigned int *)Engine::get_global_random_seed_address();
				*random_seed = 1664525 * *random_seed + 1013904223;
				auto random16 = ((hillCount * (*random_seed >> 16)) >> 16);
				auto currentIndex = 0;
				auto randomIndex = 0;
				while(true)
				{
					randomIndex = (__int16)((random16 + currentIndex) % hillCount);
					if (previousHill != hillMap[randomIndex])
						break;
					if (++currentIndex >= hillCount)
						return -1;
				}
				return hillMap[randomIndex];
				break;
		}
		//Just in case.
		return -1;
	}

	void ApplyHooks()
	{
		p_get_next_hill_index = (getnexthillindex)DetourFunc(Memory::GetAddress<BYTE*>(0x10DF1E, 0xDA4CE), (BYTE*)get_next_hill_index, 9);
	}

	void Initialize()
	{
		ApplyHooks();
		EventHandler::registerGameStateCallback({ "VariantSettings", life_cycle_in_game, OnIngame }, false);

		EventHandler::registerCountdownStartCallback(OnMatchCountdown, "VariantGameStart", true);
	/*	EventHandler::registerGameStateCallback({ "VariantSettings1", life_cycle_start_game, 
			[]()
			{
				for(auto i = 0; i < NetworkSession::getPeerCount(); i++)
					SendCustomVariantSettings(i);
			} 
			},	true);*/
		EventHandler::registerNetworkPlayerAddCallback(
			{
				"VariantSettings",
				[](int peer)
				{
					SendCustomVariantSettings();
				}
			},
			true);
	}
}