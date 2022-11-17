#pragma once
#include <string>
#include "Blam/Cache/DataTypes/StringID.h"

enum class e_hs_type : short {
	unparsed,
	special_form,
	function_name,
	passthrough,
	nothing,
	boolean,
	real,
	hs_short,
	hs_long,
	string,
	script,
	string_id,
	unit_seat_mapping,
	trigger_volume,
	cutscene_flag,
	cutscene_camera_point,
	cutscene_title,
	cutscene_recording,
	device_group,
	ai,
	ai_command_list,
	ai_command_script,
	ai_behavior,
	ai_orders,
	starting_profile,
	conversation,
	structure_bsp,
	navpoint,
	point_reference,
	style,
	hud_message,
	object_list,
	sound,
	effect,
	damage,
	looping_sound,
	animation_graph,
	damage_effect,
	object_definition,
	bitmap,
	shader,
	render_model,
	structure_definition,
	lightmap_definition,
	game_difficulty,
	team,
	actor_type,
	hud_corner,
	model_state,
	network_event,
	// objects
	object,
	unit,
	vehicle,
	weapon,
	device,
	scenery,
	object_name,
	unit_name,
	vehicle_name,
	weapon_name,
	device_name,
	scenery_name
};

struct s_hs_type_string
{
	e_hs_type hs_type;
	std::string hs_type_string;
};

static s_hs_type_string hs_type_string[]{
	{ e_hs_type::unparsed,	"unparsed" },
	{ e_hs_type::special_form,	"special form" },
	{ e_hs_type::function_name,	"function name" },
	{ e_hs_type::passthrough,	"passthrough" },
	{ e_hs_type::nothing,	"void" },
	{ e_hs_type::boolean,	"boolean" },
	{ e_hs_type::real,	"real" },
	{ e_hs_type::hs_short,	"short" },
	{ e_hs_type::hs_long,	"long" },
	{ e_hs_type::string,	"string" },
	{ e_hs_type::script,	"script" },
	{ e_hs_type::string_id,	"string_id" },
	{ e_hs_type::unit_seat_mapping,	"unit_seat_mapping" },
	{ e_hs_type::trigger_volume,	"trigger_volume" },
	{ e_hs_type::cutscene_flag,	"cutscene_flag" },
	{ e_hs_type::cutscene_camera_point,	"cutscene_camera_point" },
	{ e_hs_type::cutscene_title,	"cutscene_title" },
	{ e_hs_type::cutscene_recording,	"cutscene_recording" },
	{ e_hs_type::device_group,	"device_group" },
	{ e_hs_type::ai,	"ai" },
	{ e_hs_type::ai_command_list,	"ai_command_list" },
	{ e_hs_type::ai_command_script,	"ai_command_script" },
	{ e_hs_type::ai_behavior,	"ai_behavior" },
	{ e_hs_type::ai_orders,	"ai_orders" },
	{ e_hs_type::starting_profile,	"starting_profile" },
	{ e_hs_type::conversation,	"conversation" },
	{ e_hs_type::structure_bsp,	"structure_bsp" },
	{ e_hs_type::navpoint,	"navpoint" },
	{ e_hs_type::point_reference,	"point reference" },
	{ e_hs_type::style,	"style" },
	{ e_hs_type::hud_message,	"hud_message" },
	{ e_hs_type::object_list,	"object_list" },
	{ e_hs_type::sound,	"sound" },
	{ e_hs_type::effect,	"effect" },
	{ e_hs_type::damage,	"damage" },
	{ e_hs_type::looping_sound,	"looping_sound" },
	{ e_hs_type::animation_graph,	"animation_graph" },
	{ e_hs_type::damage_effect,	"damage_effect" },
	{ e_hs_type::object_definition,	"object_definition" },
	{ e_hs_type::bitmap,	"bitmap" },
	{ e_hs_type::shader,	"shader" },
	{ e_hs_type::render_model,	"render model" },
	{ e_hs_type::structure_definition,	"structure definition" },
	{ e_hs_type::lightmap_definition,	"lightmap definition" },
	{ e_hs_type::game_difficulty,	"game_difficulty" },
	{ e_hs_type::team,	"team" },
	{ e_hs_type::actor_type,	"actor_type" },
	{ e_hs_type::hud_corner,	"hud_corner" },
	{ e_hs_type::model_state,	"model_state" },
	{ e_hs_type::network_event,	"network_event" },
	// objects
	{ e_hs_type::object,	"object" },
	{ e_hs_type::unit,	"unit" },
	{ e_hs_type::vehicle,	"vehicle" },
	{ e_hs_type::weapon,	"weapon" },
	{ e_hs_type::device,	"device" },
	{ e_hs_type::scenery,	"scenery" },
	{ e_hs_type::object_name,	"object_name" },
	{ e_hs_type::unit_name,	"unit_name" },
	{ e_hs_type::vehicle_name,	"vehicle_name" },
	{ e_hs_type::weapon_name,	"weapon_name" },
	{ e_hs_type::device_name,	"device_name" },
	{ e_hs_type::scenery_name,	"scenery_name" },
};

struct HaloScriptGlobal
{
	e_hs_type type;
	__int8 padding[2];
	void* data;
};

typedef void** (__cdecl* hs_func_impl)(int hs_opcode, void* script_engine, char a3);

struct HaloScriptCommand
{
	e_hs_type type;
	short unk;
	hs_func_impl func;
	char* usage;
	short arg_count;
	e_hs_type arg_array[1]; // array size is dynamic, but always at least one
};

namespace HaloScript
{
	void UnitKill(datum unitDatum);
	bool UnitInVehicle(datum unitDatum);
	float UnitGetHealth(datum unitDatum);
	float UnitGetShield(datum unitDatum);

	void PhysicsSetGravity(float Multiplier);
	void PhysicsSetVelocityFrame(float unk1, float unk2, float unk3);
	void RenderLightsEnableCinenaticShadow(bool unk1, datum objectDatum, string_id StringId, float unk2);
	void ObjectDestroy(datum object_datum_index);
	void Initialize();
}
