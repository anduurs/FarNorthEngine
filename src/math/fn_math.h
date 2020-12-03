#pragma once

#include <math.h>

#define PI 3.14159265359f
#define to_radians(angle)(angle * (PI / 180.0f))
#define to_degrees(radians)(radians * (180.0f / PI))

struct vec2f
{
	f32 x, y;
};

struct vec2i
{
	int32 x, y;
};

struct vec3f
{
	f32 x, y, z;
};

struct vec4
{
	f32 x, y, z, w;
};

struct AABB
{
	int32 Xmin;
	int32 Xmax;
	int32 Ymin;
	int32 Ymax;
};

// if sorted is true then the function assumes vertices are sorted in y-axis from lower to upper, v1 = min , v3 = max
internal AABB fn_math_get_bounding_box_for_triangle(vec2i v1, vec2i v2, vec2i v3, bool lowerToUpperSorted = false)
{
	AABB result = {};

	if (!lowerToUpperSorted)
	{
		if (v1.y > v2.y) fn_swap(&v1, &v2);
		if (v1.y > v3.y) fn_swap(&v1, &v3);
		if (v2.y > v3.y) fn_swap(&v2, &v3);
	}

	result.Ymin = v1.y;
	result.Ymax = v3.y;

	if (v1.x <= v2.x) result.Xmin = v1.x;
	else result.Xmin = v2.x;

	if (v3.x < result.Xmin) result.Xmin = v3.x;

	if (v1.x >= v2.x) result.Xmax = v1.x;
	else result.Xmax = v2.x;

	if (v3.x > result.Xmax) result.Xmax = v3.x;

	return result;
}

internal inline int32 fn_math_sign(vec2i p1, vec2i p2, vec2i p3)
{
	return ((p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y));
}

internal bool fn_math_point_in_triangle(vec2i pt, vec2i v1, vec2i v2, vec2i v3)
{
	int32 d1 = fn_math_sign(pt, v1, v2);
	int32 d2 = fn_math_sign(pt, v2, v3);
	int32 d3 = fn_math_sign(pt, v3, v1);

	bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

internal inline f32 fn_math_lerpf(f32 a, f32 b, f32 t)
{
	return (a * (1.0f - t)) + (b * t);
}

internal inline f32 fn_math_vec3f_length(const vec3f* v)
{
	return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

internal inline void fn_math_vec3f_normalize(vec3f* v)
{
	f32 len = fn_math_vec3f_length(v);
	v->x = v->x / len;
	v->y = v->y / len;
	v->z = v->z / len;
}

internal inline vec3f fn_math_vec3f_add(const vec3f& lhs, const vec3f& rhs)
{
	vec3f result = {};
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;
	return result;
}

internal inline vec3f fn_math_vec3f_sub(const vec3f& lhs, const vec3f& rhs)
{
	vec3f result = {};
	result.x = lhs.x - rhs.x;
	result.y = lhs.y - rhs.y;
	result.z = lhs.z - rhs.z;
	return result;
}

internal inline f32 fn_math_vec3f_dot(const vec3f& lhs, const vec3f& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

internal inline vec3f fn_math_vec3f_cross(const vec3f& lhs, const vec3f& rhs)
{
	vec3f result = {};
	result.x = lhs.y * rhs.z - lhs.z * rhs.y;
	result.y = lhs.z * rhs.x - lhs.x * rhs.z;
	result.z = lhs.x * rhs.y - lhs.y * rhs.x;
	return result;
}

internal inline vec3f fn_math_vec3f_move_in_direction(const vec3f& position, const vec3f& direction, f32 scalar)
{
	vec3f result = {};
	result.x = position.x + direction.x * scalar;
	result.y = position.y + direction.y * scalar;
	result.z = position.z + direction.z * scalar;
	return result;
}

// QUATERNION SECTION

struct quaternion
{
	f32 x, y, z, w;
};

internal inline f32 fn_math_quat_length(const quaternion& q)
{
	return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

internal inline void fn_math_quat_normalize(quaternion* q)
{
	f32 len = fn_math_quat_length(*q);
	q->x = q->x / len;
	q->y = q->y / len;
	q->z = q->z / len;
	q->w = q->w / len;
}

internal inline quaternion fn_math_quat_conjugate(const quaternion& q)
{
	quaternion result = {};
	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;
	result.w = q.w;
	return result;
}

internal inline quaternion fn_math_quat_mul(const quaternion& lhs, const quaternion& rhs)
{
	quaternion result = {};
	result.x = lhs.x * rhs.w + lhs.w * rhs.x + lhs.y * rhs.z - lhs.z * rhs.y;
	result.y = lhs.y * rhs.w + lhs.w * rhs.y + lhs.z * rhs.x - lhs.x * rhs.z;
	result.z = lhs.z * rhs.w + lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x;
	result.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
	fn_math_quat_normalize(&result);
	return result;
}

internal inline quaternion fn_math_quat_mul_vec3f(const quaternion& q, const vec3f& v)
{
	quaternion result = {};
	result.x = q.w * v.x + q.y * v.z - q.z * v.y;
	result.y = q.w * v.y + q.z * v.x - q.x * v.z;
	result.z = q.w * v.z + q.x * v.y - q.y * v.x;
	result.w = -q.x * v.x - q.y * v.y - q.z * v.z;
	fn_math_quat_normalize(&result);
	return result;
}

internal inline vec3f fn_math_quat_forward(const quaternion& q)
{
	vec3f forward = {};

	forward.x = 2.0f * (q.x * q.z - q.w * q.y);
	forward.y = 2.0f * (q.y * q.z + q.w * q.x);
	forward.z = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);

	fn_math_vec3f_normalize(&forward);

	return forward;
}

internal inline vec3f fn_math_quat_right(const quaternion& q)
{
	vec3f right = {};

	right.x = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	right.y = 2.0f * (q.x * q.y - q.w * q.z);
	right.z = 2.0f * (q.x * q.z + q.w * q.y);

	fn_math_vec3f_normalize(&right);

	return right;
}

internal inline vec3f fn_math_quat_up(const quaternion& q)
{
	vec3f up = {};

	up.x = 2.0f * (q.x * q.y + q.w * q.z);
	up.y = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
	up.z = 2.0f * (q.y * q.z - q.w * q.x);

	fn_math_vec3f_normalize(&up);

	return up;
}

internal inline quaternion fn_math_quat_rotate(f32 angle, const vec3f& axis, const quaternion& rotation)
{
	quaternion newRotation = {};
	f32 sinHalfAngle = sinf(to_radians(angle) / 2.0f);
	newRotation.x = axis.x * sinHalfAngle;
	newRotation.y = axis.y * sinHalfAngle;
	newRotation.z = axis.z * sinHalfAngle;
	newRotation.w = cosf(to_radians(angle) / 2.0f);
	quaternion result = fn_math_quat_mul(newRotation, rotation);
	fn_math_quat_normalize(&result);
	return result;
}

internal inline quaternion fn_math_quat_angle_axis(f32 angle, const vec3f& axis)
{
	quaternion rotation = {};

	f32 sinHalfAngle = sinf(to_radians(angle) / 2.0f);
	rotation.x = axis.x * sinHalfAngle;
	rotation.y = axis.y * sinHalfAngle;
	rotation.z = axis.z * sinHalfAngle;
	rotation.w = cosf(to_radians(angle) / 2.0f);

	fn_math_quat_normalize(&rotation);

	return rotation;
}

// MATRIX SECTION

struct mat4
{
	f32 Data[16];
};

internal inline mat4 fn_math_mat4_identity()
{
	mat4 result = {};
	
	result.Data[0 + 0 * 4] = 1.0f;
	result.Data[1 + 1 * 4] = 1.0f;
	result.Data[2 + 2 * 4] = 1.0f;
	result.Data[3 + 3 * 4] = 1.0f;
	
	return result;
}

internal inline mat4 fn_math_mat4_mul(const mat4& lhs, const mat4& rhs)
{
	mat4 result = {};

	for (int32 row = 0; row < 4; row++)
	{
		for (int32 col = 0; col < 4; col++)
		{
			f32 sum = 0.0f;
			for (int32 e = 0; e < 4; e++)
			{
				sum += lhs.Data[e + row * 4] * rhs.Data[col + e * 4];
			}
			result.Data[col + row * 4] = sum;
		}
	}
	
	return result;
}

internal inline mat4 fn_math_mat4_translation(const vec3f& position)
{
	mat4 translationMatrix = fn_math_mat4_identity();

	translationMatrix.Data[3 + 0 * 4] = position.x;
	translationMatrix.Data[3 + 1 * 4] = position.y;
	translationMatrix.Data[3 + 2 * 4] = position.z;

	return translationMatrix;
}

internal inline mat4 fn_math_mat4_scaling(const vec3f& scale)
{
	mat4 scalingMatrix = fn_math_mat4_identity();

	scalingMatrix.Data[0 + 0 * 4] = scale.x;
	scalingMatrix.Data[1 + 1 * 4] = scale.y;
	scalingMatrix.Data[2 + 2 * 4] = scale.z;

	return scalingMatrix;
}

internal inline mat4 fn_math_mat4_quat_to_rotation(const quaternion& q)
{
	mat4 result = fn_math_mat4_identity();

	// right axis
	result.Data[0 + 0 * 4] = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	result.Data[1 + 0 * 4] = 2.0f * (q.x * q.y - q.w * q.z);
	result.Data[2 + 0 * 4] = 2.0f * (q.x * q.z + q.w * q.y);

	// up axis
	result.Data[0 + 1 * 4] = 2.0f * (q.x * q.y + q.w * q.z);
	result.Data[1 + 1 * 4] = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
	result.Data[2 + 1 * 4] = 2.0f * (q.y * q.z - q.w * q.x);

	// forward axis
	result.Data[0 + 2 * 4] = 2.0f * (q.x * q.z - q.w * q.y);
	result.Data[1 + 2 * 4] = 2.0f * (q.y * q.z + q.w * q.x);
	result.Data[2 + 2 * 4] = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);

	return result;
}

internal inline mat4 fn_math_mat4_local_to_world(const vec3f& position, const quaternion& rotation, const vec3f& scale)
{
	mat4 translation = fn_math_mat4_translation(position);
	mat4 scaling = fn_math_mat4_scaling(scale);
	mat4 rotationMatrix = fn_math_mat4_quat_to_rotation(rotation);

	mat4 result1 = fn_math_mat4_mul(rotationMatrix, scaling);

	return fn_math_mat4_mul(translation, result1);
}

internal inline mat4 fn_math_mat4_camera_view(const vec3f& position, const quaternion& rotation)
{
	mat4 rotationMatrix = fn_math_mat4_quat_to_rotation(rotation);

	vec3f inverse = { };
	inverse.x = -position.x;
	inverse.y = -position.y;
	inverse.z = -position.z;

	mat4 translationMatrix = fn_math_mat4_translation(inverse);

	return fn_math_mat4_mul(rotationMatrix, translationMatrix);
}

internal inline mat4 fn_math_mat4_perspective(f32 fov, f32 aspectRatio, f32 zNear, f32 zFar)
{
	f32 tanHalfFOV = tanf(to_radians(fov) / 2.0f);
	f32 zRange = zFar - zNear;

	mat4 result = {};

	result.Data[0 + 0 * 4] = 1.0f / (aspectRatio * tanHalfFOV);
	result.Data[1 + 1 * 4] = 1.0f / tanHalfFOV;
	result.Data[2 + 2 * 4] = -(zFar + zNear) / zRange;
	result.Data[3 + 2 * 4] = -(2.0f * zFar * zNear) / zRange;
	result.Data[2 + 3 * 4] = -1.0f;

	return result;
}

