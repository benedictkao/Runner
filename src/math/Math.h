#pragma once

#include <cmath>
#include "Geometry.h"
#include "../SDL.h"

namespace math {

	template <typename T>
	constexpr void coerceAtLeast(T& value, const T& min) {
		value = value > min ? value : min;
	}

	template <typename T>
	constexpr void coerceAtMost(T& value, const T& max) {
		value = value < max ? value : max;
	}

	template <typename T>
	constexpr void coercePositive(T& value) noexcept {
		coerceAtLeast(value, static_cast<T>(0));
	}

	template <typename T>
	constexpr void coerceNegative(T& value) noexcept {
		coerceAtMost(value, static_cast<T>(0));
	}

	SDL2::Rect toSDLRect(const Rect2Df&);

	bool rayVsRect(
		const Vector2Df& origin, 
		const Vector2Df& direction, 
		const Rect2Df& target,
		Vector2Df& contactPoint,
		Vector2Df& contactNormal,
		float& contactTime
	);

	bool sweptRectVsRect(
		const Rect2Df& movingRect,
		const Vector2Df& movement,
		const Rect2Df& target,
		Vector2Df& contactPoint,
		Vector2Df& contactNormal,
		float& contactTime
	);

	Vector2Df resolveSweptRectVsRect(
		const Rect2Df& movingRect,
		Vector2Df& movement,
		const Rect2Df& target
	);
}