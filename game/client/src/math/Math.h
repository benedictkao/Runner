#pragma once

#include <common/Geometry.h>
#include "sdl/SDL.h"

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

	SDL2::Rect toSDLRect(const common::Rect2Df&);

	bool rayVsRect(
		const common::Vector2Df& origin, 
		const common::Vector2Df& direction, 
		const common::Rect2Df& target,
		common::Vector2Df& contactPoint,
		common::Vector2Df& contactNormal,
		float& contactTime
	);

	bool sweptRectVsRect(
		const common::Rect2Df& movingRect,
		const common::Vector2Df& arrows,
		const common::Rect2Df& target,
		common::Vector2Df& contactPoint,
		common::Vector2Df& contactNormal,
		float& contactTime
	);

	common::Vector2Df resolveSweptRectVsRect(
		const common::Rect2Df& movingRect,
		common::Vector2Df& arrows,
		const common::Rect2Df& target
	);
}