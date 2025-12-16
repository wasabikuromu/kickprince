#pragma once
#include <memory>
#include <vector>
#include "SceneBase.h"

class Stage;
class AllyBase;
class EnemyBase;
class SkyDome;
class Player;
class Item;
class Camera;

//敵スポーン用のデータ
struct TutorialEnemySpawnData
{
	VECTOR pos;   //敵の初期座標
	int type;     //敵の種類
};

struct TutorialFlags
{
	bool isAllyKicked = false;   // 味方を蹴った瞬間
	// 他のチュートリアルフラグもここに追加可能
};

class TutorialScene : public SceneBase
{
public:

	//カメラのモード
	enum class MODE {
		FOLLOW,       //プレイヤー追尾
		ALLY_FOLLOW,  //味方追尾
		SIDE_VIEW     //固定サイドビュー
	};

	//ポーズ時
	enum class PauseState
	{
		PauseMenu,		//通常のポーズメニュー
		ShowControls,	//操作説明画面
		ShowAllies		//アイテム概要画面
	};

	//ステージクリア時
	enum class StageState
	{
		StageMenu,
		NextStage,		//次のステージへ
		SelectStage		//ステージを選択
	};

	//チュートリアル
	enum class TutorialStep {
		STEP_01_MESSAGE,
		STEP_02_MESSAGE,
		STEP_03_MESSAGE,
		STEP_04_MESSAGE,
		STEP_05_MESSAGE,
		STEP_06_MESSAGE,
		STEP_07_MESSAGE,
		STEP_08_MESSAGE,
		STEP_09_MESSAGE,
		STEP_10_MESSAGE,
		WAIT_MOVE,
		STEP_11_MESSAGE,
		STEP_12_MESSAGE,
		STEP_13_MESSAGE,
		WAIT_CAMERA,
		STEP_14_MESSAGE,
		STEP_15_MESSAGE,
		STEP_16_MESSAGE,
		STEP_17_MESSAGE,
		STEP_18_MESSAGE,
		STEP_19_MESSAGE,
		STEP_20_MESSAGE,
		STEP_21_MESSAGE,
		STEP_22_MESSAGE,
		STEP_23_MESSAGE,
		STEP_24_MESSAGE,
		STEP_25_MESSAGE,
		STEP_26_MESSAGE,
		STEP_27_MESSAGE,
		STEP_28_MESSAGE,
		STEP_29_MESSAGE,
		STEP_30_MESSAGE,
		STEP_31_MESSAGE,
		WAIT_ATTACK,
		STEP_32_MESSAGE,
		STEP_33_MESSAGE,
		STEP_34_MESSAGE,
		STEP_35_MESSAGE,
		END
	};

	static constexpr int BOSS_WAIT = 0;		//ボス出現待機
	static constexpr int BOSS_ON = 1;		//ボス出現可能
	static constexpr int BOSS_OFF = 2;		//ボス出現不可

	//敵配置
	static constexpr float X_ENEMY_POS = -90.0f;
	static constexpr float Y_ENEMY_POS = 170.0f;

	static const int MAX_STAGE = 1;			//最終ステージ番号

	//UI関係-----------------------------------------------------
	//-------------------------------------------------------------------

	static constexpr int GAME_HEIGHT_1 = 80;			//ゲーム開始時の注意書き

	//画像サイズ
	static constexpr float IMG_GAME_UI_1_SIZE = 0.5;	//imgGameUi1_のサイズ
	static constexpr float IMG_OPEGEAR_UI_SIZE = 0.8;	//imgOpeGear_のサイズ
	static constexpr float PAUSE_IMG_UI_SIZE = 0.65;	//pauseImg_のサイズ

	//ポーズメニュー関連
	static constexpr int PAUSE_MENU_ITEM_COUNT = 5;						//ポーズメニューの数
	static constexpr int PAUSE_MENU_DOWN = 1;							//下に移動
	static constexpr int PAUSE_MENU_UP = PAUSE_MENU_ITEM_COUNT - 1;		//上に移動（+3 の代わり）

	//フェード系
	static constexpr int AUTO_FADE = 240;				//自動フェード
	static constexpr int FLASH = 45;					//点滅
	static constexpr int ONE_SECOND_FRAME = 60;			//1秒

	//ステージクリア
	static constexpr float BLINK_SPEED = 0.003f;
	static constexpr int CURSOR_WIDTH = 1290;
	static constexpr int CURSOR_HEIGHT = 765;
	static constexpr int INDEX = 100;

	//設定系
	static constexpr int UI_GEAR = 100;					//imgOpeGear_のX,Yの場所

	static constexpr int UI_PAUSE_IMG_HEIGHT = 150;				//pauseImg_の高さ

	static constexpr int UI_WIDTH_PAUSE_1 = 160;				//UIを調整する
	static constexpr int UI_WIDTH_PAUSE_2 = 200;				//UIを調整する
	static constexpr int UI_WIDTH_PAUSE_3 = 240;				//UIを調整する

	static constexpr int UI_HEIGHT_PAUSE_1 = 350;				//１個目のUIの高さ
	static constexpr int UI_HEIGHT_PAUSE_2 = 470;				//２個目のUIの高さ
	static constexpr int UI_HEIGHT_PAUSE_3 = 590;				//３個目のUIの高さ
	static constexpr int UI_HEIGHT_PAUSE_4 = 710;				//４個目のUIの高さ

	static constexpr int UI_ATTACK_X = 10;						//攻撃の文字のX座標
	static constexpr int UI_NORMAL_ATTACK_Y = 450;				//通常攻撃のY座標
	static constexpr int UI_SLASH_ATTACK_Y = 500;				//スラッシュのY座標
	static constexpr int UI_EX_ATTACK_Y = 550;					//回転斬りのY座標

	static constexpr int BACK_PAUSE_WIDTH = 1600;				//ポーズに戻るときのENTERのX
	static constexpr int BACK_PAUSE_HEIGHT = 1020;				//ポーズに戻るときのENTERのY

	//-------------------------------------------------------------------

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int red = 0xff0000;	  //赤
	int green = 0x00ff00; //緑
	int blue = 0x0000ff;  //青
	int yellow = 0xffff00;//黄
	int purpl = 0x800080; //紫

	TutorialScene(int stageNo = 1);	// コンストラクタ
	~TutorialScene(void);	// デストラクタ

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	void AddItem(std::shared_ptr<Item> item);
	const std::vector<std::shared_ptr<AllyBase>>& GetEnemies() const;	//enemyの情報(pos)を見る

	void NotifyAllyKicked(void);

	void OnAllyKicked(void);

	void OnAllyKicked(AllyBase* kickedAlly);
	void ReturnToPlayerCamera(void);

	bool IsAnyAllyFlying(void) const;
	AllyBase* GetFlyingAlly(void) const;

	bool IsAllEnemiesDefeated(void) const;

	void RunTutorial(void);

private:
	int cnt;

	//味方生成
	void AllyCreate(void);

	//敵生成
	void EnemyCreate(void);

	//ポーズメニュー
	bool PauseMenu(void);

	//敵スポーン関数
	void SpawnEnemies(int stageNo);

	std::unique_ptr<Stage> stage_;		// ステージ
	std::unique_ptr<SkyDome> skyDome_;	// スカイドーム
	std::shared_ptr<Player> player_;	// プレイヤー
	std::vector<std::shared_ptr<Item>> items_;		//アイテム
	std::shared_ptr<Camera> camera_;	//カメラ

	int enemyModelId_;
	int uiDisplayFrame_;	//カウンタ

	bool uiFadeStart_ = false;
	int uiFadeFrame_ = 0;

	//-1なら追尾していない状態
	int followAllyIndex_ = -1;

	// 設定開く
	int imgOpeGearKey_;
	int imgOpeGearCon_;

	std::vector<std::shared_ptr<AllyBase>> Allys_;
	std::vector<std::shared_ptr<EnemyBase>> enemys_;
	int enCounter;//敵の出現頻度

	float allyLandTimer_ = 0.0f;
	bool isKicking_ = false;           //プレイヤーが味方を蹴っている状態
	AllyBase* currentKickedAlly_ = nullptr; //飛んでいる味方のポインタ

	int isB_;

	//ポーズ関連
	PauseState pauseState_ = PauseState::PauseMenu;
	int  pauseImg_;					//ポーズ画像

	int pauseMenuImgs[PAUSE_MENU_ITEM_COUNT];				//通常の画像
	int pauseMenuImgsSelected[PAUSE_MENU_ITEM_COUNT];		//選択中の画像

	int pauseMenuPosY[PAUSE_MENU_ITEM_COUNT];				//Y座標位置

	bool isPaused_;					//ポーズ中かどうか
	int pauseSelectIndex_;			//ポーズメニューの選択項目（上下選択）
	int pauseExplainImgs_[3];

	int imgAbutton_;
	int imgEnter_;

	int stageNo_;   // ← ステージ番号

	// 敵配置テーブル
	std::map<int, std::vector<TutorialEnemySpawnData>> enemySpawnTable_;

	//チュートリアル
	TutorialStep step_;

	bool showMessage_ = true;

	bool isTutorialPlayerMove_ = false;
	int imgPlayerMove_;		//プレイヤー移動説明画像

	bool isTutorialCameraMove_ = false;
	int imgCameraMove_;		//カメラ移動説明画像

	bool isTutorialAttack_ = false;
	int imgAttack_;			//攻撃画像


	VECTOR moveStartPos_;
	float cameraStartRotX_;
	float cameraStartRotY_;
	TutorialFlags tutorialFlags_;
	bool isTutorialMsgActive_ = false;   //メッセージ表示中は操作禁止
	bool waitAttackReady_ = false;

	int currentStep_ = 1;  
	int imgCurrentMessage_ = -1;
	int imgTutorialMsg_[36];

	//チュートリアル系画像
	int imgTutorialTextBG_;			//チュートリアルのテキスト背景
};
