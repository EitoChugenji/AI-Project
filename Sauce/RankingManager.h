#pragma once

#include "GameConfig.h"
#include "GameSession.h"
#include "SceneBase.h"

// ランキングに登録する1データ（登録名、スコア、最大コンボ、新規保存ID）のレコード構造体
struct RankingEntry
{
	wchar_t name[RANKING_NAME_MAX_LEN + 1];
	int score;
	int maxCombo;
	unsigned int saveId;
};

// 難易度ごとのランキングデータの保存、読み込み、ソート、および表示対象の状態管理を行うマネージャー
class RankingManager
{
public:
	// 現在の表示対象難易度に対応するランキングファイルを読み込む
	// 入力: なし
	// 出力: なし
	// 副作用: メモリ上のランキングリスト情報がファイルの内容で更新される
	static void Load();

	// 新しいハイスコアレコードを現在の難易度ランキングに挿入保存する
	// 入力: name(登録名文字列), score(スコア), maxCombo(最大コンボ数)
	// 出力: ランキングに新規ランクインした場合はtrue、ランク外の場合はfalse
	// 副作用: ランキングがソートされ、適合した場合はファイル保存が行われ、最新のエントリが強調表示対象になる
	static bool SaveCurrentScore(const wchar_t* name, int score, int maxCombo);

	// 現在のランキングリストのエントリ数を取得する
	// 入力: なし
	// 出力: 有効なエントリ数
	// 副作用: なし
	static int GetEntryCount();

	// 指定されたインデックスのランキングエントリを取得する
	// 入力: index(0からのインデックス)
	// 出力: ランキングレコード構造体の参照
	// 副作用: なし
	static const RankingEntry& GetEntry(int index);

	// 直近で新規追加され、強調表示（ハイライト）すべき行のインデックスを取得する
	// 入力: なし
	// 出力: ハイライト対象インデックス（存在しない場合は-1）
	// 副作用: なし
	static int GetHighlightIndex();

	// ハイライト表示状態を解除する
	// 入力: なし
	// 出力: なし
	// 副作用: ハイライトインデックスが-1にリセットされる
	static void ClearHighlight();

	// 現在の難易度のランキングデータを全消去する
	// 入力: なし
	// 出力: なし
	// 副作用: メモリ上および対応するランキングファイルのデータが初期化される
	static void ClearAll();

	// ランキング画面から戻る際の遷移先画面IDを設定する
	// 入力: sceneID(戻り先の画面ID)
	// 出力: なし
	// 副作用: 戻り先画面のID情報が保存される
	static void SetReturnScene(SceneID sceneID);

	// ランキング画面から戻る際の遷移先画面IDを取得する
	// 入力: なし
	// 出力: 戻り先の画面ID
	// 副作用: なし
	static SceneID GetReturnScene();

	// ランキング画面で表示対象とする難易度を設定する
	// 入力: diff(表示対象の難易度)
	// 出力: なし
	// 副作用: 表示対象の難易度値が変更される
	static void SetViewDifficulty(GameDifficulty diff);

	// 現在ランキング画面で表示している難易度を取得する
	// 入力: なし
	// 出力: 難易度列挙値
	// 副作用: なし
	static GameDifficulty GetViewDifficulty();

private:
	// 指定難易度に対応するランキングファイルパス名を取得する
	// 入力: diff(難易度)
	// 出力: ファイル名文字列ポインタ
	// 副作用: なし
	static const wchar_t* GetFileName(GameDifficulty diff);

	// エントリをスコア（降順）、最大コンボ（降順）の優先順位で並び替える
	// 入力: なし
	// 出力: なし
	// 副作用: メモリ上のランキング配列順序がソートされる
	static void SortEntries();

	// 現在メモリ上にあるランキングデータを対応するファイルに出力する
	// 入力: なし
	// 出力: なし
	// 副作用: 対応するdatファイルが上書き保存される
	static void SaveToFile();

	// 現在の難易度に対応するランキングファイルを読み込む
	// 入力: なし
	// 出力: 読み込み成功時はtrue、ファイル不在または失敗時はfalse
	// 副作用: メモリ上のエントリ情報が上書きされる
	static bool LoadFromFile();

	static RankingEntry s_entries[RANKING_MAX_ENTRIES];
	static int s_entryCount;
	static int s_highlightIndex;
	static unsigned int s_nextSaveId;
	static SceneID s_returnScene;
	static GameDifficulty s_viewDifficulty;
};
