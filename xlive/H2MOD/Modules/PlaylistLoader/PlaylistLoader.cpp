#include "PlaylistLoader.h"
#include "H2MOD.h"
#include "H2MOD\Modules\CustomVariantSettings\CustomVariantSettings.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "Util\Hooks\Hook.h"

namespace playlist_loader
{
	static wchar_t empty_char = '\0';
	enum e_custom_setting
	{
		none = -1,
		gravity,
		infinite_ammo,
		explosion_physics,
		hill_rotation,
		game_speed,
		infinite_grenades,
		spawn_protection,
		hill_set,
		forced_fov
	};
	std::map<std::wstring, e_custom_setting> custom_settings
	{
		{L"None", e_custom_setting::none},
		{L"Gravity", e_custom_setting::gravity},
		{L"Infinite Ammo", e_custom_setting::infinite_ammo},
		{L"Explosion Physics", e_custom_setting::explosion_physics},
		{L"Hill Rotation", e_custom_setting::hill_rotation},
		{L"Game Speed", e_custom_setting::game_speed},
		{L"Infinite Grenades", e_custom_setting::infinite_grenades},
		{L"Hill Set", e_custom_setting::hill_set},
		{L"Spawn Protection", e_custom_setting::spawn_protection},
		{L"Forced FOV", e_custom_setting::forced_fov}
	};
	e_custom_setting get_custom_setting_index(wchar_t* Name)
	{
		for (const auto& custom_setting : custom_settings)
			if (_wcsicmp(custom_setting.first.c_str(), Name) == 0)
				return custom_setting.second;
		return none;
	}
	typedef void(__thiscall* playlist_loader_invalid_entry)(playlist_entry* thisx, int a2, int a3, wchar_t* a5, wchar_t* a6, wchar_t* a7);
	playlist_loader_invalid_entry p_playlist_loader_invalid_entry;

	bool custom_setting_boolean_check(playlist_entry* playlist_entry, wchar_t* value)
	{
		if (_wcsicmp(value, L"on") == 0 || _wcsicmp(value, L"true") == 0)
			return true;
		if (_wcsicmp(value, L"off") == 0 || _wcsicmp(value, L"false") == 0)
			return false;
		p_playlist_loader_invalid_entry(
			playlist_entry,
			4,
			playlist_entry->reader_current_line,
			&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index],
			&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index + 32],
			&empty_char);

		return false;
	}
	double custom_settings_real_check(playlist_entry* playlist_entry, wchar_t* value)
	{
		if (isFloat(value))
			return std::stod(value);

		p_playlist_loader_invalid_entry(
			playlist_entry,
			4,
			playlist_entry->reader_current_line,
			&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index],
			&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index + 32],
			&empty_char);

		return NAN;
	}
	template <typename T = void*>
	T custom_settings_enum_check(playlist_entry* playlist_entry, wchar_t* value, const wchar_t** values, int values_size)
	{
		for (auto i = 0; i < values_size; i++)
			if (_wcsicmp(value, values[i]) == 0)
				return static_cast<T>(i);

		p_playlist_loader_invalid_entry(
			playlist_entry,
			4,
			playlist_entry->reader_current_line,
			&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index],
			&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index + 32],
			&empty_char);

		return static_cast<T>(-1);
	}
	template <typename T>
	T custom_settings_integer_check(playlist_entry* playlist_entry, wchar_t* value)
	{
		if (isInteger(value))
			return static_cast<T>(std::stol(value));

		p_playlist_loader_invalid_entry(
			playlist_entry,
			4,
			playlist_entry->reader_current_line,
			&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index],
			&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index + 32],
			&empty_char);

		return static_cast<T>(0);
	}
	bool process_custom_settting_variant(playlist_entry* playlist_entry)
	{

		auto result = false;

		const auto property_name = &playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index];
		const auto property_value = &playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index + 32];
		wchar_t const* temp_name = nullptr;
		wchar_t const* variant = nullptr;

		//Scan the section buffer for the current variant name to associate it with the custom 
		//setting as the property can be anywhere in the section buffer
		for (auto i = 0; i < playlist_entry->section_buffer_current_index; i++)
		{
			temp_name = &playlist_entry->section_buffer[68 * i];

			if (_wcsicmp(temp_name, L"name") == 0)
			{
				variant = &playlist_entry->section_buffer[68 * i + 32];
				break;
			}
		}
		const auto custom_setting_type = get_custom_setting_index(property_name);

		//Check if it's a custom setting
		if (custom_setting_type != e_custom_setting::none)
		{
			//Check to make sure a variant name has been found.
			if (_wcsicmp(variant, L"") != 0)
			{
				//Trim the end of the current property_value, this is done inside the normal process_setting function
				playlist_entry->section_buffer[playlist_entry->reader_current_char_index + 68 * playlist_entry->section_buffer_current_index + 32] = 0;

				//Grab or create the Custom Settings for the current variant.
				CustomVariantSettings::s_variantSettings* settings;
				const auto variant_string = std::wstring(variant);
				if (CustomVariantSettingsMap.count(variant_string) > 0)
					settings = &CustomVariantSettingsMap.at(variant_string);
				else
				{
					CustomVariantSettingsMap[variant_string] = CustomVariantSettings::s_variantSettings();
					settings = &CustomVariantSettingsMap.at(variant_string);
				}
				std::wstring wproperty(property_value);
				size_t pos = 0;
				std::wstring t;
				byte cIndex = 0;

				LOG_TRACE_GAME(L"[PlaylistLoader::ProcessCustomSetting] Variant: {} Custom Setting Detected: {} = {}", variant_string, property_name, property_value);
				switch (custom_setting_type)
				{
				case gravity:
					settings->Gravity = custom_settings_real_check(playlist_entry, property_value);
					break;
				case infinite_ammo:
					settings->InfiniteAmmo = custom_setting_boolean_check(playlist_entry, property_value);
					break;
				case explosion_physics:
					settings->ExplosionPhysics = custom_setting_boolean_check(playlist_entry, property_value);
					break;
				case hill_rotation:
					settings->HillRotation = custom_settings_enum_check<CustomVariantSettings::e_hill_rotation>(playlist_entry, property_value, CustomVariantSettings::hill_rotation_name, 4);
					break;
				case game_speed:
					settings->GameSpeed = custom_settings_real_check(playlist_entry, property_value);
					break;
				case infinite_grenades:
					settings->InfiniteGrenades = custom_setting_boolean_check(playlist_entry, property_value);
					break;
				case forced_fov:
					settings->ForcedFOV = custom_settings_real_check(playlist_entry, property_value);
					break;
				case hill_set:
					pos = 0;
					t = L"";
					cIndex = 0;
					for (auto i = 0; i < 15; i++)
						settings->PredefinedHillSet[i] = 0;
					while ((pos = wproperty.find(L",")) != std::wstring::npos)
					{
						t = wproperty.substr(0, pos);
						wproperty.erase(0, pos + 1);
						if (isInteger(t))
						{
							settings->PredefinedHillSet[cIndex] = static_cast<byte>(std::stoi(t));
							cIndex++;
						}
						else
						{
							p_playlist_loader_invalid_entry(
								playlist_entry,
								4,
								playlist_entry->reader_current_line,
								&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index],
								&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index + 32],
								&empty_char);
						}
					}
					break;
				case spawn_protection:
					settings->SpawnProtection = custom_settings_integer_check<byte>(playlist_entry, property_value);
					break;
				case none:
				default:
					p_playlist_loader_invalid_entry(
						playlist_entry,
						4,
						playlist_entry->reader_current_line,
						&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index],
						&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index + 32],
						&empty_char);
					break;
				}
				result = true;
			}
		}
		return result;
	}
	bool process_custom_setting_eval(playlist_entry* playlist_entry)
	{
		switch (playlist_entry->current_section_type)
		{
		case Playlist:
			return false;
		case Variant:
			return process_custom_settting_variant(playlist_entry);
		case Match:
			return false;
		}
		return false;
	}

	//Kept in case we want to create new headers in the future.
	//void process_playlist_header_hook(DWORD* playlist_entry, int a2)
	//{
	//	__asm
	//	{
	//		mov ecx, playlist_entry
	//		mov ebx, a2
	//		mov eax, [0x12D14]
	//		add eax, [H2BaseAddr]
	//		call eax
	//	}
	//}

	typedef void(__fastcall* h_playlist_processs_setting)(playlist_entry* playlist_entry);
	h_playlist_processs_setting p_playlist_process_setting;
	void __fastcall process_setting(playlist_entry* playlist_entry)
	{
		if (!process_custom_setting_eval(playlist_entry))
		{
			p_playlist_process_setting(playlist_entry);
		}
	}
	void reset_custom_settings(ServerConsole::ServerConsoleCommands command)
	{
		if (command == ServerConsole::play)
			CustomVariantSettingsMap.clear();
	}

	void apply_hooks()
	{
		p_playlist_process_setting = (h_playlist_processs_setting)DetourFunc(Memory::GetAddress<BYTE*>(0, 0x10FBE), (BYTE*)process_setting, 5);
		p_playlist_loader_invalid_entry = Memory::GetAddress<playlist_loader_invalid_entry>(0, 0xED2E);
	}

	void initialize()
	{
		apply_hooks();
		EventHandler::register_callback<server_command>(reset_custom_settings, execute_after);
	}
}