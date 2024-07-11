#pragma once
#include "main/game_preferences.h"

/* enums */

enum e_weapon_offsets_string
{
	_weapon_offsets_string_title,
	_weapon_offsets_string_combo_title,

	_weapon_offsets_string_battle_rifle_title,
	_weapon_offsets_string_beam_rifle_title,
	_weapon_offsets_string_brute_plasma_rifle_title,
	_weapon_offsets_string_brute_shot_title,
	_weapon_offsets_string_carbine_title,
	_weapon_offsets_string_energy_sword_title,
	_weapon_offsets_string_fuel_rod_title,
	_weapon_offsets_string_magnum_title,
	_weapon_offsets_string_needler_title,
	_weapon_offsets_string_plasma_pistol_title,
	_weapon_offsets_string_plasma_rifle_title,
	_weapon_offsets_string_rocket_launcher_title,
	_weapon_offsets_string_sentinel_beam_title,
	_weapon_offsets_string_shotgun_title,
	_weapon_offsets_string_smg_title,
	_weapon_offsets_string_sniper_title,

	_weapon_offsets_string_weapon_offset_x,
	_weapon_offsets_string_weapon_offset_y,
	_weapon_offsets_string_weapon_offset_z,
	_weapon_offsets_string_reset,

	k_weapon_offsets_string_count,

	k_weapon_offsets_weapon_title_first = _weapon_offsets_string_battle_rifle_title,
	k_weapon_offsets_weapon_title_last = _weapon_offsets_string_sniper_title,
	k_weapon_offsets_weapon_title_count = k_weapon_offsets_weapon_title_last - k_weapon_offsets_weapon_title_first + 1
};

/* globals */

const char* g_weapon_offsets_string_table_english[k_weapon_offsets_string_count]
{
	"        Weapon Offsets",
	"Weapon",
	"Battle Rifle",
	"Beam Rifle",
	"Brute Plasma Rifle",
	"Brute Shot",
	"Carbine",
	"Energy Sword",
	"Fuel Rod",
	"Magnum",
	"Needler",
	"Plasma Pistol",
	"Plasma Rifle",
	"Rocket Launcher",
	"Sentinel Beam",
	"Shotgun",
	"SMG",
	"Sniper",
	"Weapon Offset X",
	"Weapon Offset Y",
	"Weapon Offset Z",
	"Reset"
};

const char* g_weapon_offsets_string_table_spanish[k_weapon_offsets_string_count]
{
	u8"        Compensaciones de armas",
	u8"Arma",
	u8"Fusil de Batalla",
	u8"Fusil de Haz",
	u8"Rifle de Plasma Bruto",
	u8"Disparo Bruto",
	u8"Carabina",
	u8"Espada de Energía",
	u8"Varilla de Combustible",
	u8"Botella doble",
	u8"Aguijoneador",
	u8"Pistola de Plasma",
	u8"Fusil de Plasma",
	u8"Lanzacohetes",
	u8"Rayo Centinela",
	u8"Escopeta",
	u8"Subfusil",
	u8"Francotirador",
	u8"Compensación de Armas X",
	u8"Compensación de Armas Y",
	u8"Compensación de Armas Z",
	u8"Reiniciar"
};

const char** g_weapon_offsets_string_table[k_language_count] =
{
	g_weapon_offsets_string_table_english,
	NULL,
	NULL,
	NULL,
	g_weapon_offsets_string_table_spanish,
	NULL,
	NULL,
	NULL,
	NULL
};
