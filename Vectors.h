#pragma once
#include <math.h>

class Vector2 {
public:
	Vector2(float val);
	Vector2(float _x, float _y);

	float x;
	float y;

	Vector2 operator*(float scalar);

	Vector2 operator/(float scalar);

	Vector2 operator+(Vector2 const& vec);

	Vector2 operator-(Vector2 const& vec);
};

class Vector3 {
public:
	Vector3(float val);
	Vector3(float _x, float _y, float _z);
	Vector3(Vector2 const& vec, float _z);

	float x;
	float y;
	float z;

	Vector3 Norm();

	float Length();

	Vector3 operator*(float scalar);

	Vector3 operator/(float scalar);

	Vector3 operator+(Vector3 const& vec);

	Vector3 operator-(Vector3 const& vec);

	Vector3 operator-();

	float operator*(Vector3 const& vec);
};