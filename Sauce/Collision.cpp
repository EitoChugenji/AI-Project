#include "Collision.h"

#include <cmath>

bool CheckHitCircleCircle(const CircleCollider& circleA, const CircleCollider& circleB)
{
	const float dx = circleB.centerX - circleA.centerX;
	const float dy = circleB.centerY - circleA.centerY;
	const float distSq = dx * dx + dy * dy;
	const float radiusSum = circleA.radius + circleB.radius;
	return distSq <= radiusSum * radiusSum;
}

bool CheckHitRectRect(const RectCollider& rectA, const RectCollider& rectB)
{
	if (rectA.right < rectB.left) return false;
	if (rectA.left > rectB.right) return false;
	if (rectA.bottom < rectB.top) return false;
	if (rectA.top > rectB.bottom) return false;
	return true;
}

bool CheckHitCircleRect(const CircleCollider& circle, const RectCollider& rect)
{
	float closestX = circle.centerX;
	float closestY = circle.centerY;

	if (closestX < rect.left)   closestX = rect.left;
	if (closestX > rect.right)  closestX = rect.right;
	if (closestY < rect.top)    closestY = rect.top;
	if (closestY > rect.bottom) closestY = rect.bottom;

	const float dx = circle.centerX - closestX;
	const float dy = circle.centerY - closestY;
	const float distSq = dx * dx + dy * dy;
	return distSq <= circle.radius * circle.radius;
}

CircleCollider MakeCircleCollider(float centerX, float centerY, float radius)
{
	CircleCollider collider = {};
	collider.centerX = centerX;
	collider.centerY = centerY;
	collider.radius = radius;
	return collider;
}

RectCollider MakeRectColliderFromCenter(float centerX, float centerY, float halfWidth, float halfHeight)
{
	RectCollider collider = {};
	collider.left = centerX - halfWidth;
	collider.top = centerY - halfHeight;
	collider.right = centerX + halfWidth;
	collider.bottom = centerY + halfHeight;
	return collider;
}

void SeparateCircleCircle(
	CircleCollider& circleA,
	CircleCollider& circleB,
	float& velAX,
	float& velAY,
	float& velBX,
	float& velBY)
{
	float dx = circleB.centerX - circleA.centerX;
	float dy = circleB.centerY - circleA.centerY;
	float dist = std::sqrt(dx * dx + dy * dy);

	const float radiusSum = circleA.radius + circleB.radius;
	if (dist <= 0.001f)
	{
		dx = 1.0f;
		dy = 0.0f;
		dist = 1.0f;
	}

	if (dist >= radiusSum)
	{
		return;
	}

	const float overlap = radiusSum - dist;
	const float nx = dx / dist;
	const float ny = dy / dist;

	// 重なり量に応じて両オブジェクトの位置を等しく配分して押し戻す
	circleA.centerX -= nx * overlap * 0.5f;
	circleA.centerY -= ny * overlap * 0.5f;
	circleB.centerX += nx * overlap * 0.5f;
	circleB.centerY += ny * overlap * 0.5f;

	// 押し返しベクトル方向の簡易的な反発力を適用
	const float bounce = 0.5f;
	velAX -= nx * bounce;
	velAY -= ny * bounce;
	velBX += nx * bounce;
	velBY += ny * bounce;
}
