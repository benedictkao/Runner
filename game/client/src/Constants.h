#pragma once

#include <stdint.h>

namespace constants {
	constexpr float WINDOW_WIDTH{ 576.0f * 2 };
	constexpr float WINDOW_HEIGHT{ 324.0f * 2 };

	constexpr int FPS_FACTOR{ 3 };	// change this to reduce overall frame rate
	constexpr int FPS_FACTOR_SQ{ FPS_FACTOR * FPS_FACTOR };
	constexpr int BASE_FPS_MULTIPLICAND{ 15 };

	constexpr int TARGET_FPS{ FPS_FACTOR * BASE_FPS_MULTIPLICAND };
	constexpr auto MILLIS_PER_FRAME{ 1000 / TARGET_FPS };

	constexpr int ANIMATION_FRAME_PERIOD{ 1 * FPS_FACTOR };

	// acceleration
	constexpr float GRAVITY_ACCEL{ 32.0f / FPS_FACTOR_SQ };

	// velocities
	constexpr float TERMINAL_DROP_VELO{ 80.0f / FPS_FACTOR };
	constexpr float BASE_PLAYER_MOVE_SPEED{ 24.0f / FPS_FACTOR };
	constexpr float BASE_PLAYER_JUMP_SPEED{ 96.0f / FPS_FACTOR };
}