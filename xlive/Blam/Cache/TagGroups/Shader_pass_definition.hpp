#pragma once

//#pragma once
//#include "Blam/Maths/real_math.h"
//#include "Blam/Common/Common.h"
//#include "Blam/Cache/DataTypes.h"
//
///*********************************************************************
//* name: shader_pass
//* group_tag : spas
//* header size : 36
//* *********************************************************************/
//#pragma pack(push,1)
//struct s_shader_pass_group_definition
//{
//	data_block documentation;//0x0
//	struct s_parameters_block
//	{
//		string_id name;//0x0
//		data_block explanation;//0x4
//		enum class e_type : __int16
//		{
//			bitmap = 0,
//			value = 1,
//			color = 2,
//			switch_ = 3,
//		};
//		e_type type;//0xC
//		enum class e_flags : __int16
//		{
//			no_bitmap_lod = FLAG(0),
//			required_parameter = FLAG(1),
//		};
//		e_flags flags;//0xE
//		tag_reference default_bitmap;//0x10
//		float default_const_value;//0x18
//		real_color_rgb default_const_color;//0x1C
//		enum class e_source_extern : __int16
//		{
//			none = 0,
//			global_eye_forward_vector_z = 1,
//			global_eye_right_vector_x = 2,
//			global_eye_up_vector_y = 3,
//			object_primary_color = 4,
//			object_secondary_color = 5,
//			object_function_value = 6,
//			light_diffuse_color = 7,
//			light_specular_color = 8,
//			light_forward_vector_z = 9,
//			light_right_vector_x = 10,
//			light_up_vector_y = 11,
//			light_objectrelative_forward_vector_z = 12,
//			light_objectrelative_right_vector_x = 13,
//			light_objectrelative_up_vector_y = 14,
//			light_object_falloff_value = 15,
//			light_object_gel_color = 16,
//			lightmap_object_ambient_factor = 17,
//			lightmap_object_direct_vector = 18,
//			lightmap_object_direct_color = 19,
//			lightmap_object_indirect_vector = 20,
//			lightmap_object_indirect_color = 21,
//			old_fog_atmospheric_color = 22,
//			old_fog_atmospheric_max_density = 23,
//			old_fog_planar_color = 24,
//			old_fog_planar_max_density = 25,
//			old_fog_atmospheric_planar_blend_value = 26,
//			old_fog_object_atmospheric_density = 27,
//			old_fog_object_planar_density = 28,
//			old_fog_object_color = 29,
//			old_fog_object_density = 30,
//			object_model_alpha = 31,
//			object_shadow_alpha = 32,
//			light_overbrighten_diffuse_shift = 33,
//			light_overbrighten_specular_shift = 34,
//			light_diffuse_contrast = 35,
//			light_specular_gel = 36,
//			shader_specular_type = 37,
//			pad3 = 38,
//			pad3scale = 39,
//			pad_thai = 40,
//			taco_salad = 41,
//			anisotropic_binormal = 42,
//			object_light_shadow_fade = 43,
//			light_shadow_fade = 44,
//			old_fog_atmospheric_density = 45,
//			old_fog_planar_density = 46,
//			old_fog_planar_density_invert = 47,
//			object_change_color_tertiary = 48,
//			object_change_color_quaternary = 49,
//			lightmap_object_specular_color = 50,
//			shader_lightmap_type = 51,
//			lightmap_object_ambient_color = 52,
//			shader_lightmap_specular_brightness = 53,
//			global_lightmap_shadow_darkening = 54,
//			lightmap_object_env_brightness = 55,
//			fog_atmospheric_max_density = 56,
//			fog_atmospheric_color = 57,
//			fog_atmospheric_color_adj = 58,
//			fog_atmospheric_planar_blend = 59,
//			fog_atmospheric_planar_blend_adj_inv = 60,
//			fog_atmospheric_planar_blend_adj = 61,
//			fog_secondary_max_density = 62,
//			fog_secondary_color = 63,
//			fog_secondary_color_adj = 64,
//			fog_atmospheric_secondary_blend = 65,
//			fog_atmospheric_secondary_blend_adj_inv = 66,
//			fog_atmospheric_secondary_blend_adj = 67,
//			fog_sky_density = 68,
//			fog_sky_color = 69,
//			fog_sky_color_adj = 70,
//			fog_planar_max_density = 71,
//			fog_planar_color = 72,
//			fog_planar_color_adj = 73,
//			fog_planar_eye_density = 74,
//			fog_planar_eye_density_adj_inv = 75,
//			fog_planar_eye_density_adj = 76,
//			hud_waypoint_primary_color = 77,
//			hud_waypoint_secondary_color = 78,
//			lightmap_object_specular_color_times_one_half = 79,
//			light_specular_enabled = 80,
//			light_definition_specular_enabled = 81,
//			object_active_camo_amount = 82,
//			object_super_camo_amount = 83,
//			hud_custom_color_1 = 84,
//			hud_custom_color_2 = 85,
//			hud_custom_color_3 = 86,
//			hud_custom_color_4 = 87,
//			object_active_camo_rgb = 88,
//			fog_patchy_plane_nxyz = 89,
//			fog_patchy_plane_dw = 90,
//			hud_global_fade = 91,
//			screen_effect_primary = 92,
//			screen_effect_secondary = 93,
//		};
//		e_source_extern source_extern;//0x28
//		PAD(0x2);//0x2A
//	};
//	TAG_BLOCK_SIZE_ASSERT(s_parameters_block, 0x2C);
//	tag_block<s_parameters_block> parameters;//0x8
//	PAD(0x4);//0x10
//	struct s_implementations_block
//	{
//		enum class e_flags : __int16
//		{
//			delete_from_cache_file = FLAG(0),
//			critical = FLAG(1),
//		};
//		e_flags flags;//0x0
//		PAD(0x2);//0x2
//		struct s_textures_block
//		{
//			string_id source_parameter;//0x0
//			enum class e_source_extern : __int16
//			{
//				none = 0,
//				global_vector_normalization = 1,
//				unused_1 = 2,
//				global_target_texaccum = 3,
//				unused_2 = 4,
//				global_target_frame_buffer = 5,
//				global_target_z = 6,
//				unused_3 = 7,
//				global_target_shadow = 8,
//				light_falloff = 9,
//				light_gel = 10,
//				lightmap = 11,
//				unused_4 = 12,
//				global_shadow_buffer = 13,
//				global_gradient_separate = 14,
//				global_gradient_product = 15,
//				hud_bitmap = 16,
//				global_active_camo = 17,
//				global_texture_camera = 18,
//				global_water_reflection = 19,
//				global_water_refraction = 20,
//				global_aux_1 = 21,
//				global_aux_2 = 22,
//				global_particle_distortion = 23,
//				global_convolution_1 = 24,
//				global_convolution_2 = 25,
//				shader_active_camo_bump = 26,
//				first_person_scope = 27,
//			};
//			e_source_extern source_extern;//0x4
//			PAD(0x4);//0x6
//			enum class e_mode : __int16
//			{
//				NUM_2d = 0,
//				NUM_3d = 1,
//				cube_map = 2,
//				passthrough = 3,
//				texkill = 4,
//				NUM_2d_dependent_ar = 5,
//				NUM_2d_dependent_gb = 6,
//				NUM_2d_bumpenv = 7,
//				NUM_2d_bumpenv_luminance = 8,
//				NUM_3d_brdf = 9,
//				dot_product = 10,
//				dot_product_2d = 11,
//				dot_product_3d = 12,
//				dot_product_cube_map = 13,
//				dot_product_zw = 14,
//				dot_reflect_diffuse = 15,
//				dot_reflect_specular = 16,
//				dot_reflect_specular_const = 17,
//				none = 18,
//			};
//			e_mode mode;//0xA
//			PAD(0x2);//0xC
//			enum class e_dot_mapping : __int16
//			{
//				NUM_0_to_1 = 0,
//				signed_d3d = 1,
//				signed_gl = 2,
//				signed_nv = 3,
//				hilo_0_to_1 = 4,
//				hilo_signed_hemisphere_d3d = 5,
//				hilo_signed_hemisphere_gl = 6,
//				hilo_signed_hemisphere_nv = 7,
//			};
//			e_dot_mapping dot_mapping;//0xE
//			__int16 input_stage;//0x10
//			PAD(0x2);//0x12
//			struct s_address_state_block
//			{
//				enum class e_u_address_mode : __int16
//				{
//					wrap = 0,
//					mirror = 1,
//					clamp = 2,
//					border = 3,
//					clamp_to_edge = 4,
//				};
//				e_u_address_mode u_address_mode;//0x0
//				enum class e_v_address_mode : __int16
//				{
//					wrap = 0,
//					mirror = 1,
//					clamp = 2,
//					border = 3,
//					clamp_to_edge = 4,
//				};
//				e_v_address_mode v_address_mode;//0x2
//				enum class e_w_address_mode : __int16
//				{
//					wrap = 0,
//					mirror = 1,
//					clamp = 2,
//					border = 3,
//					clamp_to_edge = 4,
//				};
//				e_w_address_mode w_address_mode;//0x4
//				PAD(0x2);//0x6
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_address_state_block, 0x8);
//			tag_block<s_address_state_block> address_state;//0x14
//			struct s_filter_state_block
//			{
//				enum class e_mag_filter : __int16
//				{
//					none = 0,
//					point_sampled = 1,
//					linear = 2,
//					anisotropic = 3,
//					quincunx = 4,
//					gaussian_cubic = 5,
//				};
//				e_mag_filter mag_filter;//0x0
//				enum class e_min_filter : __int16
//				{
//					none = 0,
//					point_sampled = 1,
//					linear = 2,
//					anisotropic = 3,
//					quincunx = 4,
//					gaussian_cubic = 5,
//				};
//				e_min_filter min_filter;//0x2
//				enum class e_mip_filter : __int16
//				{
//					none = 0,
//					point_sampled = 1,
//					linear = 2,
//					anisotropic = 3,
//					quincunx = 4,
//					gaussian_cubic = 5,
//				};
//				e_mip_filter mip_filter;//0x4
//				PAD(0x2);//0x6
//				float mipmap_bias;//0x8
//				__int16 max_mipmap_index;//0xC
//				enum class e_anisotropy : __int16
//				{
//					non_anisotropic = 0,
//					NUM_2tap = 1,
//					NUM_3tap = 2,
//					NUM_4tap = 3,
//				};
//				e_anisotropy anisotropy;//0xE
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_filter_state_block, 0x10);
//			tag_block<s_filter_state_block> filter_state;//0x1C
//			struct s_kill_state_block
//			{
//				enum class e_flags : __int16
//				{
//					alpha_kill = FLAG(0),
//				};
//				e_flags flags;//0x0
//				PAD(0x2);//0x2
//				enum class e_colorkey_mode : __int16
//				{
//					disabled = 0,
//					zero_alpha = 1,
//					zero_argb = 2,
//					kill = 3,
//				};
//				e_colorkey_mode colorkey_mode;//0x4
//				PAD(0x6);//0x6
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_kill_state_block, 0xC);
//			tag_block<s_kill_state_block> kill_state;//0x24
//			struct s_misc_state_block
//			{
//				enum class e_component_sign_flags : __int16
//				{
//					r_signed = FLAG(0),
//					g_signed = FLAG(1),
//					b_signed = FLAG(2),
//					a_signed = FLAG(3),
//				};
//				e_component_sign_flags component_sign_flags;//0x0
//				PAD(0x6);//0x2
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_misc_state_block, 0x8);
//			tag_block<s_misc_state_block> misc_state;//0x2C
//			struct s_constants_block
//			{
//				string_id source_parameter;//0x0
//				PAD(0x2);//0x4
//				enum class e_constant : __int16
//				{
//					mipmap_bias_value = 0,
//					colorkey_color = 1,
//					border_color = 2,
//					border_alpha_value = 3,
//					bumpenv_mat_0_0 = 4,
//					bumpenv_mat_0_1 = 5,
//					bumpenv_mat_1_0 = 6,
//					bumpenv_mat_1_1 = 7,
//					bumpenv_lum_scale_value = 8,
//					bumpenv_lum_offset_value = 9,
//				};
//				e_constant constant;//0x6
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_constants_block, 0x8);
//			tag_block<s_constants_block> constants;//0x34
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_textures_block, 0x3C);
//		tag_block<s_textures_block> textures;//0x4
//		tag_reference vertex_shader;//0xC
//		struct s_vs_constants_block
//		{
//			string_id source_parameter;//0x0
//			enum class e_scale_by_texture_stage : __int16
//			{
//				none = 0,
//				stage_0 = 1,
//				stage_1 = 2,
//				stage_2 = 3,
//				stage_3 = 4,
//			};
//			e_scale_by_texture_stage scale_by_texture_stage;//0x4
//			enum class e_register_bank : __int16
//			{
//				vn_15 = 0,
//				cn_12 = 1,
//			};
//			e_register_bank register_bank;//0x6
//			__int16 register_index;//0x8
//			enum class e_component_mask : __int16
//			{
//				x_value = 0,
//				y_value = 1,
//				z_value = 2,
//				w_value = 3,
//				xyzrgb_color = 4,
//				x_uniform_scale = 5,
//				y_uniform_scale = 6,
//				z_uniform_scale = 7,
//				w_uniform_scale = 8,
//				xy_2d_scale = 9,
//				zw_2d_scale = 10,
//				xy_2d_translation = 11,
//				zw_2d_translation = 12,
//				xyzw_2d_simple_xform = 13,
//				xyw_row_1_2d_affine_xform = 14,
//				xyw_row_2_2d_affine_xform = 15,
//				xyz_3d_scale = 16,
//				xyz_3d_translation = 17,
//				xyzw_row_1_3d_affine_xform = 18,
//				xyzw_row_2_3d_affine_xform = 19,
//				xyzw_row_3_3d_affine_xform = 20,
//			};
//			e_component_mask component_mask;//0xA
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_vs_constants_block, 0xC);
//		tag_block<s_vs_constants_block> vs_constants;//0x14
//		data_block pixel_shader_code_unused;//0x1C
//		enum class e_channels : __int16
//		{
//			all = 0,
//			color_only = 1,
//			alpha_only = 2,
//			custom = 3,
//		};
//		e_channels channels;//0x24
//		enum class e_alphablend : __int16
//		{
//			disabled = 0,
//			add = 1,
//			multiply = 2,
//			add_src_times_dstalpha = 3,
//			add_src_times_srcalpha = 4,
//			add_dst_times_srcalpha_inverse = 5,
//			alpha_blend = 6,
//			custom = 7,
//		};
//		e_alphablend alphablend;//0x26
//		enum class e_depth : __int16
//		{
//			disabled = 0,
//			default_opaque = 1,
//			default_opaque_write = 2,
//			default_transparent = 3,
//			custom = 4,
//		};
//		e_depth depth;//0x28
//		PAD(0x2);//0x2A
//		struct s_channel_state_block
//		{
//			enum class e_flags : __int16
//			{
//				r = FLAG(0),
//				g = FLAG(1),
//				b = FLAG(2),
//				a = FLAG(3),
//			};
//			e_flags flags;//0x0
//			PAD(0x2);//0x2
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_channel_state_block, 0x4);
//		tag_block<s_channel_state_block> channel_state;//0x2C
//		struct s_alphablend_state_block
//		{
//			enum class e_blend_function : __int16
//			{
//				add = 0,
//				subtract = 1,
//				reverse_subtract = 2,
//				min = 3,
//				max = 4,
//				add_signed = 5,
//				reverse_subtract_signed = 6,
//				logicop = 7,
//			};
//			e_blend_function blend_function;//0x0
//			enum class e_blend_src_factor : __int16
//			{
//				zero = 0,
//				one = 1,
//				srccolor = 2,
//				srccolor_inverse = 3,
//				srcalpha = 4,
//				srcalpha_inverse = 5,
//				dstcolor = 6,
//				dstcolor_inverse = 7,
//				dstalpha = 8,
//				dstalpha_inverse = 9,
//				srcalpha_saturate = 10,
//				constant_color = 11,
//				constant_color_inverse = 12,
//				constant_alpha = 13,
//				constant_alpha_inverse = 14,
//			};
//			e_blend_src_factor blend_src_factor;//0x2
//			enum class e_blend_dst_factor : __int16
//			{
//				zero = 0,
//				one = 1,
//				srccolor = 2,
//				srccolor_inverse = 3,
//				srcalpha = 4,
//				srcalpha_inverse = 5,
//				dstcolor = 6,
//				dstcolor_inverse = 7,
//				dstalpha = 8,
//				dstalpha_inverse = 9,
//				srcalpha_saturate = 10,
//				constant_color = 11,
//				constant_color_inverse = 12,
//				constant_alpha = 13,
//				constant_alpha_inverse = 14,
//			};
//			e_blend_dst_factor blend_dst_factor;//0x4
//			PAD(0x6);//0x6
//			enum class e_logicop_flags : __int16
//			{
//				src_0_dst_0 = FLAG(0),
//				src_0_dst_1 = FLAG(1),
//				src_1_dst_0 = FLAG(2),
//				src_1_dst_1 = FLAG(3),
//			};
//			e_logicop_flags logicop_flags;//0xC
//			PAD(0x2);//0xE
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_alphablend_state_block, 0x10);
//		tag_block<s_alphablend_state_block> alphablend_state;//0x34
//		struct s_alphatest_state_block
//		{
//			enum class e_flags : __int16
//			{
//				alphatest_enabled = FLAG(0),
//				samplealpha_to_coverage = FLAG(1),
//				samplealpha_to_one = FLAG(2),
//			};
//			e_flags flags;//0x0
//			enum class e_alpha_compare_function : __int16
//			{
//				never = 0,
//				less = 1,
//				equal = 2,
//				less_or_equal = 3,
//				greater = 4,
//				not_equal = 5,
//				greater_or_equal = 6,
//				always = 7,
//			};
//			e_alpha_compare_function alpha_compare_function;//0x2
//			__int16 alphatest_ref;//0x4
//			PAD(0x2);//0x6
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_alphatest_state_block, 0x8);
//		tag_block<s_alphatest_state_block> alphatest_state;//0x3C
//		struct s_depth_state_block
//		{
//			enum class e_mode : __int16
//			{
//				use_z = 0,
//				use_w = 1,
//			};
//			e_mode mode;//0x0
//			enum class e_depth_compare_function : __int16
//			{
//				never = 0,
//				less = 1,
//				equal = 2,
//				less_or_equal = 3,
//				greater = 4,
//				not_equal = 5,
//				greater_or_equal = 6,
//				always = 7,
//			};
//			e_depth_compare_function depth_compare_function;//0x2
//			enum class e_flags : __int16
//			{
//				depth_write = FLAG(0),
//				offset_points = FLAG(1),
//				offset_lines = FLAG(2),
//				offset_triangles = FLAG(3),
//				clip_control_dont_cull_primitive = FLAG(4),
//				clip_control_clamp = FLAG(5),
//				clip_control_ignore_w_sign = FLAG(6),
//			};
//			e_flags flags;//0x4
//			PAD(0x2);//0x6
//			float depth_bias_slope_scale;//0x8
//			float depth_bias;//0xC
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_depth_state_block, 0x10);
//		tag_block<s_depth_state_block> depth_state;//0x44
//		struct s_cull_state_block
//		{
//			enum class e_mode : __int16
//			{
//				none = 0,
//				cw = 1,
//				ccw = 2,
//			};
//			e_mode mode;//0x0
//			enum class e_front_face : __int16
//			{
//				cw = 0,
//				ccw = 1,
//			};
//			e_front_face front_face;//0x2
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_cull_state_block, 0x4);
//		tag_block<s_cull_state_block> cull_state;//0x4C
//		struct s_fill_state_block
//		{
//			enum class e_flags : __int16
//			{
//				flat_shading = FLAG(0),
//				edge_antialiasing = FLAG(1),
//			};
//			e_flags flags;//0x0
//			enum class e_fill_mode : __int16
//			{
//				solid = 0,
//				wireframe = 1,
//				points = 2,
//			};
//			e_fill_mode fill_mode;//0x2
//			enum class e_back_fill_mode : __int16
//			{
//				solid = 0,
//				wireframe = 1,
//				points = 2,
//			};
//			e_back_fill_mode back_fill_mode;//0x4
//			PAD(0x2);//0x6
//			float line_width;//0x8
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_fill_state_block, 0xC);
//		tag_block<s_fill_state_block> fill_state;//0x54
//		struct s_misc_state_block
//		{
//			enum class e_flags : __int16
//			{
//				yuv_to_rgb = FLAG(0),
//				NUM_16bit_dither = FLAG(1),
//				NUM_32bit_dxt1_noise = FLAG(2),
//			};
//			e_flags flags;//0x0
//			PAD(0x6);//0x2
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_misc_state_block, 0x8);
//		tag_block<s_misc_state_block> misc_state;//0x5C
//		struct s_constants_block
//		{
//			string_id source_parameter;//0x0
//			PAD(0x2);//0x4
//			enum class e_constant : __int16
//			{
//				constant_blend_color = 0,
//				constant_blend_alpha_value = 1,
//				alphatest_ref_value = 2,
//				depth_bias_slope_scale_value = 3,
//				depth_bias_value = 4,
//				line_width_value = 5,
//				fog_color = 6,
//			};
//			e_constant constant;//0x6
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_constants_block, 0x8);
//		tag_block<s_constants_block> constants;//0x64
//		tag_reference pixel_shader;//0x6C
//	};
//	TAG_BLOCK_SIZE_ASSERT(s_implementations_block, 0x74);
//	tag_block<s_implementations_block> implementations;//0x14
//	struct s_postprocess_definition_block
//	{
//		struct s_implementations_block
//		{
//			__int16 block_index_data_1;//0x0
//			__int16 block_index_data_2;//0x2
//			__int16 block_index_data_;//0x4
//			PAD(0xF0);//0x6
//			__int16 block_index_dat_4;//0xF6
//			__int16 block_index_data_5;//0xF8
//			__int16 block_index_data_6;//0xFA
//			tag_reference vertex_shader;//0xFC
//			PAD(0x18);//0x104
//			__int16 block_index_data_7;//0x11C
//			__int16 block_index_data_8;//0x11E
//			__int16 block_index_data_9;//0x120
//			__int16 block_index_data_10;//0x122
//			__int16 block_index_data_11;//0x124
//			__int16 block_index_data_12;//0x126
//			__int16 block_index_data_13;//0x128
//			__int16 block_index_data_14;//0x12A
//			__int16 block_index_data_15;//0x12C
//			__int16 block_index_data_17;//0x130
//			__int16 block_index_data_16;//0x12E
//			struct s_pixel_shader_block
//			{
//				__int32 pixel_shader_handle_runtime_1;//0x0
//				__int32 pixel_shader_handle_runtime_2;//0x4
//				__int32 pixel_shader_handle_runtime_3;//0x8
//				struct s_constant_register_defaults_block
//				{
//					__int32 defaults;//0x0
//				};
//				TAG_BLOCK_SIZE_ASSERT(s_constant_register_defaults_block, 0x4);
//				tag_block<s_constant_register_defaults_block> constant_register_defaults;//0xC
//				data_block compiled_shader_1;//0x14
//				data_block compiled_shader_2;//0x1C
//				data_block compiled_shader_3;//0x24
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_pixel_shader_block, 0x2C);
//			tag_block<s_pixel_shader_block> pixel_shader;//0x132
//			struct s_pixel_shader_switch_extern_map_block
//			{
//				__int8 switch_parameter;//0x0
//				__int8 case_scalar;//0x1
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_pixel_shader_switch_extern_map_block, 0x2);
//			tag_block<s_pixel_shader_switch_extern_map_block> pixel_shader_switch_extern_map;//0x13A
//			struct s_pixel_shader_index_block_block
//			{
//				__int8 pixel_shader_index;//0x0
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_pixel_shader_index_block_block, 0x1);
//			tag_block<s_pixel_shader_index_block_block> pixel_shader_index_block;//0x142
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_implementations_block, 0x14A);
//		tag_block<s_implementations_block> implementations;//0x0
//		struct s_textures_block
//		{
//			__int8 bitmap_parameter_index;//0x0
//			__int8 bitmap_extern_index;//0x1
//			__int8 texture_stage_index;//0x2
//			__int8 flags;//0x3
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_textures_block, 0x4);
//		tag_block<s_textures_block> textures;//0x8
//		struct s_render_states_block
//		{
//			__int8 state_index;//0x0
//			__int32 state_value;//0x1
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_render_states_block, 0x5);
//		tag_block<s_render_states_block> render_states;//0x10
//		struct s_texture_states_block
//		{
//			PAD(0x18);//0x0
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_texture_states_block, 0x18);
//		tag_block<s_texture_states_block> texture_states;//0x18
//		struct s_ps_fragments_block
//		{
//			__int8 switch_parameter_index;//0x0
//			__int16 block_index_data;//0x1
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_ps_fragments_block, 0x3);
//		tag_block<s_ps_fragments_block> ps_fragments;//0x20
//		struct s_ps_permutations_block
//		{
//			__int16 enum_index;//0x0
//			__int16 flags;//0x2
//			__int16 block_index_data;//0x4
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_ps_permutations_block, 0x6);
//		tag_block<s_ps_permutations_block> ps_permutations;//0x28
//		struct s_ps_combiners_block
//		{
//			PAD(0x18);//0x0
//			__int8 color_a_register_ptr_index;//0x18
//			__int8 color_b_register_ptr_index;//0x19
//			__int8 color_c_register_ptr_index;//0x1A
//			__int8 color_d_register_ptr_index;//0x1B
//			__int8 alpha_a_register_ptr_index;//0x1C
//			__int8 alpha_b_register_ptr_index;//0x1D
//			__int8 alpha_c_register_ptr_index;//0x1E
//			__int8 alpha_d_register_ptr_index;//0x1F
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_ps_combiners_block, 0x20);
//		tag_block<s_ps_combiners_block> ps_combiners;//0x30
//		struct s_externs_block
//		{
//			PAD(0x3);//0x0
//			__int8 extern_index;//0x3
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_externs_block, 0x4);
//		tag_block<s_externs_block> externs;//0x38
//		struct s_constants_block
//		{
//			string_id parameter_name;//0x0
//			__int8 component_mask;//0x4
//			__int8 scale_by_texture_stage;//0x5
//			__int8 function_index;//0x6
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_constants_block, 0x7);
//		tag_block<s_constants_block> constants;//0x40
//		struct s_constant_info_block
//		{
//			string_id parameter_name;//0x0
//			PAD(0x3);//0x4
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_constant_info_block, 0x7);
//		tag_block<s_constant_info_block> constant_info;//0x48
//		struct s_old_implementations_block
//		{
//			struct s_render_states_block
//			{
//				__int8 state_index;//0x0
//				__int32 state_value;//0x1
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_render_states_block, 0x5);
//			tag_block<s_render_states_block> render_states;//0x0
//			struct s_texture_stage_states_block
//			{
//				__int8 state_index;//0x0
//				__int8 stage_index;//0x1
//				__int32 state_value;//0x2
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_texture_stage_states_block, 0x6);
//			tag_block<s_texture_stage_states_block> texture_stage_states;//0x8
//			struct s_render_state_parameters_block
//			{
//				__int8 parameter_index;//0x0
//				__int8 parameter_type;//0x1
//				__int8 state_index;//0x2
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_render_state_parameters_block, 0x3);
//			tag_block<s_render_state_parameters_block> render_state_parameters;//0x10
//			struct s_texture_stage_parameters_block
//			{
//				__int8 parameter_index;//0x0
//				__int8 parameter_type;//0x1
//				__int8 state_index;//0x2
//				__int8 stage_index;//0x3
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_texture_stage_parameters_block, 0x4);
//			tag_block<s_texture_stage_parameters_block> texture_stage_parameters;//0x18
//			struct s_textures_block
//			{
//				__int8 stage_index;//0x0
//				__int8 parameter_index;//0x1
//				__int8 global_texture_index;//0x2
//				__int8 flags;//0x3
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_textures_block, 0x4);
//			tag_block<s_textures_block> textures;//0x20
//			struct s_vn_constants_block
//			{
//				__int8 register_index;//0x0
//				__int8 parameter_index;//0x1
//				__int8 destination_mask;//0x2
//				__int8 scale_by_texture_stage;//0x3
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_vn_constants_block, 0x4);
//			tag_block<s_vn_constants_block> vn_constants;//0x28
//			struct s_cn_constants_block
//			{
//				__int8 register_index;//0x0
//				__int8 parameter_index;//0x1
//				__int8 destination_mask;//0x2
//				__int8 scale_by_texture_stage;//0x3
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_cn_constants_block, 0x4);
//			tag_block<s_cn_constants_block> cn_constants;//0x30
//			__int16 block_index_data;//0x38
//			__int16 block_index_data;//0x3A
//			__int16 block_index_data;//0x3C
//			__int16 block_index_data;//0x3E
//			__int16 block_index_data;//0x40
//			__int16 block_index_data;//0x42
//			__int16 block_index_data;//0x44
//			__int16 block_index_data;//0x46
//			__int16 block_index_data;//0x48
//			__int16 block_index_data;//0x4A
//			__int16 block_index_data;//0x4C
//			__int16 block_index_data;//0x4E
//			__int16 block_index_data;//0x50
//			__int16 block_index_data;//0x52
//			__int16 block_index_data;//0x54
//			__int16 block_index_data;//0x56
//			__int16 block_index_data;//0x58
//			__int16 block_index_data;//0x5A
//			__int16 block_index_data;//0x5C
//			__int16 block_index_data;//0x5E
//			__int16 block_index_data;//0x60
//			tag_reference vertex_shader;//0x62
//			PAD(0x18);//0x6A
//			struct s_value_externs_block
//			{
//				__int8 parameter_index;//0x0
//				__int8 extern_index;//0x1
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_value_externs_block, 0x2);
//			tag_block<s_value_externs_block> value_externs;//0x82
//			struct s_color_externs_block
//			{
//				__int8 parameter_index;//0x0
//				__int8 extern_index;//0x1
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_color_externs_block, 0x2);
//			tag_block<s_color_externs_block> color_externs;//0x8A
//			struct s_switch_externs_block
//			{
//				__int8 parameter_index;//0x0
//				__int8 extern_index;//0x1
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_switch_externs_block, 0x2);
//			tag_block<s_switch_externs_block> switch_externs;//0x92
//			__int16 bitmap_parameter_count;//0x9A
//			PAD(0xF2);//0x9C
//			struct s_pixel_shader_fragments_block
//			{
//				__int8 switch_parameter_index;//0x0
//				__int16 block_index_data;//0x1
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_pixel_shader_fragments_block, 0x3);
//			tag_block<s_pixel_shader_fragments_block> pixel_shader_fragments;//0x18E
//			struct s_pixel_shader_permutations_block
//			{
//				__int16 enum_index;//0x0
//				enum class e_flags : __int16
//				{
//					has_final_combiner = FLAG(0),
//				};
//				e_flags flags;//0x2
//				__int16 block_index_data;//0x4
//				__int16 block_index_data;//0x6
//				PAD(0x8);//0x8
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_pixel_shader_permutations_block, 0x10);
//			tag_block<s_pixel_shader_permutations_block> pixel_shader_permutations;//0x196
//			struct s_pixel_shader_combiners_block
//			{
//				PAD(0x18);//0x0
//				__int8 color_a_register_ptr_index;//0x18
//				__int8 color_b_register_ptr_index;//0x19
//				__int8 color_c_register_ptr_index;//0x1A
//				__int8 color_d_register_ptr_index;//0x1B
//				__int8 alpha_a_register_ptr_index;//0x1C
//				__int8 alpha_b_register_ptr_index;//0x1D
//				__int8 alpha_c_register_ptr_index;//0x1E
//				__int8 alpha_d_register_ptr_index;//0x1F
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_pixel_shader_combiners_block, 0x20);
//			tag_block<s_pixel_shader_combiners_block> pixel_shader_combiners;//0x19E
//			struct s_pixel_shader_constants_block
//			{
//				enum class e_parameter_type : __int8
//				{
//					bitmap = 0,
//					value = 1,
//					color = 2,
//					switch_ = 3,
//				};
//				e_parameter_type parameter_type;//0x0
//				__int8 combiner_index;//0x1
//				__int8 register_index;//0x2
//				enum class e_component_mask : __int8
//				{
//					x_value = 0,
//					y_value = 1,
//					z_value = 2,
//					w_value = 3,
//					xyzrgb_color = 4,
//				};
//				e_component_mask component_mask;//0x3
//				PAD(0x2);//0x4
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_pixel_shader_constants_block, 0x6);
//			tag_block<s_pixel_shader_constants_block> pixel_shader_constants;//0x1A6
//			PAD(0x8);//0x1AE
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_old_implementations_block, 0x1B6);
//		tag_block<s_old_implementations_block> old_implementations;//0x50
//	};
//	TAG_BLOCK_SIZE_ASSERT(s_postprocess_definition_block, 0x58);
//	tag_block<s_postprocess_definition_block> postprocess_definition;//0x1C
//};
//TAG_GROUP_SIZE_ASSERT(s_shader_pass_group_definition, 0x24);
//#pragma pack(pop)

