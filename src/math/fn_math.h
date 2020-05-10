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

internal inline vec3 fn_math_vec3_add(const vec3& lhs, const vec3& rhs)
{
	vec3 result = {};
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;
	return result;
}

internal inline vec3 fn_math_vec3_sub(const vec3& lhs, const vec3& rhs)
{
	vec3 result = {};
	result.x = lhs.x - rhs.x;
	result.y = lhs.y - rhs.y;
	result.z = lhs.z - rhs.z;
	return result;
}

internal inline float fn_math_vec3_dot(const vec3& lhs, const vec3& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

internal inline vec3 fn_math_vec3_cross(const vec3& lhs, const vec3& rhs)
{
	vec3 result = {};
	result.x = lhs.y * rhs.z - lhs.z * rhs.y;
	result.y = lhs.z * rhs.x - lhs.x * rhs.z;
	result.z = lhs.x * rhs.y - lhs.y * rhs.x;
	return result;
}

internal inline vec3 fn_math_vec3_move_in_direction(const vec3& position, const vec3& direction, float scalar)
{
	vec3 result = {};
	result.x = position.x + direction.x * scalar;
	result.y = position.y + direction.y * scalar;
	result.z = position.z + direction.z * scalar;
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

internal inline vec3 fn_math_quat_forward(const quaternion& q)
{
	vec3 forward = {};

	forward.x = 2.0f * (q.x * q.z - q.w * q.y);
	forward.y = 2.0f * (q.y * q.z + q.w * q.x);
	forward.z = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);

	fn_math_vec3_normalize(&forward);

	return forward;
}

internal inline vec3 fn_math_quat_right(const quaternion& q)
{
	vec3 right = {};

	right.x = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	right.y = 2.0f * (q.x * q.y - q.w * q.z);
	right.z = 2.0f * (q.x * q.z + q.w * q.y);

	fn_math_vec3_normalize(&right);

	return right;
}

internal inline vec3 fn_math_quat_up(const quaternion& q)
{
	vec3 up = {};

	up.x = 2.0f * (q.x * q.y + q.w * q.z);
	up.y = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
	up.z = 2.0f * (q.y * q.z - q.w * q.x);

	fn_math_vec3_normalize(&up);

	return up;
}

internal inline quaternion fn_math_quat_rotate(float angle, const vec3& axis, const quaternion& rotation)
{
	quaternion newRotation = {};
	float sinHalfAngle = sinf(TO_RADIANS(angle) / 2.0f);
	newRotation.x = axis.x * sinHalfAngle;
	newRotation.y = axis.y * sinHalfAngle;
	newRotation.z = axis.z * sinHalfAngle;
	newRotation.w = cosf(TO_RADIANS(angle) / 2.0f);
	quaternion result = fn_math_quat_mul(newRotation, rotation);
	fn_math_quat_normalize(&result);
	return result;
}

internal inline quaternion fn_math_quat_angle_axis(float angle, const vec3& axis)
{
	quaternion rotation = {};

	float sinHalfAngle = sinf(TO_RADIANS(angle) / 2.0f);
	rotation.x = axis.x * sinHalfAngle;
	rotation.y = axis.y * sinHalfAngle;
	rotation.z = axis.z * sinHalfAngle;
	rotation.w = cosf(TO_RADIANS(angle) / 2.0f);

	fn_math_quat_normalize(&rotation);

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
			float sum = 0.0f;
			for (int32 e = 0; e < 4; e++)
			{
				sum += lhs.Data[e + row * 4] * rhs.Data[col + e * 4];
			}
			result.Data[col + row * 4] = sum;
		}
	}
	
	return result;
}

internal inline mat4 fn_math_mat4_translation(const vec3& position)
{
	mat4 translationMatrix = fn_math_mat4_identity();

	translationMatrix.Data[3 + 0 * 4] = position.x;
	translationMatrix.Data[3 + 1 * 4] = position.y;
	translationMatrix.Data[3 + 2 * 4] = position.z;

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

internal inline mat4 fn_math_mat4_camera_view(const vec3& position, const quaternion& rotation)
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
	float tanHalfFOV = tanf(TO_RADIANS(fov) / 2.0f);
	float zRange = zFar - zNear;

	mat4 result = {};

	result.Data[0 + 0 * 4] = 1.0f / (aspectRatio * tanHalfFOV);
	result.Data[1 + 1 * 4] = 1.0f / tanHalfFOV;
	result.Data[2 + 2 * 4] = -(zFar + zNear) / zRange;
	result.Data[3 + 2 * 4] = -(2.0f * zFar * zNear) / zRange;
	result.Data[2 + 3 * 4] = -1.0f;

	return result;
}

