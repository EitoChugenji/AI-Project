#pragma once

// シーン遷移用の識別ID定義
enum class SceneID
{
	None,
	Title,
	Main,
	Result,
	Ranking,
	Setting,
	Loading
};

// ゲーム内の各画面（タイトル、メイン等）の共通の初期化・更新・描画ライフサイクルを規定するインターフェース
class SceneBase
{
public:
	virtual ~SceneBase() {}

	// 各シーンの開始時に必要な画像アセットの読込や変数値の初期化を行う
	// 入力: なし
	// 出力: なし
	// 副作用: 各シーンクラスのメンバ変数が初期化される
	virtual void Init() = 0;

	// 各フレームの入力処理およびゲームロジックの更新を行う
	// 入力: なし
	// 出力: なし
	// 副作用: 押下キー等による状態変更など
	virtual void Update() = 0;

	// 各フレームの描画処理を行う
	// 入力: なし
	// 出力: なし
	// 副作用: バックバッファへの描画が行われる
	virtual void Draw() = 0;

	// 次フレームの遷移先シーンIDを取得する
	// 入力: なし
	// 出力: 遷移先シーンのID（遷移しない場合はSceneID::None）
	// 副作用: なし
	virtual SceneID GetNextSceneID() const { return SceneID::None; }
};
