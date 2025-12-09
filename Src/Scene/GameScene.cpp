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
#include "../Object/Enemy/EnemyDog.h"
#include "../Object/Enemy/EnemyOnion.h"
#include "../Object/Enemy/EnemyVirus.h"
#include "../Object/Enemy/EnemyBoss.h"
#include "../Object/Ally/AllyRed.h"
#include "../Object/Ally/AllyBlue.h"
#include "../Object/Ally/AllyBlack.h"
#include "../Object/Planet.h"
#include "GameScene.h"

static Camera::MODE cameraMode = Camera::MODE::FOLLOW;

GameScene::GameScene(int stageNo)
	: stageNo_(stageNo)
{
	player_ = nullptr;
	skyDome_ = nullptr;
	stage_ = nullptr;
	imgOpeGear_ = -1;
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
	EnemyCreate();

	//ステージごとの敵配置
	SpawnEnemies(stageNo_);

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
	imgOpeGear_ = resMng_.Load(ResourceManager::SRC::OPE_GEAR).handleId_;

	pauseImg_ = LoadGraph("Data/Image/pause.png");

	imgNiceKick_ = resMng_.Load(ResourceManager::SRC::NICE_KICK).handleId_;
	imgNextStage_ = resMng_.Load(ResourceManager::SRC::NEXT_STAGE).handleId_;
	imgSelectStage_ = resMng_.Load(ResourceManager::SRC::SELECT_STAGE).handleId_;
	imgBackTitle_ = resMng_.Load(ResourceManager::SRC::BACK_TITLE).handleId_;
	imgAbutton_ = resMng_.Load(ResourceManager::SRC::A_BUTTON).handleId_;

	pauseExplainImgs_[0] = resMng_.Load(ResourceManager::SRC::PAUSEOPE).handleId_; // 操作説明
	pauseExplainImgs_[1] = resMng_.Load(ResourceManager::SRC::PAUSEITEM).handleId_;   // アイテム概要

	//カウンタ
	uiFadeStart_ = false;
	uiFadeFrame_ = 0;
	uiDisplayFrame_ = 0;

	//ポーズ
	isPaused_ = false;
	pauseSelectIndex_ = 0;

	//ステージメニュー
	isStageMenu_ = false;
	stageSelectIndex_ = 0;

	//カメラのポーズ解除
	camera_ = SceneManager::GetInstance().GetCamera().lock();
	if (camera_) {
		camera_->SetPaused(false); //ここが重要！

		//ミニマップ用カメラ
		camera_->SetFollow(&player_->GetTransform());
		camera_->ChangeMode(Camera::MODE::FOLLOW);
	}

	//音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::GAME_BGM, Sound::TIMES::LOOP);

	mainCamera->SetFollow(&player_->GetTransform());
	mainCamera->ChangeMode(Camera::MODE::FOLLOW);

	allyLandTimer_ = 0.0f;
	isKicking_ = false;
	currentKickedAlly_ = nullptr;

	isB_ = BOSS_WAIT;
}

void GameScene::Update(void)
{
	cnt++;
	InputManager& ins = InputManager::GetInstance();

	//簡易deltaTime(60FPS想定）
	const float deltaTime = 1.0f / 60.0f;

	//ステージクリアメニューの処理を最優先
	if (stageMenu_ == StageState::StageMenu)
	{
		if (StageClearMenu()) return;
	}

	if (PauseMenu()) return;

	//右入力で順送り
	if (ins.IsTrgDown(KEY_INPUT_R) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_RIGHT))
	{
		if (!isKicking_)  //通常時
		{
			switch (cameraMode)
			{
			case Camera::MODE::FOLLOW:
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
		else  //味方が飛んでいる間
		{
			switch (cameraMode)
			{
			case Camera::MODE::ALLY_FOLLOW:
				cameraMode = Camera::MODE::SIDE_VIEW;
				mainCamera->ChangeMode(Camera::MODE::SIDE_VIEW);
				break;

			case Camera::MODE::SIDE_VIEW:
				cameraMode = Camera::MODE::ALLY_FOLLOW;
				if (currentKickedAlly_)
					mainCamera->SetFollow(&currentKickedAlly_->GetTransform());
				mainCamera->ChangeMode(Camera::MODE::ALLY_FOLLOW);
				break;
			}
		}
	}

	if (isKicking_ && currentKickedAlly_)
	{
		//味方の状態をチェック
		if (!currentKickedAlly_->IsBlow())
		{
			allyLandTimer_ += deltaTime;

			if (allyLandTimer_ > 1.0f)
			{
				allyLandTimer_ = 0.0f;
				isKicking_ = false;

				currentKickedAlly_ = nullptr;

				cameraMode = Camera::MODE::FOLLOW;
				mainCamera->SetFollow(&player_->GetTransform());
				mainCamera->ChangeMode(Camera::MODE::FOLLOW);

				player_->SetControlEnabled(true);
			}
		}
	}

	//通常の更新処理
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

	//敵が全滅したら次ステージへ
	if (IsAllEnemiesDefeated())
	{
		//最後のステージなら即ゲームクリアへ
		if (stageNo_ >= MAX_STAGE)
		{
			SceneManager::GetInstance()
				.ChangeScene(SceneManager::SCENE_ID::CLEAR);
			return;
		}

		//それ以外は従来どおりクリアメニューへ
		isStageMenu_ = true;
		stageMenu_ = StageState::StageMenu;
		stageSelectIndex_ = 0;

		return;
	}

	if (waitForAllyActionEnd_)
	{
		bool allFinished = true;

		for (auto& ally : Allys_)
		{
			if (!ally) continue;

			// 攻撃中ならまだ終わっていない
			if (ally->IsAttacking())
			{
				allFinished = false;
				break;
			}

			// 動いているならまだ終わっていない
			if (!ally->IsStoppedCompletely())
			{
				allFinished = false;
				break;
			}
		}

		// 敵も同様チェック
		if (allFinished)
		{
			for (auto& enemy : enemys_)
			{
				if (!enemy) continue;

				if (enemy->IsAlive())
				{
					allFinished = false;
					break;
				}

				if (!enemy->IsDeadFinished())
				{
					allFinished = false;
					break;
				}
			}
		}

		if (allFinished)
		{
			waitForAllyActionEnd_ = false;
			CheckEndCondition();
		}

		//3回以内に敵が全滅しなかった場合
		if (resultMenuState_ == RESULT_MENU_STATE::SHOW_RETRY_MENU)
		{
			UpdateRetryMenu();
			return;    // ゲーム本編は止める
		}

	//	bool allFinished = true;

	//	// --- 味方チェック（ログつき） ---
	//	for (size_t i = 0; i < Allys_.size(); ++i)
	//	{
	//		auto& ally = Allys_[i];
	//		if (!ally) continue;

	//		bool isAttacking = ally->IsAttacking();               // 追加した関数を使う前提
	//		bool isStopped = ally->IsStoppedCompletely();
	//		bool isActionFin = ally->IsActionFinished();          // 既存の関数

	//		// ログ: index, flags
	//		printfDx("Ally[%d] Attacking=%d Stopped=%d ActionFin=%d\n",
	//			(int)i, (int)isAttacking, (int)isStopped, (int)isActionFin);

	//		// まだ終わってないなら中断
	//		if (isAttacking || !isStopped || !isActionFin)
	//		{
	//			allFinished = false;
	//			// どの条件で止まっているか詳細ログ
	//			if (isAttacking) printfDx(" -> Ally[%d] is still attacking\n", (int)i);
	//			else if (!isStopped) printfDx(" -> Ally[%d] not stopped\n", (int)i);
	//			else if (!isActionFin) printfDx(" -> Ally[%d] action not finished\n", (int)i);
	//			break;
	//		}
	//	}

	//	// --- 敵チェック（ログつき） ---
	//	if (allFinished)
	//	{
	//		for (size_t j = 0; j < enemys_.size(); ++j)
	//		{
	//			auto& enemy = enemys_[j];
	//			if (!enemy) continue;

	//			bool alive = enemy->IsAlive();
	//			bool deadFinished = enemy->IsDeadFinished();
	//			printfDx("Enemy[%d] Alive=%d DeadFinished=%d\n", (int)j, (int)alive, (int)deadFinished);

	//			if (alive || !deadFinished)
	//			{
	//				allFinished = false;
	//				if (alive) printfDx(" -> Enemy[%d] still alive\n", (int)j);
	//				else printfDx(" -> Enemy[%d] death anim not finished\n", (int)j);
	//				break;
	//			}
	//		}
	//	}

	//	if (allFinished)
	//	{
	//		printfDx("All finished! Calling CheckEndCondition()\n");
	//		waitForAllyActionEnd_ = false;
	//		CheckEndCondition();
	//	}
	}
}

void GameScene::Draw(void)
{
	skyDome_->Draw();
	stage_->Draw();
	for (auto ally : Allys_)
	{
		ally->Draw();
		ally->DrawShots(); //ショットだけ別描画関数を呼ぶ
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

	if (isPaused_) 
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), black, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		if (pauseState_ == PauseState::PauseMenu)
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

	if (resultMenuState_ == RESULT_MENU_STATE::SHOW_RETRY_MENU)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		SetFontSize(DEFAULT_FONT_SIZE * 2.0);
		DrawString(500, 300, "味方が全て行動しました", GetColor(255, 255, 255));

		// 選択肢
		SetFontSize(DEFAULT_FONT_SIZE * 1.5);
		DrawString(520, 380, menuIndex_ == 0 ? "> リトライ" : "  リトライ", menuIndex_ == 0 ? GetColor(255, 255, 0) : GetColor(255, 255, 255));
		DrawString(520, 420, menuIndex_ == 1 ? "> ゲームオーバー" : "  ゲームオーバー", menuIndex_ == 1 ? GetColor(255, 255, 0) : GetColor(255, 255, 255));
		SetFontSize(DEFAULT_FONT_SIZE);
		return;
	}

	if (isStageMenu_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), black, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawGraph(500, 0, imgNiceKick_, true);
		DrawGraph(1300, 700, imgNextStage_, true);
		DrawGraph(1300, 800, imgSelectStage_, true);
		DrawGraph(1300, 900, imgBackTitle_, true);

		//GetNowCount() = 経過ミリ秒
		float alpha2 = (sinf(GetNowCount() * BLINK_SPEED) + 1.0f) * 0.5f;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(alpha2 * 255));

		//カーソル描画
		DrawRotaGraph(
			CURSOR_WIDTH,
			CURSOR_HEIGHT + (stageSelectIndex_ * INDEX),
			0.7,
			0,
			imgAbutton_,
			true
		);

		//ブレンドモード解除
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

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

void GameScene::OnAllyKicked(AllyBase* kickedAlly)
{
	if (!kickedAlly || !mainCamera) return;

	player_->SetControlEnabled(false);

	isKicking_ = true;
	currentKickedAlly_ = kickedAlly;
	allyLandTimer_ = 0.0f;

	kickedAlly->SetActionFinished(false);

	cameraMode = Camera::MODE::ALLY_FOLLOW;
	mainCamera->SetFollow(&kickedAlly->GetTransform());
	mainCamera->ChangeMode(Camera::MODE::ALLY_FOLLOW);

	kickCount_++;

	if (kickCount_ >= MAX_KICK)
	{
		waitForAllyActionEnd_ = true;
	}
}

void GameScene::CheckEndCondition(void)
{
    bool isEnemyAlive = false;

    for (auto& e : enemys_)
    {
        if (e && e->IsAlive())
        {
            isEnemyAlive = true;
            break;
        }
    }

    if (isEnemyAlive)
    {
        // 敵が残っている → リトライ / ゲームオーバー メニューへ
        resultMenuState_ = RESULT_MENU_STATE::SHOW_RETRY_MENU;
        menuIndex_ = 0;
        player_->SetControlEnabled(false);
    }
}

void GameScene::UpdateRetryMenu(void)
{
	InputManager& ins = InputManager::GetInstance();
	// 矢印キーで選択
	if (ins.IsTrgDown(KEY_INPUT_DOWN) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_DOWN))
	{
		menuIndex_ = (menuIndex_ + 1) % 2;
	}
	if (ins.IsTrgDown(KEY_INPUT_UP) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_TOP))
	{
		menuIndex_ = (menuIndex_ + 2 - 1) % 2; // (menuIndex_ - 1 + 2) % 2
	}

	// 決定キー
	if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		if (menuIndex_ == 0)
		{
			// リトライ：同じステージを再読み込みする
			mainCamera->SetPaused(false);
			SceneManager::GetInstance().ChangeStageScene(SceneManager::SCENE_ID::GAME, stageNo_);
			return;
		}
		else if (menuIndex_ == 1)
		{
			// ゲームオーバー
			mainCamera->SetPaused(false);
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
			return;
		}
	}

	// メニュー表示中はそれ以外の更新を行わない
	return;
}

void GameScene::ReturnToPlayerCamera(void)
{
	//プレイヤー追尾に戻す
	cameraMode = Camera::MODE::FOLLOW;
	mainCamera->SetFollow(&player_->GetTransform());
	mainCamera->ChangeMode(Camera::MODE::FOLLOW);
	player_->SetControlEnabled(true);
}

void GameScene::AllyCreate(void)
{
	Allys_.clear(); // 念のためクリア

	// プレイヤー位置と回転を取得
	VECTOR basePos = player_->GetTransform().pos;
	float playerAngleY = player_->GetTransform().rot.y; //Y軸回転（ラジアン）

	//前方・右方向ベクトル
	VECTOR forward = VGet(sinf(playerAngleY), 0.0f, cosf(playerAngleY));
	VECTOR right = VGet(cosf(playerAngleY), 0.0f, -sinf(playerAngleY));

	//プレイヤーの前方に出す距離と横間隔
	const float forwardDist = 300.0f; //前方方向へ距離
	const float spacing = 200.0f;     //横間隔

	//3種類の味方を左から右へ並べる
	struct EnemySpawnData {
		AllyBase::TYPE type;
		float horizontalOffset; //横方向の位置補正
	};

	std::vector<EnemySpawnData> spawnList = {
		{ AllyBase::TYPE::RED,   -spacing },	//左
		{ AllyBase::TYPE::BLUE,   0.0f     },	//中央
		{ AllyBase::TYPE::BLACK,  spacing  }	//右
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
			enemy =std::make_shared<AllyBule>();
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

void GameScene::EnemyCreate(void)
{
	enemySpawnTable_[1] = {
		{ VGet(X_ENEMY_POS, Y_ENEMY_POS, 2500), 2},
	};

	enemySpawnTable_[2] = {
		{ VGet(X_ENEMY_POS + 200, Y_ENEMY_POS, 1500), 1 },
		{ VGet(X_ENEMY_POS - 200, Y_ENEMY_POS, 1500), 1 },
	};

	enemySpawnTable_[3] = {
		{ VGet(X_ENEMY_POS, Y_ENEMY_POS, 3500), 3 },
	};

	enemySpawnTable_[4] = {
		{ VGet(X_ENEMY_POS, Y_ENEMY_POS, 1500), 0 },
		{ VGet(X_ENEMY_POS, Y_ENEMY_POS, 2500), 1 },
		{ VGet(X_ENEMY_POS, Y_ENEMY_POS, 3500), 2 },
	};

	enemySpawnTable_[5] = {
		{ VGet(X_ENEMY_POS, Y_ENEMY_POS, 1500), 0 },
		{ VGet(X_ENEMY_POS, Y_ENEMY_POS, 2500), 0 },
		{ VGet(X_ENEMY_POS, Y_ENEMY_POS, 3500), 3 },
	};
}

bool GameScene::PauseMenu(void)
{
	InputManager& ins = InputManager::GetInstance();

	//ポーズ画面種類
	constexpr int GameBack = 0;
	constexpr int ShowControls = 1;
	constexpr int ShowItems = 2;
	constexpr int TitleBack = 3;

	//TABキーでポーズのON/OFF切り替え（Menu中のみ）
	if (ins.IsTrgDown(KEY_INPUT_TAB)|| 
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::MENU)
		&& pauseState_ == PauseState::PauseMenu)
	{
		isPaused_ = !isPaused_;
		pauseSelectIndex_ = 0;
		mainCamera->SetPaused(isPaused_);
		return true;
	}

	if (!isPaused_) return false; // ポーズ中でなければ通常更新

	if (pauseState_ == PauseState::PauseMenu)
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
			case GameBack:
				isPaused_ = false;
				mainCamera->SetPaused(false);
				break;

			case ShowControls: 
				pauseState_ = PauseState::ShowControls; 
				break;

			case ShowItems:
				pauseState_ = PauseState::ShowItems; 
				break;

			case TitleBack:
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
				break;
			}
		}
	}
	else
	{
		if (ins.IsTrgDown(KEY_INPUT_RETURN)||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
			pauseState_ = PauseState::PauseMenu;
	}

	return true;
}

bool GameScene::StageClearMenu(void)
{
	InputManager& ins = InputManager::GetInstance();

	constexpr int NextStage = 0;
	constexpr int StageSelect = 1;
	constexpr int BackToTitle = 2;

	if (isStageMenu_)
	{
		if (ins.IsTrgDown(KEY_INPUT_DOWN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_DOWN))
			stageSelectIndex_ = (stageSelectIndex_ + 1) % 3;

		if (ins.IsTrgDown(KEY_INPUT_UP) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_TOP))
			stageSelectIndex_ = (stageSelectIndex_ + 3 - 1) % 3;

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
		{
			switch (stageSelectIndex_)
			{
			case NextStage:
				if (stageNo_ >= MAX_STAGE) {
					//すべてのステージをクリアしたのでゲームクリアへ
					SceneManager::GetInstance()
						.ChangeScene(SceneManager::SCENE_ID::CLEAR);
				}
				else {
					//次のステージへ
					SceneManager::GetInstance()
						.ChangeStageScene(SceneManager::SCENE_ID::GAME, stageNo_ + 1);
				}

				break;

			case StageSelect:
				SceneManager::GetInstance()
					.ChangeScene(SceneManager::SCENE_ID::STAGE_SELECT);
				break;

			case BackToTitle:
				SceneManager::GetInstance()
					.ChangeScene(SceneManager::SCENE_ID::TITLE);
				break;
			}
		}

		return true;
	}

	return false;
}

void GameScene::SpawnEnemies(int stageNo)
{
	//登録がないステージ
	if (enemySpawnTable_.count(stageNo) == 0)
		return;

	//敵種類
	constexpr int Dog = 0;
	constexpr int Onion = 1;
	constexpr int Virus = 2;
	constexpr int Boss = 3;

	auto& list = enemySpawnTable_[stageNo];

	for (const auto& e : list)
	{
		std::shared_ptr<EnemyBase> enemy;

		//敵の種類を割り当て
		switch (e.type)
		{
		case Dog:
			enemy = std::make_shared<EnemyDog>();
			break;

		case Onion:
			enemy = std::make_shared<EnemyOnion>();
			break;

		case Virus:
			enemy = std::make_shared<EnemyVirus>();
			break;

		case Boss:
			enemy = std::make_shared<EnemyBoss>();
			break;

		default:
			continue;
		}

		enemy->SetGameScene(this);
		enemy->SetPos(e.pos);
		enemy->SetPlayer(player_);
		enemy->SetAlly(&Allys_);
		enemy->Init();

		enemys_.push_back(enemy);
	}
}

bool GameScene::IsAnyAllyFlying(void) const
{
	for (auto& a : Allys_)
	{
		if (a->IsBlow())
			return true;
	}
	return false;
}

//今飛んでいる味方を返す（必要なら）
AllyBase* GameScene::GetFlyingAlly(void) const
{
	for (auto& a : Allys_)
	{
		if (a->IsBlow())
			return a.get();
	}
	return nullptr;
}

bool GameScene::IsAllEnemiesDefeated(void) const
{
	for (auto& e : enemys_)
	{
		if (!e) continue;
		if (e->IsAlive())
			return false;
	}
	return true;
}
