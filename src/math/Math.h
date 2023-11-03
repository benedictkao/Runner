#pragma once

#include <cmath>
#include "../Components.h"
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

	SDL2::Rect toRect(const TransformComponent&);

	bool rayVsRect(
		const Vector2Df& origin, 
		const Vector2Df& direction, 
		const TransformComponent& target,
		Vector2Df& contactPoint,
		Vector2Df& contactNormal,
		float& contactTime
	);

	bool sweptRectVsRect(
		const TransformComponent& movingRect,
		const Vector2Df& direction,
		const TransformComponent& target,
		Vector2Df& contactPoint,
		Vector2Df& contactNormal,
		float& contactTime
	);
}