#include "Math.h"

#include <utility>
#include <iostream>

SDL2::Rect math::toSDLRect(const Rect2Df& srcRect) {
	return { static_cast<int>(srcRect.pos.x),
		static_cast<int>(srcRect.pos.y),
		static_cast<int>(srcRect.size.x),
		static_cast<int>(srcRect.size.y) };
}

bool math::rayVsRect(
	const Vector2Df& origin,
	const Vector2Df& direction,
	const Rect2Df& target,
	Vector2Df& contactPoint,
	Vector2Df& contactNormal,
	float& contactTime
) {
	contactPoint = { 0,0 };
	contactNormal = { 0,0 };

	// cache result since division is expensive
	Vector2Df invDir = direction.inverse();

	// calc contactTime2D and farContactTime2D
	Vector2Df contactTime2D = (target.pos - origin) * invDir;
	Vector2Df farContactTime2D = (target.pos + target.size - origin) * invDir;

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
	if (contactTime < 0 || contactTime >= 1.0f) {
		return false;
	}

	// calculate contact point
	contactPoint = origin + direction * contactTime;

	// get contact normal
	if (contactTime2D.x < contactTime2D.y) { // cross vertical line before horizontal -> vertical collision
		contactNormal = { 0.0f, direction.y < 0.0f ? 1.0f : -1.0f };
	}
	else if (contactTime2D.x > contactTime2D.y) { // cross horizontal line before vertical -> horizontal collision
		contactNormal = { direction.x < 0.0f ? 1.0f : -1.0f, 0.0f };
	}
	// Note if contactTime2D.x == contactTime2D.y, collision is principly in a diagonal
	// so pointless to resolve. By returning contactNormal = {0, 0} even though it is
	// considered a hit, the resolver wont change anything.

	return true;
}

bool math::sweptRectVsRect(
	const Rect2Df& movingRect,
	const Vector2Df& movement,
	const Rect2Df& target,
	Vector2Df& contactPoint, 
	Vector2Df& contactNormal, 
	float& contactTime
) {
	Vector2Df center = { movingRect.pos + movingRect.size / 2 };
	Rect2Df expanded = { target.pos - movingRect.size / 2, target.size + movingRect.size };

	return rayVsRect(center, movement, expanded, contactPoint, contactNormal, contactTime);
}

Vector2Df math::resolveSweptRectVsRect(
	const Rect2Df& movingRect, 
	Vector2Df& movement, 
	const Rect2Df& target
) {
	Vector2Df contactPoint, contactNormal;
	float contactTime;
	bool hasCollision = sweptRectVsRect(movingRect, movement, target, contactPoint, contactNormal, contactTime);
	if (hasCollision) {
		// take absolute speed since direction should come from contactNormal
		Vector2Df speed = { std::abs(movement.x), std::abs(movement.y) };
		float adjustmentMagnitude = 1 - contactTime;	// has to be <= 1
		Vector2Df veloAdjustment = contactNormal * speed * adjustmentMagnitude;
		movement += veloAdjustment;
	}
	return contactNormal;
}