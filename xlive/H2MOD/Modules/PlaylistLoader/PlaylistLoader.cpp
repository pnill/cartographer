#include "PlaylistLoader.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD.h"
#include "H2MOD/Modules/Startup/Startup.h"
#include "H2MOD/Modules/Utils/Utils.h"
#include "H2MOD/Modules/CustomVariantSettings/CustomVariantSettings.h"

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
	};
	std::map<std::wstring, e_custom_setting> custom_settings;
	e_custom_setting get_custom_setting_index(wchar_t* Name)
	{
		for (const auto &custom_setting : custom_settings)
			if (_wcsicmp(custom_setting.first.c_str(), Name) == 0)
				return custom_setting.second;
		return none;
	}
	void playlist_invalid_item_hook(playlist_entry* playlist_entry, int a2, int a3, int a4, wchar_t *a5, wchar_t *a6, wchar_t *a7)
	{
		__asm
		{
			push a7
			push a6
			push a5
			push a4
			push a3
			mov ebx, a2
			mov ecx, playlist_entry
			mov eax, [0xED2E]
			add eax, [H2BaseAddr]
			call eax
			add esp, 20
		}
	}

	//void test_hook(int a3, int a4, wchar_t* a5, wchar_t* a6, wchar_t* a7)
	//{

	//	_asm
	//	{
	//		push esi
	//	}
	//	DWORD* a1;
	//	int a2;
	//	_asm
	//	{
	//		mov a1, ecx
	//		mov a2, ebx
	//	}
	//	playlist_invalid_item_hook((playlist_entry*)a1, a2, a3, a4, a5, a6, a7);
	//	_asm
	//	{
	//		pop esi
	//		retn 14h
	//	}
	//}

	bool custom_setting_boolean_check(playlist_entry* playlist_entry, wchar_t* value)
	{
		if (_wcsicmp(value, L"on") == 0 || _wcsicmp(value, L"true") == 0)
			return true;
		if (_wcsicmp(value, L"off") == 0 || _wcsicmp(value, L"false") == 0)
			return false;
		playlist_invalid_item_hook(
			playlist_entry,
			0,
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

		playlist_invalid_item_hook(
			playlist_entry,
			0,
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
		for(auto i = 0; i < values_size; i++)
			if (_wcsicmp(value, values[i]) == 0) 
				return static_cast<T>(i);

		playlist_invalid_item_hook(
			playlist_entry,
			0,
			4,
			playlist_entry->reader_current_line,
			&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index],
			&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index + 32],
			&empty_char);

		return static_cast<T>(-1);
	}

	bool process_custom_setting(playlist_entry* playlist_entry)
	{
		//Section Type must be Variant
		if (playlist_entry->current_section_type != 1)
			return false;

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
		if (custom_setting_type != -1) 
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

				LOG_INFO_GAME(L"[PlaylistLoader::ProcessCustomSetting] Variant: {} Custom Setting Detected: {} = {}", variant, property_name, property_value);
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
						settings->HillRotation = custom_settings_enum_check<CustomVariantSettings::e_hill_rotation>(playlist_entry, property_value, CustomVariantSettings::hill_rotation_name, 3);
						break;
					case none:
					default:
						playlist_invalid_item_hook(
							playlist_entry,
							0,
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



	//void  current_item_name_trim_end(playlist_entry *pThis)
	//{
	//	int i; // eax
	//	int v2; // edx
	//	wchar_t v3; // si
	//	wchar_t *v4; // edx

	//	pThis->section_buffer[pThis->reader_current_char_index + 68 * pThis->section_buffer_current_index] = 0;
	//	for (i = pThis->reader_current_char_index - 1; i > 0; *v4 = 0)
	//	{
	//		v2 = i + 68 * pThis->section_buffer_current_index;
	//		v3 = pThis->section_buffer[v2];
	//		v4 = &pThis->section_buffer[v2];
	//		if (v3 != ' ' && v3 != '\t')
	//			break;
	//		--i;
	//	}
	//}

	typedef void(__fastcall* h_playlist_processs_setting)(playlist_entry* playlist_entry);
	h_playlist_processs_setting p_playlist_process_setting;
	void __fastcall process_setting(playlist_entry* playlist_entry)
	{
		if (!process_custom_setting(playlist_entry)) 
		{
			p_playlist_process_setting(playlist_entry);
		}
	}

	//typedef void(__fastcall* h_playlist_read_line)(playlist_entry* playlist_entry, DWORD a1, wchar_t* string_buffer);
	//h_playlist_read_line p_playlist_read_line;

	//void __fastcall read_line(playlist_entry* playlist_entry, DWORD a1, wchar_t* string_buffer)
	//{
	//	//p_playlist_read_line(pThis, playlist_entry, stringBuffer);
	//	while (*string_buffer)
	//	{
	//		switch (playlist_entry->reader_current_mode)
	//		{
	//		case new_line:
	//			switch (*string_buffer)
	//			{
	//			case '\t':
	//			case ' ':
	//				goto Go_to_next_char;
	//			case '\n':
	//				goto Go_to_next_line;
	//			case ';':
	//				playlist_entry->reader_current_mode = seek_to_next_line;
	//				goto Go_to_next_char;
	//			case '[':
	//				playlist_entry->reader_current_mode = header_start;
	//				goto Go_to_next_char;
	//			default:
	//				playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index + 66 + 2] = 0;
	//				playlist_entry->reader_current_mode = property_name_read;
	//				playlist_entry->reader_current_char_index = 0;
	//				break;
	//			}
	//			break;
	//		case header_start:
	//			switch (*string_buffer)
	//			{
	//			case '\t':
	//			case ' ':
	//				goto Go_to_next_char;
	//			case '\n':
	//			case ']':
	//				playlist_entry->reader_current_char_index = 0;
	//				goto process_header;
	//			default:
	//				playlist_entry->reader_current_char_index = 0;
	//				playlist_entry->reader_current_mode = header_read;
	//				break;
	//			}
	//			break;
	//		case header_read:
	//			if (*string_buffer != '\n' && *string_buffer != ']')
	//			{
	//				if (playlist_entry->reader_current_char_index >= 31)
	//				{
	//					playlist_entry->header_buffer[playlist_entry->reader_current_char_index] = 0;
	//					playlist_invalid_item_hook(
	//						playlist_entry,
	//						0,
	//						3,
	//						playlist_entry->reader_current_line,
	//						playlist_entry->header_buffer,
	//						&empty_char,
	//						&empty_char);
	//					playlist_entry->reader_current_mode = seek_to_next_line;
	//				}
	//				else
	//				{
	//					playlist_entry->header_buffer[playlist_entry->reader_current_char_index] = *string_buffer;
	//					++playlist_entry->reader_current_char_index;
	//				}
	//				goto Go_to_next_char;
	//			}
	//		process_header:
	//			process_playlist_header_hook(reinterpret_cast<DWORD*>(playlist_entry), 0);
	//			playlist_entry->reader_current_mode = seek_to_next_line;
	//			break;
	//		case property_name_read:
	//			if (*string_buffer != '\n')
	//			{
	//				if (*string_buffer == '=')
	//				{
	//					current_item_name_trim_end(playlist_entry);
	//					playlist_entry->reader_current_mode = property_deliminator_scan;
	//				}
	//				else
	//				{
	//					if (playlist_entry->reader_current_char_index >= 31)
	//					{
	//						playlist_entry->section_buffer[playlist_entry->reader_current_char_index + 68 * playlist_entry->section_buffer_current_index] = 0;
	//						playlist_invalid_item_hook(
	//							playlist_entry,
	//							0,
	//							2,
	//							playlist_entry->reader_current_line,
	//							&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index],
	//							&empty_char,
	//							&empty_char);
	//						playlist_entry->reader_current_mode = seek_to_next_line;
	//					}
	//					else
	//					{
	//						playlist_entry->section_buffer[playlist_entry->reader_current_char_index + 68 * playlist_entry->section_buffer_current_index] = *string_buffer;
	//						++playlist_entry->reader_current_char_index;
	//					}
	//				}
	//				goto Go_to_next_char;
	//			}
	//			current_item_name_trim_end(playlist_entry);
	//			playlist_entry->reader_current_mode = property_deliminator_scan;
	//			break;
	//		case property_deliminator_scan:
	//			if (*string_buffer == '\t' || *string_buffer == ' ')
	//				goto Go_to_next_char;
	//			playlist_entry->reader_current_char_index = 0;
	//			playlist_entry->reader_current_mode = property_value_read;
	//			break;
	//		case property_value_read:
	//			if (*string_buffer == '\n')
	//			{
	//				if (!process_custom_setting(playlist_entry)) {
	//					process_playlist_setting_hook(reinterpret_cast<DWORD*>(playlist_entry));
	//				}
	//				goto go_to_next_item;
	//			}
	//			if (playlist_entry->reader_current_char_index >= 31)
	//			{
	//				playlist_entry->section_buffer[playlist_entry->reader_current_char_index + 68 * playlist_entry->section_buffer_current_index + 32] = 0;
	//				playlist_invalid_item_hook(
	//					playlist_entry,
	//					0,
	//					4,
	//					playlist_entry->reader_current_line,
	//					&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index],
	//					&playlist_entry->section_buffer[68 * playlist_entry->section_buffer_current_index + 32],
	//					&empty_char);
	//				playlist_entry->reader_current_mode = seek_to_next_line;
	//			}
	//			else
	//			{
	//				playlist_entry->section_buffer[playlist_entry->reader_current_char_index + 68 * playlist_entry->section_buffer_current_index + 32] = *string_buffer;
	//				++playlist_entry->reader_current_char_index;
	//			}
	//			goto Go_to_next_char;
	//		case seek_to_next_line:
	//			if (*string_buffer == '\n')
	//			{
	//			go_to_next_item:
	//				playlist_entry->reader_current_mode = new_line;
	//			Go_to_next_line:
	//				++playlist_entry->reader_current_line;
	//			}
	//		Go_to_next_char:
	//			++string_buffer;
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//}

	void apply_hooks()
	{
		//p_playlist_read_line = (h_playlist_read_line)DetourFunc(Memory::GetAddress<BYTE*>(0, 0x12E4F), (BYTE*)read_line, 12);
		p_playlist_process_setting = (h_playlist_processs_setting)DetourFunc(Memory::GetAddress<BYTE*>(0, 0x10FBE), (BYTE*)process_setting, 5);
		//PatchCall(Memory::GetAddress(0, 0x1288D), test_hook);
	}

	void initialize()
	{
		apply_hooks();
		custom_settings.emplace(L"Gravity", e_custom_setting::gravity);
		custom_settings.emplace(L"Infinite Ammo", e_custom_setting::infinite_ammo);
		custom_settings.emplace(L"Explosion Physics", e_custom_setting::explosion_physics);
		custom_settings.emplace(L"Hill Rotation", e_custom_setting::hill_rotation);
	}
}