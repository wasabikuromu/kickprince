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
	imgOpeGearKey_ = -1;
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
	imgOpeGearKey_ = resMng_.Load(ResourceManager::SRC::OPE_GEAR_KEYBOARD).handleId_;
	imgOpeGearCon_ = resMng_.Load(ResourceManager::SRC::OPE_GEAR_CONTROLLER).handleId_;

	//ポーズ画像
	pauseImg_ = LoadGraph("Data/Image/Pause/Pause.png");

	//ステージクリア後
	imgNiceKick_ = resMng_.Load(ResourceManager::SRC::NICE_KICK).handleId_;
	imgNextStage_ = resMng_.Load(ResourceManager::SRC::NEXT_STAGE).handleId_;
	imgSelectStage_ = resMng_.Load(ResourceManager::SRC::SELECT_STAGE).handleId_;
	imgBackTitle_ = resMng_.Load(ResourceManager::SRC::BACK_TITLE).handleId_;

	//Aボタン
	imgAbutton_ = resMng_.Load(ResourceManager::SRC::A_BUTTON).handleId_;

	//ポーズ関連
	pauseMenuImgs[0] = resMng_.Load(ResourceManager::SRC::PAUSE_1).handleId_;
	pauseMenuImgs[1] = resMng_.Load(ResourceManager::SRC::PAUSE_2).handleId_;
	pauseMenuImgs[2] = resMng_.Load(ResourceManager::SRC::PAUSE_3).handleId_;
	pauseMenuImgs[3] = resMng_.Load(ResourceManager::SRC::PAUSE_4).handleId_;
	pauseMenuImgs[4] = resMng_.Load(ResourceManager::SRC::PAUSE_5).handleId_;

	pauseMenuImgsSelected[0] = resMng_.Load(ResourceManager::SRC::SELECT_PAUSE_1).handleId_;
	pauseMenuImgsSelected[1] = resMng_.Load(ResourceManager::SRC::SELECT_PAUSE_2).handleId_;
	pauseMenuImgsSelected[2] = resMng_.Load(ResourceManager::SRC::SELECT_PAUSE_3).handleId_;
	pauseMenuImgsSelected[3] = resMng_.Load(ResourceManager::SRC::SELECT_PAUSE_4).handleId_;
	pauseMenuImgsSelected[4] = resMng_.Load(ResourceManager::SRC::SELECT_PAUSE_5).handleId_;

	pauseExplainImgs_[0] = resMng_.Load(ResourceManager::SRC::PAUSEOPE1).handleId_;		//操作説明
	pauseExplainImgs_[1] = resMng_.Load(ResourceManager::SRC::PAUSEOPE2).handleId_;		//操作説明
	pauseExplainImgs_[2] = resMng_.Load(ResourceManager::SRC::PAUSEALLY).handleId_;		//アイテム概要

	pauseMenuPosY[0] = UI_HEIGHT_PAUSE_1;
	pauseMenuPosY[1] = UI_HEIGHT_PAUSE_2;
	pauseMenuPosY[2] = UI_HEIGHT_PAUSE_3;
	pauseMenuPosY[3] = UI_HEIGHT_PAUSE_4;
	pauseMenuPosY[4] = UI_HEIGHT_PAUSE_5;

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
		camera_->SetPaused(false); 

		//ミニマップ用カメラ
		camera_->SetFollow(&player_->GetTransform());
		camera_->ChangeMode(Camera::MODE::FOLLOW);
	}

	//音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::GAME_BGM, Sound::TIMES::LOOP);

	mainCamera->SetFollow(&player_->GetTransform());
	mainCamera->ChangeMode(Camera::MODE::FOLLOW);
	mainCamera->SetControlEnabled(true);

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
	const float deltaTime = 1.0f / DELTA_TIME;

	//ステージクリアメニューの処理を最優先
	if (stageMenu_ == StageState::StageMenu)
	{
		if (StageClearMenu()) return;
	}

	if (PauseMenu()) return;

	if (ins.IsTrgDown(KEY_INPUT_R) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::TOP))
	{
		if (!isKicking_)
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
		else
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

	if (GetJoypadNum() == 0) 
	{
		DrawRotaGraph(UI_GEAR, UI_GEAR, IMG_OPEGEAR_UI_SIZE, 0.0, imgOpeGearKey_, true);
	}
	else
	{
		DrawRotaGraph(UI_GEAR, UI_GEAR, IMG_OPEGEAR_UI_SIZE, 0.0, imgOpeGearCon_, true);
	}
	
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

	//ポーズUIレイアウト
	static constexpr int DARK_ALPHA = 200;			//グレー目にする
	static constexpr int CONTROL_ALPHA = 110;		//コントローラー部の明るさ
	static constexpr int ALLY_ALPHA = 150;			//味方教える部の明るさ

	static constexpr int PAUSE_Y = 100;				//pauseImg_のY

	if (isPaused_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, DARK_ALPHA);
		DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), black, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawRotaGraph((Application::SCREEN_SIZE_X) / 2, PAUSE_Y, 1.0, 0.0, pauseImg_, true);

		if (pauseState_ == PauseState::PauseMenu)
		{
			for (int i = 0; i < PAUSE_MENU_ITEM_COUNT; i++)
			{
				//選択中なら黄色
				int img = (pauseSelectIndex_ % PAUSE_MENU_ITEM_COUNT == i)
					? pauseMenuImgsSelected[i]
					: pauseMenuImgs[i];

				//画像のサイズ取得
				int w, h;
				GetGraphSize(img, &w, &h);

				//中央揃え座標計算
				int drawX = Application::SCREEN_SIZE_X / 2 - w / 2;
				int drawY = pauseMenuPosY[i];

				DrawGraph(drawX, drawY, img, true);
			}
		}
		else if (pauseState_ == PauseState::ShowControls)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, CONTROL_ALPHA);
			DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), white, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			if (GetJoypadNum() == 0)
			{
				DrawGraph(0, 0, pauseExplainImgs_[1], true);
			}
			else
			{
				DrawGraph(0, 0, pauseExplainImgs_[0], true);
			}
		}
		else if (pauseState_ == PauseState::ShowAllies)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, ALLY_ALPHA);
			DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), white, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			DrawGraph(0, 0, pauseExplainImgs_[2], true);
		}
		return;
	}

	//ステージUIレイアウト
	constexpr int BG_ALPHA = 200;			//グレー目にする
		
	constexpr int NICE_KICK_X = 500;		//imgNiceKick_のX
	constexpr int NICE_KICK_Y = 0;			//imgNiceKick_のY

	constexpr int MENU_X = 1300;			//メニューのX

	constexpr int NEXT_STAGE_Y = 700;		//imgNextStage_のY
	constexpr int SELECT_STAGE_Y = 800;		//imgSelectStage_のY
	constexpr int BACK_TITLE_Y = 900;		//imgBackTitle_のY
	constexpr float CURSOR_SCALE = 0.7f;	//imgAbutton_の大きさ

	if (isStageMenu_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, BG_ALPHA);
		DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), black, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawGraph(NICE_KICK_X, NICE_KICK_Y, imgNiceKick_, true);
		DrawGraph(MENU_X, NEXT_STAGE_Y, imgNextStage_, true);
		DrawGraph(MENU_X, SELECT_STAGE_Y, imgSelectStage_, true);
		DrawGraph(MENU_X, BACK_TITLE_Y, imgBackTitle_, true);

		//GetNowCount() = 経過ミリ秒
		float alpha2 = (sinf(GetNowCount() * BLINK_SPEED) + 1.0f) * 0.5f;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(alpha2 * MAX_ALPHA));

		//カーソル描画
		DrawRotaGraph(
			CURSOR_WIDTH,
			CURSOR_HEIGHT + (stageSelectIndex_ * INDEX),
			CURSOR_SCALE,
			0,
			imgAbutton_,
			true
		);

		//ブレンドモード解除
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		return;
	}
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
}

void GameScene::UpdateRetryMenu(void)
{
	InputManager& ins = InputManager::GetInstance();

	constexpr int RETRY_MENU_COUNT = 2;

	//矢印キーで選択
	if (ins.IsTrgDown(KEY_INPUT_DOWN) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_DOWN))
	{
		menuIndex_ = (menuIndex_ + 1) % RETRY_MENU_COUNT;
	}
	if (ins.IsTrgDown(KEY_INPUT_UP) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_TOP))
	{
		menuIndex_ = (menuIndex_ + RETRY_MENU_COUNT - 1) % RETRY_MENU_COUNT;
	}

	//決定キー
	if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		if (menuIndex_ == 0)
		{
			//リトライ：同じステージを再読み込みする
			mainCamera->SetPaused(false);
			SceneManager::GetInstance().ChangeStageScene(SceneManager::SCENE_ID::GAME, stageNo_);
			return;
		}
		else if (menuIndex_ == 1)
		{
			//ゲームオーバー 
			mainCamera->SetPaused(false);
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
			return;
		}
	}

	//メニュー表示中はそれ以外の更新を行わない
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
	Allys_.clear(); //念のためクリア

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
			VScale(forward, forwardDist),               
			VScale(right, data.horizontalOffset)          
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
		{ VGet(-MAIN_ENEMY_X, MAIN_ENEMY_Y, MAIN_DEPTH_2), VIRUS},
	};

	enemySpawnTable_[2] = {
		{ VGet(-MAIN_ENEMY_X + ONION_WIDTH_1, MAIN_ENEMY_Y, MAIN_DEPTH_1), ONION },
		{ VGet(-MAIN_ENEMY_X - ONION_WIDTH_1, MAIN_ENEMY_Y, MAIN_DEPTH_1), ONION },
		{ VGet(-MAIN_ENEMY_X + ONION_WIDTH_2, MAIN_ENEMY_Y, MAIN_DEPTH_1 + ONION_DEPTH), ONION },
		{ VGet(-MAIN_ENEMY_X - ONION_WIDTH_2, MAIN_ENEMY_Y, MAIN_DEPTH_1 + ONION_DEPTH), ONION },
	};

	enemySpawnTable_[3] = {
		{ VGet(-MAIN_ENEMY_X, MAIN_ENEMY_Y, MAIN_DEPTH_3), BOSS },
	};

	enemySpawnTable_[4] = {
		{ VGet(-MAIN_ENEMY_X, MAIN_ENEMY_Y, MAIN_DEPTH_1), DOG },
		{ VGet(-MAIN_ENEMY_X, MAIN_ENEMY_Y, MAIN_DEPTH_2), ONION },
		{ VGet(-MAIN_ENEMY_X, MAIN_ENEMY_Y, MAIN_DEPTH_3), VIRUS },
	};

	enemySpawnTable_[5] = {
		{ VGet(-MAIN_ENEMY_X + ONION_WIDTH_1, MAIN_ENEMY_Y, MAIN_DEPTH_1), ONION },
		{ VGet(-MAIN_ENEMY_X - ONION_WIDTH_1, MAIN_ENEMY_Y, MAIN_DEPTH_1), ONION },
		{ VGet(-MAIN_ENEMY_X, MAIN_ENEMY_Y, MAIN_DEPTH_3), BOSS },
	};
}

bool GameScene::PauseMenu(void)
{
	InputManager& ins = InputManager::GetInstance();

	//ポーズ画面種類
	constexpr int GameBack = 0;
	constexpr int ShowControls = 1;
	constexpr int ShowAllies = 2;
	constexpr int Retry = 3;
	constexpr int GiveUp = 4;

	//TABキーでポーズのON/OFF切り替え（Menu中のみ）
	if (ins.IsTrgDown(KEY_INPUT_TAB)|| 
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::MENU)
		&& pauseState_ == PauseState::PauseMenu)
	{
		SoundManager::GetInstance().Play(SoundManager::SRC::PAUSE_SE, Sound::TIMES::ONCE);
		isPaused_ = !isPaused_;
		pauseSelectIndex_ = 0;
		mainCamera->SetPaused(isPaused_);
		return true;
	}

	if (!isPaused_) return false; //ポーズ中でなければ通常更新

	if (pauseState_ == PauseState::PauseMenu)
	{
		if (ins.IsTrgDown(KEY_INPUT_DOWN) ||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_DOWN))
		{
			SoundManager::GetInstance().Play(SoundManager::SRC::CURSOR_MOVE_SE, Sound::TIMES::ONCE);
			pauseSelectIndex_ = (pauseSelectIndex_ + PAUSE_MENU_DOWN) % PAUSE_MENU_ITEM_COUNT;
		}

		if (ins.IsTrgDown(KEY_INPUT_UP) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_TOP))
		{
			SoundManager::GetInstance().Play(SoundManager::SRC::CURSOR_MOVE_SE, Sound::TIMES::ONCE);
			pauseSelectIndex_ = (pauseSelectIndex_ + PAUSE_MENU_UP) % PAUSE_MENU_ITEM_COUNT;
		}

		if (ins.IsTrgDown(KEY_INPUT_RETURN)||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
		{
			SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::ONCE);
			switch (pauseSelectIndex_)
			{
			case GameBack:
				isPaused_ = false;
				mainCamera->SetPaused(false);
				break;

			case ShowControls: 
				pauseState_ = PauseState::ShowControls; 
				break;

			case ShowAllies:
				pauseState_ = PauseState::ShowAllies; 
				break;

			case Retry:
				SceneManager::GetInstance().ChangeStageScene(SceneManager::SCENE_ID::GAME, stageNo_);
				break;

			case GiveUp:
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
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
		{
			SoundManager::GetInstance().Play(SoundManager::SRC::CURSOR_MOVE_SE, Sound::TIMES::ONCE);
			stageSelectIndex_ = (stageSelectIndex_ + 1) % 3;
		}

		if (ins.IsTrgDown(KEY_INPUT_UP) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_TOP))
		{
			SoundManager::GetInstance().Play(SoundManager::SRC::CURSOR_MOVE_SE, Sound::TIMES::ONCE);
			stageSelectIndex_ = (stageSelectIndex_ + 3 - 1) % 3;
		}

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
		{
			SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::ONCE);
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

	auto& list = enemySpawnTable_[stageNo];

	for (const auto& e : list)
	{
		std::shared_ptr<EnemyBase> enemy;

		//敵の種類を割り当て
		switch (e.type)
		{
		case DOG:
			enemy = std::make_shared<EnemyDog>();
			break;

		case ONION:
			enemy = std::make_shared<EnemyOnion>();
			break;

		case VIRUS:
			enemy = std::make_shared<EnemyVirus>();
			break;

		case BOSS:
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
