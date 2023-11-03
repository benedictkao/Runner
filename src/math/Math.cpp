#include "Math.h"

#include <utility>

SDL2::Rect math::toRect(const TransformComponent& transform) {
	return { static_cast<int>(transform.pos.x),
		static_cast<int>(transform.pos.y),
		static_cast<int>(transform.size.x),
		static_cast<int>(transform.size.y) };
}

bool math::rayVsRect(
	const Vector2Df& origin,
	const Vector2Df& direction,
	const TransformComponent& target,
	Vector2Df& contactPoint,
	Vector2Df& contactNormal,
	float& contactTime
) {
	// calc contactTime2D and farContactTime2D
	Vector2Df contactTime2D = (target.pos - origin) / direction;
	Vector2Df farContactTime2D = (target.pos + target.size - origin) / direction;

	// 0 divided by 0
	if (std::isnan(contactTime2D.x) || std::isnan(contactTime2D.y) || std::isnan(farContactTime2D.x) || std::isnan(farContactTime2D.y))
		return false;

	// swap to make near and far relative to direction of travel
	if (contactTime2D.x > farContactTime2D.x) std::swap(contactTime2D.x, farContactTime2D.x);
	if (contactTime2D.y > farContactTime2D.y) std::swap(contactTime2D.y, farContactTime2D.y);

	// no collision
	if (contactTime2D.x > farContactTime2D.y || contactTime2D.y > farContactTime2D.x) {
		return false;
	}

	contactTime = std::max(contactTime2D.x, contactTime2D.y);
	float farContactTime = std::min(farContactTime2D.x, farContactTime2D.y);

	// collision happened behind direction, or in same direction but more than distance travelled in single frame
	if (contactTime < 0 || contactTime > 1.0f) {
		return false;
	}

	// calculate contact point
	contactPoint = origin + direction * contactTime;

	// get contact normal
	if (contactTime2D.x < contactTime2D.y) { // cross vertical line before horizontal -> vertical collision
		contactNormal = { 0.0f, direction.y < 0.0f ? 1.0f : -1.0f };
	}
	else { // also covers == condition, which is when we collide exactly at the corner
		contactNormal = { direction.x < 0.0f ? 1.0f : -1.0f, 0.0f };
	}
	return true;
}

bool math::sweptRectVsRect(
	const TransformComponent& movingRect,
	const Vector2Df& direction, 
	const TransformComponent& target,
	Vector2Df& contactPoint, 
	Vector2Df& contactNormal, 
	float& contactTime
) {
	Vector2Df center = { movingRect.pos + movingRect.size / 2 };
	TransformComponent expanded = { target.pos - movingRect.size / 2, target.size + movingRect.size };

	return rayVsRect(center, direction, expanded, contactPoint, contactNormal, contactTime);
}