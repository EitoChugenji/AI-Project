#include "Collision.h"

#include <cmath>

// =============================================================================
// 円と円の当たり判定
// 中心間の距離が「半径の合計」以下なら当たっている
// =============================================================================
bool CheckHitCircleCircle(const CircleCollider& circleA, const CircleCollider& circleB)
{
	const float dx = circleB.centerX - circleA.centerX;
	const float dy = circleB.centerY - circleA.centerY;
	const float distSq = dx * dx + dy * dy;
	const float radiusSum = circleA.radius + circleB.radius;
	return distSq <= radiusSum * radiusSum;
}

// =============================================================================
// 矩形と矩形の当たり判定（AABB）
// どの軸でも重なっていなければ「当たっていない」
// =============================================================================
bool CheckHitRectRect(const RectCollider& rectA, const RectCollider& rectB)
{
	if (rectA.right < rectB.left) return false;
	if (rectA.left > rectB.right) return false;
	if (rectA.bottom < rectB.top) return false;
	if (rectA.top > rectB.bottom) return false;
	return true;
}

// =============================================================================
// 円と矩形の当たり判定
// 矩形上で円の中心に一番近い点を求め、そこまでの距離と半径を比べる
// =============================================================================
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

// =============================================================================
// 【参考】円同士がめり込んだときに少し押し返す処理
// MainScene::ResolveEntityCollisions から呼ぶ想定
// =============================================================================
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

	// 位置を半分ずつ押し返す
	circleA.centerX -= nx * overlap * 0.5f;
	circleA.centerY -= ny * overlap * 0.5f;
	circleB.centerX += nx * overlap * 0.5f;
	circleB.centerY += ny * overlap * 0.5f;

	// 速度も法線方向で少し跳ね返す（簡易版）
	const float bounce = 0.5f;
	velAX -= nx * bounce;
	velAY -= ny * bounce;
	velBX += nx * bounce;
	velBY += ny * bounce;
}
