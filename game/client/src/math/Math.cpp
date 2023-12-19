#include "Math.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <utility>

#include <logging/Logger.h>

SDL2::Rect math::toSDLRect(const common::Rect2Df& srcRect) {
	return { static_cast<int>(srcRect.pos.x),
		static_cast<int>(srcRect.pos.y),
		static_cast<int>(srcRect.size.x),
		static_cast<int>(srcRect.size.y) };
}

bool math::rayVsRect(
	const common::Vector2Df& origin,
	const common::Vector2Df& direction,
	const common::Rect2Df& target,
	common::Vector2Df& contactPoint,
	common::Vector2Df& contactNormal,
	float& contactTime
) {
	contactPoint = { 0,0 };
	contactNormal = { 0,0 };

	// cache result since division is expensive
	common::Vector2Df invDir = direction.inverse();

	// calc contactTime2D and farContactTime2D
	common::Vector2Df contactTime2D = (target.pos - origin) * invDir;
	common::Vector2Df farContactTime2D = (target.pos + target.size - origin) * invDir;

	// 0 divided by 0
	if (std::isnan(contactTime2D.x) || std::isnan(contactTime2D.y) || std::isnan(farContactTime2D.x) || std::isnan(farContactTime2D.y))
		return false;

	// swap to make near and far relative to direction of travel
	if (contactTime2D.x > farContactTime2D.x) std::swap(contactTime2D.x, farContactTime2D.x);
	if (contactTime2D.y > farContactTime2D.y) std::swap(contactTime2D.y, farContactTime2D.y);

	// no collision
	if (contactTime2D.x >= farContactTime2D.y || contactTime2D.y >= farContactTime2D.x) {
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
	else if (direction.x != 0 && direction.y != 0) { // contact point and normal is at a diagonal
		float cx = direction.x < 0 ? 1.0f : -1.0f * static_cast<float>(M_SQRT2);
		float cy = direction.y < 0 ? 1.0f : -1.0f * static_cast<float>(M_SQRT2);
		contactNormal = { cx, cy };
	}
	else { // contact point is at a diagonal, but moving either up or down only, so there is effectively no collision
		return false;
	}

	return true;
}

bool math::sweptRectVsRect(
	const common::Rect2Df& movingRect,
	const common::Vector2Df& arrows,
	const common::Rect2Df& target,
	common::Vector2Df& contactPoint, 
	common::Vector2Df& contactNormal, 
	float& contactTime
) {
	common::Vector2Df center = { movingRect.pos + movingRect.size / 2 };
	common::Rect2Df expanded = { target.pos - movingRect.size / 2, target.size + movingRect.size };

	//return rayVsRect(center, arrows, expanded, contactPoint, contactNormal, contactTime);

	bool result = rayVsRect(center, arrows, expanded, contactPoint, contactNormal, contactTime);
	return result;
}

common::Vector2Df math::resolveSweptRectVsRect(
	const common::Rect2Df& movingRect, 
	common::Vector2Df& arrows, 
	const common::Rect2Df& target
) {
	common::Vector2Df contactPoint, contactNormal;
	float contactTime;
	bool hasCollision = sweptRectVsRect(movingRect, arrows, target, contactPoint, contactNormal, contactTime);
	if (hasCollision) {
		// take absolute speed since direction should come from contactNormal
		common::Vector2Df speed = { std::abs(arrows.x), std::abs(arrows.y) };
		float adjustmentMagnitude = 1 - contactTime;	// has to be <= 1
		common::Vector2Df veloAdjustment = contactNormal * speed * adjustmentMagnitude;
		arrows += veloAdjustment;
	}
	return contactNormal;
}