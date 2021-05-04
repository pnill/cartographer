#include "CustomVariantSettings.h"
#include "H2MOD/Modules/Networking/Memory/bitstream.h"
#include "H2MOD/Modules/Networking/NetworkSession/NetworkSession.h"
#include "H2MOD.h"
#include "H2MOD/Modules/Networking/CustomPackets/CustomPackets.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
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

	void SendCustomVariantSettings(int peerIndex)
	{
		network_session* session = NetworkSession::getCurrentNetworkSession();
		if (NetworkSession::localPeerIsSessionHost())
		{
			//TODO: Find and map out struct with current variant information.
			auto VariantName = std::wstring(Memory::GetAddress<wchar_t*>(0, 0x534A18));
			if (CustomVariantSettingsMap.count(VariantName) > 0)
			{
				CurrentVariantSettings = CustomVariantSettingsMap.at(VariantName);
				network_observer* observer = session->network_observer_ptr;
				peer_observer_channel* observer_channel = NetworkSession::getPeerObserverChannel(peerIndex);
				if (peerIndex != -1 && peerIndex != session->local_peer_index && observer_channel->field_1)
				{
					observer->sendNetworkMessage(session->session_index, observer_channel->observer_index,
						network_observer::e_network_message_send_protocol::in_band, custom_variant_settings,
						CustomVariantSettingsPacketSize, &CustomVariantSettingsMap.at(VariantName));
				}
			}
		}
	}
	void ResetSettings()
	{
		CurrentVariantSettings = s_variantSettings();
	}

	void OnGamestateChange(game_life_cycle state)
	{
		if (state == life_cycle_in_game) {
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
		}
	}
	void OnNetworkPlayerEvent(int peerIndex, EventHandler::NetworkPlayerEventType type)
	{
		if(type == EventHandler::NetworkPlayerEventType::add)
		{
			SendCustomVariantSettings(peerIndex);
		}
	}
	void OnMatchCountdown()
	{
		//
		//Anything host related before the game starts goes here.
		//
		for(auto i = 0; i < NetworkSession::getPeerCount(); i++)
			SendCustomVariantSettings(i);
	}

	typedef int(__cdecl c_get_next_hill_index)(int previousHill);
	c_get_next_hill_index* p_get_next_hill_index;
	signed int __cdecl get_next_hill_index(int previousHill)
	{
		int hillCount = *Memory::GetAddress<int*>(0x4dd0a8, 0x5008e8);

		//Return -1 to tell the engine there is no koth hills on the map.
		if (hillCount <= 0)
			return -1;
		switch(CurrentVariantSettings.HillRotation)
		{
			case sequential:
				if (previousHill + 1 >= hillCount)
					return 0;
				return previousHill + 1;
			case reverse:
				if (previousHill - 1 <= 0)
					return hillCount;
				return previousHill - 1;
			default:
			case random:
				return p_get_next_hill_index(previousHill);
		}
		//Just in case.
		return -1;
	}

	void ApplyHooks()
	{
		//p_get_next_hill_index = (getnexthillindex)DetourFunc(Memory::GetAddress<BYTE*>(0x10DF1E, 0xDA4CE), (BYTE*)get_next_hill_index, 9);
		p_get_next_hill_index = Memory::GetAddress<c_get_next_hill_index*>(0x10DF1E, 0xDA4CE);
		PatchCall(Memory::GetAddress(0x10FE1F, 0xDC3CF), get_next_hill_index);
		PatchCall(Memory::GetAddress(0x10FE55, 0xDC405), get_next_hill_index);
	}

	void Initialize()
	{
		ApplyHooks();
		
		//EventHandler::registerGameStateCallback({ "VariantSettings", life_cycle_in_game, OnIngame }, false);
		//EventHandler::registerCountdownStartCallback(OnMatchCountdown, "VariantGameStart", true);
		//EventHandler::register_callback(OnMatchCountdown, countdown_start, before, false, true);
		/*EventHandler::registerNetworkPlayerAddCallback(
			{
				"VariantSettings",
				[](int peer)
				{
					SendCustomVariantSettings();
				}
			},
			true);*/

		EventHandler::register_callback<EventHandler::GameStateEvent>(OnGamestateChange);
		EventHandler::register_callback<EventHandler::CountdownStartEvent>(OnMatchCountdown, execute_after, true);
		EventHandler::register_callback<EventHandler::NetworkPlayerEvent>(OnNetworkPlayerEvent, execute_after, true);

	}
}