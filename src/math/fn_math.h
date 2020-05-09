#pragma once

#include <math.h>

#define PI 3.14159265359f
#define TO_RADIANS(angle)(angle * (PI / 180.0f))
#define TO_DEGREES(radians)(radians * (180.0f / PI))

// VECTOR SECTION
struct vec2
{
	float x, y;
};

struct vec3
{
	float x, y, z;
};

struct vec4
{
	float x, y, z, w;
};

internal inline float fn_math_vec3_length(const vec3& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

internal inline void fn_math_vec3_normalize(vec3* v)
{
	float len = fn_math_vec3_length(*v);
	v->x = v->x / len;
	v->y = v->y / len;
	v->z = v->z / len;
}

internal inline vec3 fn_math_vec3_add(const vec3& v1, const vec3& v2)
{
	vec3 result = {};
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

internal inline vec3 fn_math_vec3_sub(const vec3& v1, const vec3& v2)
{
	vec3 result = {};
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

internal inline float fn_math_vec3_dot(const vec3& v1, const vec3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

internal inline vec3 fn_math_vec3_cross(const vec3& v1, const vec3& v2)
{
	vec3 result = {};
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

// QUATERNION SECTION

struct quaternion
{
	float x, y, z, w;
};

internal inline float fn_math_quat_length(const quaternion& q)
{
	return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

internal inline void fn_math_quat_normalize(quaternion* q)
{
	float len = fn_math_quat_length(*q);
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

internal inline quaternion fn_math_quat_mul(const quaternion& q1, const quaternion& q2)
{
	quaternion result = {};
	result.x = q1.x * q2.w + q1.w * q2.x + q1.y * q2.z - q1.z * q2.y;
	result.y = q1.y * q2.w + q1.w * q2.y + q1.z * q2.x - q1.x * q2.z;
	result.z = q1.z * q2.w + q1.w * q2.z + q1.x * q2.y - q1.y * q2.x;
	result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	fn_math_quat_normalize(&result);
	return result;
}

internal inline quaternion fn_math_quat_mul_vec3(const quaternion& q, const vec3& v)
{
	quaternion result = {};
	result.x = q.w * v.x + q.y * v.z - q.z * v.y;
	result.y = q.w * v.y + q.z * v.x - q.x * v.z;
	result.z = q.w * v.z + q.x * v.y - q.y * v.x;
	result.w = -q.x * v.x - q.y * v.y - q.z * v.z;
	fn_math_quat_normalize(&result);
	return result;
}

internal inline vec3 fn_math_quat_forward(const quaternion& rotation)
{
	vec3 zAxis = { 0.0f, 0.0f, 1.0f };
	quaternion conjugate = fn_math_quat_conjugate(rotation);
	quaternion q = fn_math_quat_mul(fn_math_quat_mul_vec3(rotation, zAxis), conjugate);
	vec3 forward = {};
	forward.x = q.x;
	forward.y = q.y;
	forward.z = q.z;
}

internal inline void fn_math_quat_rotate(float angle, const vec3& axis, quaternion* rotation)
{
	float sinHalfAngle = sinf(TO_RADIANS(angle) / 2.0f);
	rotation->x = axis.x * sinHalfAngle;
	rotation->y = axis.y * sinHalfAngle;
	rotation->z = axis.z * sinHalfAngle;
	rotation->w = cosf(TO_RADIANS(angle) / 2.0f);
}

internal inline quaternion fn_math_quat_angle_axis(float angle, const vec3& axis)
{
	quaternion rotation = {};

	float sinHalfAngle = sinf(TO_RADIANS(angle) / 2.0f);
	rotation.x = axis.x * sinHalfAngle;
	rotation.y = axis.y * sinHalfAngle;
	rotation.z = axis.z * sinHalfAngle;
	rotation.w = cosf(TO_RADIANS(angle) / 2.0f);

	return rotation;
}

// MATRIX SECTION

struct mat4
{
	float Data[16];
};

internal inline mat4 fn_math_mat4_identity()
{
	mat4 result = {};

	// @TODO(Anders): Check if matrix data array gets initalized and if it does then remove memset 
	memset(result.Data, 0, sizeof(float) * 16);
	
	result.Data[0 + 0 * 4] = 1.0f;
	result.Data[1 + 1 * 4] = 1.0f;
	result.Data[2 + 2 * 4] = 1.0f;
	result.Data[3 + 3 * 4] = 1.0f;
	
	return result;
}

internal inline mat4 fn_math_mat4_mul(const mat4& m1, const mat4& m2)
{
	mat4 result = {};
	memset(result.Data, 0, sizeof(float) * 16);

	for (uint32 i = 0; i < 16; i += 4)
	{
		for (uint32 j = 0; j < 4; j++)
		{
			result.Data[i + j] = 
				  (m2.Data[i + 0] * m1.Data[j + 0])
				+ (m2.Data[i + 1] * m1.Data[j + 4])
				+ (m2.Data[i + 2] * m1.Data[j + 8])
				+ (m2.Data[i + 3] * m1.Data[j + 12]);
		}
	}

	return result;
}

internal inline mat4 fn_math_mat4_translation(const vec3& position)
{
	mat4 translationMatrix = fn_math_mat4_identity();

	translationMatrix.Data[0 + 3 * 4] = position.x;
	translationMatrix.Data[1 + 3 * 4] = position.y;
	translationMatrix.Data[2 + 3 * 4] = position.z;

	return translationMatrix;
}

internal inline mat4 fn_math_mat4_scaling(const vec3& scale)
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

internal inline mat4 fn_math_mat4_local_to_world(const vec3& position, const quaternion& rotation, const vec3& scale)
{
	mat4 translation = fn_math_mat4_translation(position);
	mat4 scaling = fn_math_mat4_scaling(scale);
	mat4 rotationMatrix = fn_math_mat4_quat_to_rotation(rotation);

	mat4 result1 = fn_math_mat4_mul(rotationMatrix, scaling);

	return fn_math_mat4_mul(translation, result1);
}

internal inline mat4 fn_math_mat4_look_at(const vec3& position, const quaternion& rotation)
{
	mat4 rotationMatrix = fn_math_mat4_quat_to_rotation(fn_math_quat_conjugate(rotation));

	vec3 inverse = { };
	inverse.x = -position.x;
	inverse.y = -position.y;
	inverse.z = -position.z;

	mat4 translationMatrix = fn_math_mat4_translation(inverse);

	return fn_math_mat4_mul(rotationMatrix, translationMatrix);
}

internal inline mat4 fn_math_mat4_perspective(float fov, float aspectRatio, float zNear, float zFar)
{
	float tanHalfFOV = tanf(fov / 2.0f);
	float zRange = zFar - zNear;

	mat4 result = {};
	memset(result.Data, 0, sizeof(float) * 16);

	result.Data[0 + 0 * 4] = 1.0f / (aspectRatio * tanHalfFOV);
	result.Data[1 + 1 * 4] = 1.0f / tanHalfFOV;
	result.Data[2 + 2 * 4] = -(zFar + zNear) / zRange;
	result.Data[3 + 2 * 4] = (-2.0f * zFar * zNear) / zRange;
	result.Data[2 + 3 * 4] = -1;

	return result;
}

