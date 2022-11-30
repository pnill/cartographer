#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Engine/math/real_math.h"

/*********************************************************************
* name: weather_system
* group_tag : weat
* header size : 176
* *********************************************************************/
struct s_weather_system_group_definition
{
	struct s_particle_system_block
	{
		tag_reference sprites;//0x0
		float view_box_width;//0x8
		float view_box_height;//0xC
		float view_box_depth;//0x10
		float exclusion_radius;//0x14
		float max_velocity;//0x18
		float min_mass;//0x1C
		float max_mass;//0x20
		float min_size;//0x24
		float max_size;//0x28
		__int32 maximum_number_of_particles;//0x2C
		PAD(0xC);//0x30
		float bitmap_animation_speed;//0x3C
		__int32 block_offset;//0x40
		__int32 block_size;//0x44
		__int32 section_data_size;//0x48
		__int32 resource_data_size;//0x4C
		struct s_resources_block
		{
			enum class e_type : __int8
			{
				tag_block = 0,
				tag_data = 1,
				vertex_buffer = 2,
			};
			e_type type;//0x0
			PAD(0x3);//0x1
			__int16 primary_locator;//0x4
			__int16 secondary_locator;//0x6
			__int32 resource_data_size;//0x8
			__int32 resource_data_offset;//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_resources_block, 0x10);
		tag_block<s_resources_block> resources;//0x50
		PAD(0x4);//0x58
		__int16 owner_tag_section_offset;//0x5C
		PAD(0x6);//0x5E
		struct s_particle_system_data_block
		{
			struct s_particles_render_data_block
			{
				float positionx;//0x0
				float positiony;//0x4
				float positionz;//0x8
				float size;//0xC
				PAD(0x4);//0x10
			};
			TAG_BLOCK_SIZE_ASSERT(s_particles_render_data_block, 0x14);
			tag_block<s_particles_render_data_block> particles_render_data;//0x0
			struct s_particles_other_data_block
			{
				float velocityx;//0x0
				float velocityy;//0x4
				float velocityz;//0x8
				PAD(0xC);//0xC
				float mass;//0x18
				float creation_time_stamp;//0x1C
			};
			TAG_BLOCK_SIZE_ASSERT(s_particles_other_data_block, 0x20);
			tag_block<s_particles_other_data_block> particles_other_data;//0x8
			PAD(0x20);//0x10
		};
		TAG_BLOCK_SIZE_ASSERT(s_particle_system_data_block, 0x30);
		tag_block<s_particle_system_data_block> particle_system_data;//0x64
		enum class e_type : __int16
		{
			generic = 0,
			snow = 1,
			rain = 2,
			rain_splash = 3,
			bugs = 4,
			sand_storm = 5,
			debris = 6,
			bubbles = 7,
		};
		e_type type;//0x6C
		PAD(0x2);//0x6E
		float mininum_opacity;//0x70
		float maxinum_opacity;//0x74
		float rain_streak_scale;//0x78
		float rain_line_width;//0x7C
		PAD(0xC);//0x80
	};
	TAG_BLOCK_SIZE_ASSERT(s_particle_system_block, 0x8C);
	tag_block<s_particle_system_block> particle_system;//0x0
	struct s_background_plates_block
	{
		tag_reference texture_0;//0x0
		tag_reference texture_1;//0x8
		tag_reference texture_2;//0x10
		float plate_positions_0;//0x18
		float plate_positions_1;//0x1C
		float plate_positions_2;//0x20
		PAD(0x24);//0x24
		float texture_scale_0;//0x48
		float texture_scale_1;//0x4C
		float texture_scale_2;//0x50
		PAD(0x24);//0x54
		float plate_z_near;//0x78
		float plate_z_far;//0x7C
		float depth_blend_z_near;//0x80
		float depth_blend_z_far;//0x84
		float opacity_0;//0x88
		float opacity_1;//0x8C
		float opacity_2;//0x90
		enum class e_flags : __int32
		{
			forward_motion = FLAG(0),
			auto_position_planes = FLAG(1),
			auto_scale_planesauto_update_speed = FLAG(2),
		};
		e_flags flags;//0x94
		real_color_rgb tint_color0;//0x98
		real_color_rgb tint_color1;//0xA4
		real_color_rgb tint_color2;//0xB0
		float mass_1;//0xBC
		float mass_2;//0xC0
		float mass_3;//0xC4
		PAD(0x2E0);//0xC8
	};
	TAG_BLOCK_SIZE_ASSERT(s_background_plates_block, 0x3A8);
	tag_block<s_background_plates_block> background_plates;//0x8
	float wind_tiling_scale;//0x10
	PAD(0xC);//0x14
	float primary_rate_of_change;//0x20
	float primary_min_strength;//0x24
	PAD(0x20);//0x28
	float gust_diretional_rate_of_change;//0x48
	float gust_strength_rate_of_change;//0x4C
	float gust_cone_angle;//0x50
	PAD(0x38);//0x54
	float turbulance_rate_of_change;//0x8C
	PAD(0xC);//0x90
	float gravity_constant;//0x9C
	struct s_wind_pirmitives_block
	{
		PAD(0xC);//0x0
		float radius;//0xC
		float strength;//0x10
		enum class e_wind_primitive_type : __int16
		{
			vortex = 0,
			gust = 1,
			implosion = 2,
			explosion = 3,
		};
		e_wind_primitive_type wind_primitive_type;//0x14
		PAD(0x2);//0x16
	};
	TAG_BLOCK_SIZE_ASSERT(s_wind_pirmitives_block, 0x18);
	tag_block<s_wind_pirmitives_block> wind_pirmitives;//0xA0
	PAD(0x4);//0xA8
	float fade_radius;//0xAC
};
TAG_GROUP_SIZE_ASSERT(s_weather_system_group_definition, 0xB0);
