#pragma once

namespace math {
	template <class T>
	T max(T a, T b) noexcept {
		return a > b ? a : b;
	}
}