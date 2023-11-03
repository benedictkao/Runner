#include "Geometry.h"

Vector2D Vector2D::operator+(const Vector2D& v) const {
	return { x + v.x, y + v.y };
}

Vector2D Vector2D::operator-(const Vector2D& v) const {
	return { x - v.x, y - v.y };
}

Vector2D Vector2D::operator*(const Vector2D& v) const {
	return { x * v.x, y * v.y };
}

Vector2D Vector2D::operator/(const Vector2D& v) const {
	return { x / v.x, y / v.y };
}

Vector2Df Vector2Df::operator+(const Vector2Df& v) const {
	return { x + v.x, y + v.y };
}

Vector2Df Vector2Df::operator-(const Vector2Df& v) const {
	return { x - v.x, y - v.y };
}

Vector2Df Vector2Df::operator*(const Vector2Df& v) const {
	return { x * v.x, y * v.y };
}

Vector2Df Vector2Df::operator/(const Vector2Df& v) const {
	return { x / v.x, y / v.y };
}

Vector2Df Vector2Df::operator*(float f) const {
	return { x * f, y * f };
}

Vector2Df Vector2Df::operator/(float f) const {
	return { x / f, y / f };
}

Vector2Df& Vector2Df::operator+=(const Vector2Df& v) {
	x += v.x;
	y += v.y;
	return *this;
}

Vector2Df Vector2Df::inverse() const {
	return { 1.0f / x, 1.0f / y };
}
