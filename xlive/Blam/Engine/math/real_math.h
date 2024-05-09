#pragma once

#define k_real_math_epsilon 0.000099999997f

#define DEGREES_TO_RADIANS(angle) ((real32)((angle) * M_PI / 180.0))
#define RADIANS_TO_DEGREES(angle) ((real32)((angle) * 180.0 / M_PI))

union real_point2d
{
	real32 v[2];
	struct { real32 x, y; };
};
ASSERT_STRUCT_SIZE(real_point2d, sizeof(real32) * 2);

typedef real32 real_angle;

union real_euler_angles2d
{
	real32 v[2];
	struct { real_angle yaw, pitch; };
};
ASSERT_STRUCT_SIZE(real_euler_angles2d, sizeof(real_angle) * 2);

union real_euler_angles3d
{
	real32 v[3];
	struct { real_angle yaw, pitch, roll; };
};
ASSERT_STRUCT_SIZE(real_euler_angles3d, sizeof(real_angle) * 3);

union real_vector4d
{
	real32 v[4];
	struct { real32 i, j, k, w; };
};
ASSERT_STRUCT_SIZE(real_vector4d, sizeof(real32) * 4);

union real_vector2d
{
	real32 v[2];
	struct { real32 i, j; };
};
ASSERT_STRUCT_SIZE(real_vector2d, sizeof(real32) * 2);

union real_vector3d
{
	real32 v[3];
	struct { real_vector2d vector2d; };
	struct { real32 i, j, k; };
	struct { real32 x, y, z; };
};
ASSERT_STRUCT_SIZE(real_vector3d, sizeof(real32) * 3);

typedef real_vector3d real_point3d;

struct real_plane2d
{
	real_vector2d normal;
	real32 distance;
};
ASSERT_STRUCT_SIZE(real_plane2d, sizeof(real_vector2d) + 4);

struct real_plane3d
{
	real_vector3d normal;
	real32 distance;
};
ASSERT_STRUCT_SIZE(real_plane3d, sizeof(real_vector3d) + sizeof(real32));

union real_quaternion
{
	struct { real32 i, j, k, w; };
	struct { real_vector3d vector; };
	struct { real32 n[4]; } v;
};
ASSERT_STRUCT_SIZE(real_quaternion, sizeof(real32) * 4);

union real_bounds
{
	real32 v[2];
	struct { real32 lower, upper; };
};
ASSERT_STRUCT_SIZE(real_bounds, sizeof(real32) * 2);

union real_rectangle2d
{
	real32 v[4];
	struct { real32 x0, x1, y0, y1; };
};
ASSERT_STRUCT_SIZE(real_rectangle2d, sizeof(real32) * 4);

union real_rectangle3d
{
	real32 v[6];
	struct { real32 x0, x1, y0, y1, z0, z1; };
};
ASSERT_STRUCT_SIZE(real_rectangle3d, sizeof(real32) * 6);

class c_quantized_orientation
{
public:
	int16 rotation_x;
	int16 rotation_y;
	int16 rotation_z;
	int16 rotation_w;
	real_point3d default_translation;
	real32 default_scale;
};
ASSERT_STRUCT_SIZE(c_quantized_orientation, 24);

struct real_orientation
{
	real_quaternion quaternion;
	real_point3d position;
	real32 scale;
};
ASSERT_STRUCT_SIZE(real_orientation, 32);

const real_vector2d global_forward2d = { 1.0f, 0.0f };
const real_vector3d global_forward3d = { 1.0f, 0.0f, 0.0f };
const real_vector3d global_left3d = { 0.0f, 1.0f, 0.0f };
const real_vector3d global_up3d = { 0.0f, 0.0f, 1.0f };

const real_vector3d global_back3d = { -1.0f, 0.0f, 0.0f };
const real_vector3d global_right3d = { 0.0f, -1.0f, 0.0f };
const real_vector3d global_down3d = { 0.0f, 0.0f, -1.0f };

const real_vector3d global_zero_vector3d = { 0.0f, 0.0f, 0.0f };
const real_orientation global_identity_orientation = { {0.0f, 0.0f, 0.0f, 1.0f,}, global_zero_vector3d, 1.0f };

static BLAM_MATH_INL real32 square_root(real32 f)
{
	return sqrt(f);
}

static BLAM_MATH_INL real32 reciprocal_square_root(real32 f)
{
	return 1.0f / square_root(f);
}

static BLAM_MATH_INL real32 dot_product2d(const real_vector2d* a, const real_vector2d* b)
{
	return (a->i * b->i) + (a->j * b->j);
}

static BLAM_MATH_INL real32 dot_product3d(const real_vector3d* a, const real_vector3d* b)
{
	return a->i * b->i + a->j * b->j + a->k * b->k;
}

static BLAM_MATH_INL real32 dot_product4d_quaternion(const real_quaternion* a, const real_quaternion* b)
{
	return a->i * b->i + a->j * b->j + a->k * b->k + a->w * b->w;
}

static BLAM_MATH_INL real32 magnitude_squared3d(const real_vector3d* vector)
{
	return vector->i * vector->i + vector->j * vector->j + vector->k * vector->k;
}

static BLAM_MATH_INL real32 magnitude3d(const real_vector3d* vector)
{
	real32 magnitude_squared = magnitude_squared3d(vector);
	return square_root(magnitude_squared);
}

static BLAM_MATH_INL bool valid_real(real32 value)
{
	int32 value_as_int = *(int32*)&value;
	return (value_as_int & 0x7F800000) != 0x7F800000;
}

static BLAM_MATH_INL bool valid_realcmp(real32 a, real32 b)
{
	real32 result = a - b;
	return valid_real(result) && fabs(result) < 0.01f;
}

static BLAM_MATH_INL bool valid_real_normal3d(const real_vector3d* normal)
{
	real32 magnitude = magnitude_squared3d(normal);
	return valid_realcmp(magnitude, 1.0f);
}

static BLAM_MATH_INL real_vector2d* scale_vector2d(const real_vector2d* in, real32 scale, real_vector2d* out)
{
	out->i = in->i * scale;
	out->j = in->j * scale;
	return out;
}

static BLAM_MATH_INL real_vector3d* scale_vector3d(const real_vector3d* in, real32 scale, real_vector3d* out)
{
	out->i = in->i * scale;
	out->j = in->j * scale;
	out->k = in->k * scale;
	return out;
}

static BLAM_MATH_INL real_vector3d* add_vectors3d(const real_vector3d* a, const real_vector3d* b, real_vector3d* out)
{
	out->i = a->i + b->i;
	out->j = a->j + b->j;
	out->k = a->k + b->k;
	return out;
}

static BLAM_MATH_INL real_vector3d* multiply_vectors3d(const real_vector3d* a, const real_vector3d* b, real_vector3d* out)
{
	out->i = a->i * b->i;
	out->j = a->j * b->j;
	out->k = a->k * b->k;
	return out;
}

static BLAM_MATH_INL real_vector3d* subtract_vector3d(const real_vector3d* a, const real_vector3d* b, real_vector3d* out)
{
	out->i = a->i - b->i;
	out->j = a->j - b->j;
	out->k = a->k - b->k;
	return out;
}

static BLAM_MATH_INL real_vector3d* vector_from_points3d(const real_point3d* a, const real_point3d* b, real_vector3d* out)
{
	subtract_vector3d(a, b, out);
	return out;
}

static BLAM_MATH_INL real_point3d* point_from_line3d(const real_point3d* point, const real_vector3d* direction, real32 length, real_point3d* out)
{
	real_vector3d direction_scaled;
	scale_vector3d(direction, length, &direction_scaled);
	add_vectors3d(&direction_scaled, point, out);
	return out;
}

static BLAM_MATH_INL real32 normalize3d(real_vector3d* v1)
{
	real32 length = magnitude3d(v1);
	if (abs(length) >= k_real_math_epsilon)
	{
		scale_vector3d(v1, 1.0f / length, v1);
	}
	else
	{
		length = 0.0f;	// vector already normalized
	}
	return length;
}

static BLAM_MATH_INL real_vector3d* cross_product3d(const real_vector3d* up, const real_vector3d* forward, real_vector3d* out_left)
{
	out_left->i = up->j * forward->k - up->k * forward->j;
	out_left->j = up->k * forward->i - up->i * forward->k;
	out_left->k = up->i * forward->j - up->j * forward->i;
	return out_left;
}

static BLAM_MATH_INL void set_real_point3d(real_point3d* point, real32 x, real32 y, real32 z)
{
	point->x = x;
	point->y = y;
	point->z = z;
	return;
}

void __cdecl real_math_initialize(void);

void __cdecl real_math_reset_precision(void);

real32 distance_squared2d(const real_point2d* a, const real_point2d* b);

real32 distance2d(const real_point2d* a, const real_point2d* b);

real32 normalize2d(real_vector2d* vector);

real_vector2d* perpendicular2d(const real_vector2d* in, real_vector2d* out);

real_quaternion* quaternion_normalize(real_quaternion* quaternion);

real_quaternion* fast_quaternion_interpolate_and_normalize(const real_quaternion* previous, const real_quaternion* current, real32 fractional_ticks, real_quaternion* quaternion);

real32 normalize3d_with_default(real_vector3d* a, const real_vector3d* b);

bool valid_real_vector3d_axes2(real_vector3d* forward, real_vector3d* up);

real32 magnitude3d(const real_vector3d* v1);

real32 distance_squared3d(const real_point3d* p1, const real_point3d* p2);

real32 distance3d(const real_point3d* p1, const real_point3d* p2);

bool limit3d(real_vector3d* v, real32 limit);

real_point3d* points_interpolate(const real_vector3d* previous_point, const real_point3d* target_point, real32 fractional_tick, real_point3d* out);

real32 scale_interpolate(real32 previous_scale, real32 current_scale, real32 fractional_tick, real32* out_scale);

real_vector3d* __cdecl perpendicular3d(const real_vector3d* in, real_vector3d* out);

real_vector3d* __cdecl generate_up_vector3d(const real_vector3d* forward, real_vector3d* up);
