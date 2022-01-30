#include "Vectors.h"

Vector2::Vector2(float val) : x(val), y(val) {}
Vector2::Vector2(float _x, float _y) : x(_x), y(_y) {}

Vector2 Vector2::operator*(float scalar)
{
	return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(float scalar)
{
	return Vector2(x / scalar, y / scalar);
}

Vector2 Vector2::operator+(Vector2 const& vec)
{
	return Vector2(x + vec.x, y + vec.y);
}

Vector2 Vector2::operator-(Vector2 const& vec)
{
	return Vector2(x - vec.x, y - vec.y);
}

Vector3::Vector3(float val) : x(val), y(val), z(val) {}
Vector3::Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
Vector3::Vector3(Vector2 const& vec, float _z) : x(vec.x), y(vec.y), z(_z) {}

Vector3 Vector3::Norm()
{
	return *this / this->Length();
}

float Vector3::Length()
{
	return sqrtf(x * x + y * y + z * z);
}

Vector3 Vector3::operator*(float scalar)
{
	return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar)
{
	return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3 Vector3::operator+(Vector3 const& vec)
{
	return Vector3(x + vec.x, y + vec.y, z + vec.z);
}

Vector3 Vector3::operator-(Vector3 const& vec)
{
	return Vector3(x - vec.x, y - vec.y, z - vec.z);
}

Vector3 Vector3::operator-()
{
	return Vector3(-x, -y, -z);
}

float Vector3::operator*(Vector3 const& vec)
{
	return x * vec.x + y * vec.y + z * vec.z;
}