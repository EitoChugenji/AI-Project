#pragma once

// ゲームロジックと分離し、純粋な幾何学計算のみを行うコライダー及び衝突判定定義

struct CircleCollider
{
	float centerX;
	float centerY;
	float radius;
};

struct RectCollider
{
	float left;
	float top;
	float right;
	float bottom;
};

struct CollisionHitInfo
{
	bool isHit;
	bool circleVsCircle;
	bool rectVsRect;
	bool circleVsRect;
};

// 円同士の衝突判定を行う
// 入力: circleA(円Aの座標と半径), circleB(円Bの座標と半径)
// 出力: 衝突している場合はtrue、それ以外はfalse
// 副作用: なし
bool CheckHitCircleCircle(const CircleCollider& circleA, const CircleCollider& circleB);

// 軸平行境界ボックス(AABB)による矩形同士の衝突判定を行う
// 入力: rectA(矩形Aの範囲), rectB(矩形Bの範囲)
// 出力: 衝突している場合はtrue、それ以外はfalse
// 副作用: なし
bool CheckHitRectRect(const RectCollider& rectA, const RectCollider& rectB);

// 円と矩形の最短距離を基準にした衝突判定を行う
// 入力: circle(円の座標と半径), rect(矩形の範囲)
// 出力: 衝突している場合はtrue、それ以外はfalse
// 副作用: なし
bool CheckHitCircleRect(const CircleCollider& circle, const RectCollider& rect);

// 指定された中心座標と半径から円形コライダーを作成する
// 入力: centerX(中心X座標), centerY(中心Y座標), radius(半径)
// 出力: 作成された円形コライダー構造体
// 副作用: なし
CircleCollider MakeCircleCollider(float centerX, float centerY, float radius);

// 指定された中心座標と半幅・半高から矩形コライダーを作成する
// 入力: centerX(中心X座標), centerY(中心Y座標), halfWidth(横幅の半分), halfHeight(縦幅の半分)
// 出力: 作成された矩形コライダー構造体
// 副作用: なし
RectCollider MakeRectColliderFromCenter(float centerX, float centerY, float halfWidth, float halfHeight);

// 円同士が重なっている場合に位置を押し戻し、簡易的な反射速度を適用する
// 入力: circleA(円Aのコライダー参照), circleB(円Bのコライダー参照), velAX(円AのX速度参照), velAY(円AのY速度参照), velBX(円BのX速度参照), velBY(円BのY速度参照)
// 出力: なし
// 副作用: コライダーの座標および速度値が直接変更される
void SeparateCircleCircle(
	CircleCollider& circleA,
	CircleCollider& circleB,
	float& velAX,
	float& velAY,
	float& velBX,
	float& velBY);
