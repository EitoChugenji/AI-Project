#pragma once

// =============================================================================
// Collision.h
// 当たり判定だけを担当するファイル（ゲームロジックと分離）
//
// 【使い方の流れ】
// 1. オブジェクトの位置・サイズを Circle または Rect に入れる
// 2. CheckHit〇〇 関数で当たっているか調べる（true / false）
// 3. 当たっていたら、ゲーム側でフラグを立てて処理する
// =============================================================================

// 円形コライダー（動物・アイテムなど）
struct CircleCollider
{
	float centerX;
	float centerY;
	float radius;
};

// 矩形コライダー（障害物・部屋の家具など）
// left/top/right/bottom は部屋内座標（ピクセル）
struct RectCollider
{
	float left;
	float top;
	float right;
	float bottom;
};

// 当たり判定の結果をまとめる構造体（フラグ処理の例）
struct CollisionHitInfo
{
	bool isHit;           // 今フレームで何かと当たったか
	bool circleVsCircle;  // 円同士だったか
	bool rectVsRect;      // 矩形同士だったか
	bool circleVsRect;    // 円と矩形だったか
};

// -----------------------------------------------------------------------------
// 当たり判定関数（純粋な計算だけ。描画やスコアはここでは行わない）
// -----------------------------------------------------------------------------

// 円と円
bool CheckHitCircleCircle(const CircleCollider& circleA, const CircleCollider& circleB);

// 矩形と矩形（AABB：軸に平行な箱同士）
bool CheckHitRectRect(const RectCollider& rectA, const RectCollider& rectB);

// 円と矩形
bool CheckHitCircleRect(const CircleCollider& circle, const RectCollider& rect);

// -----------------------------------------------------------------------------
// 補助：ゲームエンティティからコライダーを作るための便利関数
// （MainScene から呼ぶ想定）
// -----------------------------------------------------------------------------

CircleCollider MakeCircleCollider(float centerX, float centerY, float radius);
RectCollider MakeRectColliderFromCenter(float centerX, float centerY, float halfWidth, float halfHeight);

// 円同士が重なったときに押し返す（位置と速度を直接変更）
void SeparateCircleCircle(
	CircleCollider& circleA,
	CircleCollider& circleB,
	float& velAX,
	float& velAY,
	float& velBX,
	float& velBY);
