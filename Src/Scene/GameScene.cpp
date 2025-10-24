#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/GravityManager.h"
#include "../Manager/ResourceManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/SkyDome.h"
#include "../Object/Stage.h"
#include "../Object/Player.h"
#include "../Object/AllyBase.h"
#include "../Object/EnemyBase.h"
#include "../Object/Enemy/EnemyBoss.h"
#include "../Object/Enemy/EnemyCactus.h"
#include "../Object/Enemy/EnemyDog.h"
#include "../Object/Enemy/EnemyMimic.h"
#include "../Object/Planet.h"
#include "GameScene.h"

GameScene::GameScene(void)
{
	player_ = nullptr;
	skyDome_ = nullptr;
	stage_ = nullptr;
	imgGameUi1_ = -1;
	imgOpeGear_ = -1;
	//item_ = nullptr;
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	cnt = 0;
	//プレイヤー
	player_ = std::make_shared<Player>();
	GravityManager::GetInstance().SetPlayer(player_);
	player_->Init();

	//敵
	BossCreate();

	//味方のモデル
	AllyCreate();

	player_->SetEnemy(&Allys_);

	//ステージ
	stage_ = std::make_unique<Stage>(*player_);
	stage_->Init();

	//ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	//スカイドーム
	skyDome_ = std::make_unique<SkyDome>(player_->GetTransform());
	skyDome_->Init();

	//画像
	imgGameUi1_ = resMng_.Load(ResourceManager::SRC::GAMEUI_1).handleId_;
	imgOpeGear_ = resMng_.Load(ResourceManager::SRC::OPE_GEAR).handleId_;

	pauseImg_ = LoadGraph("Data/Image/pause.png");

	pauseExplainImgs_[0] = resMng_.Load(ResourceManager::SRC::PAUSEOPE).handleId_; // 操作説明
	pauseExplainImgs_[1] = resMng_.Load(ResourceManager::SRC::PAUSEITEM).handleId_;   // アイテム概要

	//カウンタ
	uiFadeStart_ = false;
	uiFadeFrame_ = 0;
	uiDisplayFrame_ = 0;

	//ポーズ
	isPaused_ = false;
	pauseSelectIndex_ = 0;

	//カメラのポーズ解除
	camera_ = SceneManager::GetInstance().GetCamera().lock();
	if (camera_) {
		camera_->SetPaused(false); // ← ここが重要！

		//ミニマップ用カメラ
		camera_->SetFollow(&player_->GetTransform());
		camera_->ChangeMode(Camera::MODE::FOLLOW);
	}

	//音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::GAME_BGM, Sound::TIMES::LOOP);

	mainCamera->SetFollow(&player_->GetTransform());
	mainCamera->ChangeMode(Camera::MODE::FOLLOW);

	isB_ = BOSS_WAIT;
}

void GameScene::Update(void)
{
	cnt++;
	InputManager& ins = InputManager::GetInstance();

	if (PauseMenu()) return;

	//-------------------------
	// カメラモード管理
	//-------------------------

	static Camera::MODE cameraMode = Camera::MODE::FOLLOW;

	// ---- 右入力で順送り ----
	if (ins.IsTrgDown(KEY_INPUT_R) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_RIGHT))
	{
		switch (cameraMode)
		{
		case Camera::MODE::FOLLOW:
			cameraMode = Camera::MODE::SIDE_VIEW;
			mainCamera->ChangeMode(Camera::MODE::SIDE_VIEW);
			player_->SetControlEnabled(false);
			break;

		case Camera::MODE::SIDE_VIEW:
			cameraMode = Camera::MODE::ALLY_FOLLOW;
			mainCamera->SetFollow(&Allys_[0]->GetTransform());
			mainCamera->ChangeMode(Camera::MODE::ALLY_FOLLOW);
			player_->SetControlEnabled(false);
			break;

		case Camera::MODE::ALLY_FOLLOW:
			cameraMode = Camera::MODE::FOLLOW;
			mainCamera->SetFollow(&player_->GetTransform());
			mainCamera->ChangeMode(Camera::MODE::FOLLOW);
			player_->SetControlEnabled(true);
			break;
		}
	}

	// ---- 左入力で逆送り ----
	if (ins.IsTrgDown(KEY_INPUT_T) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_LEFT))
	{
		switch (cameraMode)
		{
		case Camera::MODE::FOLLOW:
			cameraMode = Camera::MODE::ALLY_FOLLOW;
			mainCamera->SetFollow(&Allys_[0]->GetTransform());
			mainCamera->ChangeMode(Camera::MODE::ALLY_FOLLOW);
			player_->SetControlEnabled(false);
			break;

		case Camera::MODE::ALLY_FOLLOW:
			cameraMode = Camera::MODE::SIDE_VIEW;
			mainCamera->ChangeMode(Camera::MODE::SIDE_VIEW);
			player_->SetControlEnabled(false);
			break;

		case Camera::MODE::SIDE_VIEW:
			cameraMode = Camera::MODE::FOLLOW;
			mainCamera->SetFollow(&player_->GetTransform());
			mainCamera->ChangeMode(Camera::MODE::FOLLOW);
			player_->SetControlEnabled(true);
			break;
		}
	}

	//-------------------------
	// 通常の更新処理
	//-------------------------

	uiDisplayFrame_++;

	skyDome_->Update();
	stage_->Update();
	player_->Update();

	for (auto& ally : Allys_) {
		if (!ally) continue;
		ally->Update();
	}

	for (auto& enemy : enemys_) {
		if (!enemy) continue;
		enemy->Update();
	}
}

void GameScene::Draw(void)
{
	skyDome_->Draw();
	stage_->Draw();
	for (auto ally : Allys_)
	{
		ally->Draw();
	}

	for (auto enemy : enemys_)
	{
		enemy->Draw();
	}

	player_->Draw();

	for (auto enemy : enemys_)
	{
		enemy->DrawBossHpBar();
	}
	DrawRotaGraph(UI_GEAR, UI_GEAR, IMG_OPEGEAR_UI_SIZE, 0.0, imgOpeGear_, true);

	//入力チェック or 時間経過でフェード開始
	if (!uiFadeStart_) 
	{
		if ((CheckHitKey(KEY_INPUT_W)
			|| CheckHitKey(KEY_INPUT_A)
			|| CheckHitKey(KEY_INPUT_S)
			|| CheckHitKey(KEY_INPUT_D))
			|| uiDisplayFrame_ >= AUTO_FADE)  // 時間経過による自動フェード
		{
			uiFadeStart_ = true;
			uiFadeFrame_ = 0;
		}
	}
	if (!uiFadeStart_) 
	{
		// フェード前（通常表示）
		DrawRotaGraph((Application::SCREEN_SIZE_X / 2), GAME_HEIGHT_1, IMG_GAME_UI_1_SIZE, 0, imgGameUi1_, true);
	}
	else if (uiFadeFrame_ < ONE_SECOND_FRAME)
	{
		// フェード中（60フレームで徐々に消す）
		int alpha = static_cast<int>(255 * (ONE_SECOND_FRAME - uiFadeFrame_) / ONE_SECOND_FRAME);
		DrawRotaGraph((Application::SCREEN_SIZE_X / 2), GAME_HEIGHT_1, IMG_GAME_UI_1_SIZE, 0, imgGameUi1_, true);
		uiFadeFrame_++;
	}

	if (isPaused_) 
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), black, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		if (pauseState_ == PauseState::Menu)
		{
			DrawRotaGraph((Application::SCREEN_SIZE_X / 2), UI_PAUSE_IMG_HEIGHT, PAUSE_IMG_UI_SIZE, 0, pauseImg_, true);
			SetFontSize(DEFAULT_FONT_SIZE * 5.0);

			DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_1, "ゲームに戻る", white);
			if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == 0)
				DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_1, "ゲームに戻る", yellow);

			DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_1, UI_HEIGHT_PAUSE_2, "操作説明", white);
			if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == 1)
				DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_1, UI_HEIGHT_PAUSE_2, "操作説明", yellow);

			DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_3, "アイテム概要", white);
			if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == 2)
				DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_3, "アイテム概要", yellow);

			DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_2, UI_HEIGHT_PAUSE_4, "タイトルへ", white);
			if (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == 3)
				DrawString((Application::SCREEN_SIZE_X / 2) - UI_WIDTH_PAUSE_2, UI_HEIGHT_PAUSE_4, "タイトルへ", yellow);

			SetFontSize(DEFAULT_FONT_SIZE);
		}
		else if (pauseState_ == PauseState::ShowControls) 
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
			DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), white, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			DrawGraph(0, 0, pauseExplainImgs_[0], true);
			SetFontSize(DEFAULT_FONT_SIZE * 2.5);
			DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT,"Enterキーで戻る", yellow);
			if (cnt % FLASH * 2.0 <= FLASH)DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", white);
			SetFontSize(DEFAULT_FONT_SIZE);
		}
		else if (pauseState_ == PauseState::ShowItems) 
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
			DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), white, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			DrawGraph(0, 0, pauseExplainImgs_[1], true);
			SetFontSize(DEFAULT_FONT_SIZE * 2.5);
			DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", yellow);
			if (cnt % FLASH * 2.0 <= FLASH)DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", white);
			SetFontSize(DEFAULT_FONT_SIZE);
		}
		return;
	}
#pragma region UI
	SetFontSize(DEFAULT_FONT_SIZE * 2.0);
	DrawString(UI_ATTACK_X,UI_NORMAL_ATTACK_Y,"E:通常攻撃" , white);
	SetFontSize(DEFAULT_FONT_SIZE);
#pragma endregion
}

void GameScene::Release(void)
{
	SoundManager::GetInstance().Stop(SoundManager::SRC::GAME_BGM);
}

void GameScene::AddItem(std::shared_ptr<Item> item)
{
	items_.push_back(item);
}

const std::vector<std::shared_ptr<AllyBase>>& GameScene::GetEnemies() const
{
	return Allys_;
}

void GameScene::AllyCreate(void)
{
	Allys_.clear(); // 念のためクリア

	// プレイヤー位置と回転を取得
	VECTOR basePos = player_->GetTransform().pos;
	float playerAngleY = player_->GetTransform().rot.y; // Y軸回転（ラジアン）

	// 前方・右方向ベクトル
	VECTOR forward = VGet(sinf(playerAngleY), 0.0f, cosf(playerAngleY));
	VECTOR right = VGet(cosf(playerAngleY), 0.0f, -sinf(playerAngleY));

	// プレイヤーの前方に出す距離と横間隔
	const float forwardDist = 300.0f; // 前方方向へ距離
	const float spacing = 200.0f;     // 横間隔

	// 3種類の味方を左から右へ並べる
	struct EnemySpawnData {
		AllyBase::TYPE type;
		float horizontalOffset; // 横方向の位置補正
	};

	std::vector<EnemySpawnData> spawnList = {
		{ AllyBase::TYPE::RED,   -spacing }, // 左
		{ AllyBase::TYPE::BLUE,   0.0f     }, // 中央
		{ AllyBase::TYPE::BLACK,  spacing  }  // 右
	};

	for (auto& data : spawnList)
	{
		std::shared_ptr<AllyBase> enemy;

		// 種類ごとに生成
		switch (data.type)
		{
		case AllyBase::TYPE::RED:
			enemy = std::make_shared<AllyRed>();
			break;
		case AllyBase::TYPE::BLUE:
			enemy = std::make_shared<AllyBule>();
			break;
		case AllyBase::TYPE::BLACK:
			enemy = std::make_shared<AllyBlack>();
			break;
		default:
			continue;
		}

		// 配置座標を計算
		VECTOR offset = VAdd(
			VScale(forward, forwardDist),                 // 前方向へ
			VScale(right, data.horizontalOffset)          // 横へ並べる
		);
		VECTOR spawnPos = VAdd(basePos, offset);

		// 基本設定
		enemy->SetGameScene(this);
		enemy->SetPos(spawnPos);
		enemy->SetPlayer(player_);
		enemy->SetEnemy(&enemys_);
		enemy->Init();

		Allys_.emplace_back(std::move(enemy));
	}
}

void GameScene::BossCreate(void)
{
	std::shared_ptr<EnemyBase> boss = std::make_shared<EnemyBoss>();
	boss->SetGameScene(this);
	boss->SetPos(VGet(0, 200, 3500));
	boss->SetPlayer(player_);
	boss->SetAlly(&Allys_);
	boss->Init();

	enemys_.emplace_back(std::move(boss));
}

bool GameScene::PauseMenu(void)
{
	InputManager& ins = InputManager::GetInstance();

	//TABキーでポーズのON/OFF切り替え（Menu中のみ）
	if (ins.IsTrgDown(KEY_INPUT_TAB)|| 
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::MENU)
		&& pauseState_ == PauseState::Menu)
	{
		isPaused_ = !isPaused_;
		pauseSelectIndex_ = 0;
		mainCamera->SetPaused(isPaused_);
		return true;
	}

	if (!isPaused_) return false; // ポーズ中でなければ通常更新

	if (pauseState_ == PauseState::Menu)
	{
		if (ins.IsTrgDown(KEY_INPUT_DOWN)||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_DOWN))
			pauseSelectIndex_ = (pauseSelectIndex_ + PAUSE_MENU_DOWN) % PAUSE_MENU_ITEM_COUNT;

		if (ins.IsTrgDown(KEY_INPUT_UP)||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_TOP))
			pauseSelectIndex_ = (pauseSelectIndex_ + PAUSE_MENU_UP) % PAUSE_MENU_ITEM_COUNT;

		if (ins.IsTrgDown(KEY_INPUT_RETURN)||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
		{
			switch (pauseSelectIndex_)
			{
			case 0:
				isPaused_ = false;
				mainCamera->SetPaused(false);
				break;
			case 1: pauseState_ = PauseState::ShowControls; break;
			case 2: pauseState_ = PauseState::ShowItems; break;
			case 3: SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE); break;
			}
		}
	}
	else
	{
		if (ins.IsTrgDown(KEY_INPUT_RETURN)||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
			pauseState_ = PauseState::Menu;
	}

	return true;
}