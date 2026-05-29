#include "Collision.h"

#include <cmath>

/**
 * @brief 円同士の当たり判定を行う
 * @param circleA, circleB 判定対象の円コライダー
 * @return true: 衝突している, false: 衝突していない
 * @note 副作用なし
 */
bool CheckHitCircleCircle(const CircleCollider& circleA, const CircleCollider& circleB)
{
	const float dx = circleB.centerX - circleA.centerX;
	const float dy = circleB.centerY - circleA.centerY;
	const float distSq = dx * dx + dy * dy;
	const float radiusSum = circleA.radius + circleB.radius;

	// 平方根計算(sqrt)は高コストなため、距離の2乗と半径の合計の2乗で比較する
	return distSq <= radiusSum * radiusSum;
}

/**
 * @brief 矩形(AABB)同士の当たり判定を行う
 * @param rectA, rectB 判定対象の矩形コライダー
 * @return true: 衝突している, false: 衝突していない
 * @note 副作用なし
 */
bool CheckHitRectRect(const RectCollider& rectA, const RectCollider& rectB)
{
	// 重ならない条件を早期リターンで弾き、判定の分岐コストを最小化する
	if (rectA.right < rectB.left) return false;
	if (rectA.left > rectB.right) return false;
	if (rectA.bottom < rectB.top) return false;
	if (rectA.top > rectB.bottom) return false;
	return true;
}

/**
 * @brief 円と矩形の当たり判定を行う
 * @param circle 判定対象の円コライダー
 * @param rect 判定対象の矩形コライダー
 * @return true: 衝突している, false: 衝突していない
 * @note 副作用なし
 */
bool CheckHitCircleRect(const CircleCollider& circle, const RectCollider& rect)
{
	float closestX = circle.centerX;
	float closestY = circle.centerY;

	// 矩形内で円の中心に最も近い座標（クランプされた座標）を算出
	if (closestX < rect.left)   closestX = rect.left;
	if (closestX > rect.right)  closestX = rect.right;
	if (closestY < rect.top)    closestY = rect.top;
	if (closestY > rect.bottom) closestY = rect.bottom;

	const float dx = circle.centerX - closestX;
	const float dy = circle.centerY - closestY;
	const float distSq = dx * dx + dy * dy;

	// クランプ座標と円の中心との距離で判定し、sqrt計算を回避
	return distSq <= circle.radius * circle.radius;
}

/**
 * @brief 円コライダー構造体を生成する
 * @param centerX, centerY, radius コライダーのパラメータ
 * @return 初期化済みのCircleCollider
 * @note 副作用なし
 */
CircleCollider MakeCircleCollider(float centerX, float centerY, float radius)
{
	CircleCollider collider = {};
	collider.centerX = centerX;
	collider.centerY = centerY;
	collider.radius = radius;
	return collider;
}

/**
 * @brief 中心座標から矩形コライダー構造体を生成する
 * @param centerX, centerY, halfWidth, halfHeight コライダーのパラメータ
 * @return 初期化済みのRectCollider
 * @note 副作用なし
 */
RectCollider MakeRectColliderFromCenter(float centerX, float centerY, float halfWidth, float halfHeight)
{
	RectCollider collider = {};
	collider.left = centerX - halfWidth;
	collider.top = centerY - halfHeight;
	collider.right = centerX + halfWidth;
	collider.bottom = centerY + halfHeight;
	return collider;
}

/**
 * @brief 重なった2つの円のめり込みを解消し、反発速度を適用する
 * @param circleA, circleB 対象の円コライダー
 * @param velAX, velAY, velBX, velBY 対象の速度ベクトル
 * @note 副作用: 引数のコライダー座標と速度ベクトルの値が直接書き換えられる
 */
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

	// 完全一致によるゼロ除算(NaN発生)を防ぐため、微小距離の場合はダミーの押し出しベクトルを設定
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

	// 両オブジェクトの質量が同じ前提として、重なり量を等分してめり込みを解消する
	circleA.centerX -= nx * overlap * 0.5f;
	circleA.centerY -= ny * overlap * 0.5f;
	circleB.centerX += nx * overlap * 0.5f;
	circleB.centerY += ny * overlap * 0.5f;

	// 押し返しベクトル方向に固定の反発係数を適用して、簡易的な物理挙動をシミュレート
	const float bounce = 0.5f;
	velAX -= nx * bounce;
	velAY -= ny * bounce;
	velBX += nx * bounce;
	velBY += ny * bounce;
}