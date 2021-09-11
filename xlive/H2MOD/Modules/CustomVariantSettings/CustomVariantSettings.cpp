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
#include "Blam/Engine/Players/PlayerControls.h"
#include "Blam/Engine/Game/GameTimeGlobals.h"

std::map<std::wstring, CustomVariantSettings::s_variantSettings> CustomVariantSettingsMap;
CustomVariantSettings::s_variantSettings CurrentVariantSettings;
namespace CustomVariantSettings
{
	float* defaultGravity;
	void __cdecl EncodeVariantSettings(bitstream* stream, int a2, s_variantSettings* data)
	{
		stream->data_encode_bits("gravity", &data->Gravity, sizeof(data->Gravity) * CHAR_BIT); //16.
		stream->data_encode_bits("game speed", &data->GameSpeed, sizeof(data->GameSpeed) * CHAR_BIT);
		stream->data_encode_bool("Infinite Ammo", data->InfiniteAmmo);
		stream->data_encode_bool("Explosion Physics", data->ExplosionPhysics);
		stream->data_encode_integer("Hill Rotation", (byte)data->HillRotation, 8);
		stream->data_encode_bool("Infinite Grenades", data->InfiniteGrenades);
	}
	bool __cdecl DecodeVariantSettings(bitstream* stream, int a2, s_variantSettings* data)
	{
		double gravity;
		stream->data_decode_bits("gravity", &gravity, sizeof(gravity) * CHAR_BIT);
		data->Gravity = gravity;
		double gamespeed;
		stream->data_decode_bits("game speed", &gamespeed, sizeof(gamespeed) * CHAR_BIT);
		data->GameSpeed = gamespeed;
		data->InfiniteAmmo = stream->data_decode_bool("Infinite Ammo");
		data->ExplosionPhysics = stream->data_decode_bool("Explosion Physics");
		data->HillRotation = (e_hill_rotation)stream->data_decode_integer("Hill Rotation", 8);
		data->InfiniteGrenades = stream->data_decode_bool("Infinite Grenades");

		return stream->packet_is_valid() == false;
	}

	void RecieveCustomVariantSettings(s_variantSettings* data)
	{
		CurrentVariantSettings.Gravity = data->Gravity;
		CurrentVariantSettings.InfiniteAmmo = data->InfiniteAmmo;
		CurrentVariantSettings.ExplosionPhysics = data->ExplosionPhysics;
		CurrentVariantSettings.HillRotation = data->HillRotation;
		CurrentVariantSettings.GameSpeed = data->GameSpeed;
		CurrentVariantSettings.InfiniteGrenades = data->InfiniteGrenades;
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
				if (CurrentVariantSettings != s_variantSettings()) {
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
	}
	void ResetSettings()
	{
		CurrentVariantSettings = s_variantSettings();
	}

	void OnPlayerSpawn(datum PlayerDatum)
	{
		if (NetworkSession::localPeerIsSessionHost()) {
			if (CurrentVariantSettings.InfiniteGrenades)
			{
				h2mod->set_player_unit_grenades_count(PlayerDatum.ToAbsoluteIndex(), Fragmentation, 99, false);
				h2mod->set_player_unit_grenades_count(PlayerDatum.ToAbsoluteIndex(), Plasma, 99, false);
			}
		}
	}

	void OnPlayerDeath(datum PlayerDatum, datum KillerDatum)
	{
		if (NetworkSession::localPeerIsSessionHost()) {
			if (CurrentVariantSettings.InfiniteGrenades)
			{
				//Prevent Players from dropping 198 grenades on death..
				h2mod->set_player_unit_grenades_count(PlayerDatum.ToAbsoluteIndex(), Fragmentation, 4, false);
				h2mod->set_player_unit_grenades_count(PlayerDatum.ToAbsoluteIndex(), Plasma, 4, false);
			}
		}
	}

	void ApplyCustomSettings()
	{
		//
		//Anything to be done on host and client goes here.
		//
		if (CurrentVariantSettings != s_variantSettings()) {
			physics_constants::get()->gravity = CurrentVariantSettings.Gravity * physics_constants::get_default_gravity();
			time_globals::get()->game_speed = CurrentVariantSettings.GameSpeed;
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

			//Client Only
			if (!Memory::isDedicatedServer()) {
				if (CurrentVariantSettings.ExplosionPhysics)
					WriteValue(Memory::GetAddress(0x17a44b), (BYTE)0x1e);
				else
					WriteValue(Memory::GetAddress(0x17a44b), (BYTE)0);
			}

			//Server Only
			if (Memory::isDedicatedServer())
			{

			}

			//Host Only
			if (NetworkSession::localPeerIsSessionHost())
			{
				// *((_DWORD *)v1 + 94) = seconds_to_ticks_imprecise(1);
				//Changing the argument passed to seconds_to_ticks_impercise.
				WriteValue(Memory::GetAddress(0x55d01, 0x5e1f9), (BYTE)CurrentVariantSettings.SpawnProtection);
			}
		}
	}
	void OnGamestateChange(game_life_cycle state)
	{
		if (state == life_cycle_in_game) 
		{
			ApplyCustomSettings();
		}
		else if(state == life_cycle_none || state == life_cycle_pre_game || state == life_cycle_post_game)
		{
			ResetSettings();
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
	int currentPredefinedIndex = 0;
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
			case predefined:
				if (currentPredefinedIndex == 15)
					currentPredefinedIndex = 0;
				else if (CurrentVariantSettings.PredefinedHillSet[currentPredefinedIndex + 1] == 0)
					currentPredefinedIndex = 0;
				else
					++currentPredefinedIndex;
				return CurrentVariantSettings.PredefinedHillSet[currentPredefinedIndex] - 1;
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
		
		EventHandler::register_callback<EventHandler::GameStateEvent>(OnGamestateChange);
		EventHandler::register_callback<EventHandler::CountdownStartEvent>(OnMatchCountdown, execute_after,true);
		EventHandler::register_callback<EventHandler::NetworkPlayerEvent>(OnNetworkPlayerEvent, execute_after, true);
		EventHandler::register_callback<EventHandler::BlueScreenEvent>(ApplyCustomSettings, execute_after);
		EventHandler::register_callback<EventHandler::PlayerSpawnEvent>(OnPlayerSpawn, execute_after);
		EventHandler::register_callback<EventHandler::PlayerDeathEvent>(OnPlayerDeath, execute_before);
	}
}