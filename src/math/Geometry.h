#pragma once

struct Vector2D {
	int x;
	int y;

	Vector2D operator+(const Vector2D& v) const;
	Vector2D operator-(const Vector2D& v) const;
	Vector2D operator*(const Vector2D& v) const;
	Vector2D operator/(const Vector2D& v) const;
};

struct Vector2Df {
	float x;
	float y;

	Vector2Df operator+(const Vector2Df& v) const;
	Vector2Df operator-(const Vector2Df& v) const;
	Vector2Df operator*(const Vector2Df& v) const;
	Vector2Df operator/(const Vector2Df& v) const;

	Vector2Df operator*(float f) const;
	Vector2Df operator/(float f) const;

	Vector2Df& operator+=(const Vector2Df& v);

	Vector2Df inverse() const;
};

struct Padding {
	int left;
	int right;
	int top;
	int bottom;
};

struct Rect2Df {
	Vector2Df pos;
	Vector2Df size;
};