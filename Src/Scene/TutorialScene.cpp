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
#include "TutorialScene.h"

static Camera::MODE cameraMode = Camera::MODE::FOLLOW;

TutorialScene::TutorialScene(int stageNo)
	: stageNo_(stageNo)
{
	player_ = nullptr;
	skyDome_ = nullptr;
	stage_ = nullptr;
	imgOpeGearKey_ = -1;
}

TutorialScene::~TutorialScene(void)
{
}

void TutorialScene::Init(void)
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

	imgPlayerMove_ = LoadGraph("Data/Image/Tutorial/PlayerMove.png");
	imgCameraMove_ = LoadGraph("Data/Image/Tutorial/CameraMove.png");
	imgAttack_ = LoadGraph("Data/Image/Tutorial/Attack.png");

	imgAbutton_ = resMng_.Load(ResourceManager::SRC::A_BUTTON).handleId_;
	imgEnter_ = resMng_.Load(ResourceManager::SRC::ENTER).handleId_;

	//チュートリアルメッセージ画像
	imgTutorialMsg_[1] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_01).handleId_;
	imgTutorialMsg_[2] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_02).handleId_;
	imgTutorialMsg_[3] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_03).handleId_;
	imgTutorialMsg_[4] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_04).handleId_;
	imgTutorialMsg_[5] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_05).handleId_;
	imgTutorialMsg_[6] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_06).handleId_;
	imgTutorialMsg_[7] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_07).handleId_;
	imgTutorialMsg_[8] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_08).handleId_;
	imgTutorialMsg_[9] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_09).handleId_;
	imgTutorialMsg_[10] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_10).handleId_;
	imgTutorialMsg_[11] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_11).handleId_;
	imgTutorialMsg_[12] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_12).handleId_;
	imgTutorialMsg_[13] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_13).handleId_;
	imgTutorialMsg_[14] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_14).handleId_;
	imgTutorialMsg_[15] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_15).handleId_;
	imgTutorialMsg_[16] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_16).handleId_;
	imgTutorialMsg_[17] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_17).handleId_;
	imgTutorialMsg_[18] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_18).handleId_;
	imgTutorialMsg_[19] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_19).handleId_;
	imgTutorialMsg_[20] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_20).handleId_;
	imgTutorialMsg_[21] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_21).handleId_;
	imgTutorialMsg_[22] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_22).handleId_;
	imgTutorialMsg_[23] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_23).handleId_;
	imgTutorialMsg_[24] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_24).handleId_;
	imgTutorialMsg_[25] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_25).handleId_;
	imgTutorialMsg_[26] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_26).handleId_;
	imgTutorialMsg_[27] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_27).handleId_;
	imgTutorialMsg_[28] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_28).handleId_;
	imgTutorialMsg_[29] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_29).handleId_;
	imgTutorialMsg_[30] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_30).handleId_;
	imgTutorialMsg_[31] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_31).handleId_;
	imgTutorialMsg_[32] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_32).handleId_;
	imgTutorialMsg_[33] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_33).handleId_;
	imgTutorialMsg_[34] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_34).handleId_;
	imgTutorialMsg_[35] = resMng_.Load(ResourceManager::SRC::TUTORIAL_MSG_35).handleId_;

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

	pauseMenuPosY[0] = 350;
	pauseMenuPosY[1] = 470;
	pauseMenuPosY[2] = 590;
	pauseMenuPosY[3] = 710;
	pauseMenuPosY[4] = 830;

	pauseImg_ = LoadGraph("Data/Image/Pause/Pause.png");

	pauseExplainImgs_[0] = resMng_.Load(ResourceManager::SRC::PAUSEOPE1).handleId_;		//操作説明
	pauseExplainImgs_[1] = resMng_.Load(ResourceManager::SRC::PAUSEOPE2).handleId_;		//操作説明
	pauseExplainImgs_[2] = resMng_.Load(ResourceManager::SRC::PAUSEALLY).handleId_;		//アイテム概要

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

	currentStep_ = 1;

	isB_ = BOSS_WAIT;

	step_ = TutorialStep::STEP_01_MESSAGE;

	// まずすべての操作を禁止
	player_->SetControlEnabled(false);
	mainCamera->SetControlEnabled(false);
}

void TutorialScene::Update(void)
{
	cnt++;
	InputManager& ins = InputManager::GetInstance();

	//簡易deltaTime(60FPS想定）
	const float deltaTime = 1.0f / 60.0f;

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

	RunTutorial();
}

void TutorialScene::Draw(void)
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

	//チュートリアル
	if (showMessage_) {
		DrawGraph(1050, 30, imgCurrentMessage_, true);

		float alpha2 = (sinf(GetNowCount() * BLINK_SPEED) + 1.0f) * 0.5f;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(alpha2 * 255));

		if (GetJoypadNum() == 0)
		{
			//カーソル描画
			DrawRotaGraph(
				1810,
				155,
				0.6,
				0,
				imgEnter_,
				true
			);
		}
		else
		{
			//カーソル描画
			DrawRotaGraph(
				1810,
				155,
				0.4,
				0,
				imgAbutton_,
				true
			);
		}

		//ブレンドモード解除
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (isTutorialPlayerMove_)
	{
		DrawGraph(1100, 30, imgPlayerMove_, true);
	}

	if (isTutorialCameraMove_)
	{
		DrawGraph(1000, 30, imgCameraMove_, true);
	}

	if (isTutorialAttack_)
	{
		DrawGraph(1100, 30, imgAttack_, true);
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

	if (isPaused_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), black, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawRotaGraph((Application::SCREEN_SIZE_X) / 2, 100, 1.0, 0.0, pauseImg_, true);

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

				//DrawRotaGraph(drawX, drawY, 1.0, 0.0, img, true);
				DrawGraph(drawX, drawY, img, true);
			}
		}
		else if (pauseState_ == PauseState::ShowControls)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 110);
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
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
			DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), white, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			DrawGraph(0, 0, pauseExplainImgs_[2], true);
		}
		return;
	}
}

void TutorialScene::Release(void)
{
	SoundManager::GetInstance().Stop(SoundManager::SRC::GAME_BGM);
}

void TutorialScene::AddItem(std::shared_ptr<Item> item)
{
	items_.push_back(item);
}

const std::vector<std::shared_ptr<AllyBase>>& TutorialScene::GetEnemies() const
{
	return Allys_;
}

void TutorialScene::NotifyAllyKicked(void)
{
	tutorialFlags_.isAllyKicked = true;
}

void TutorialScene::OnAllyKicked(void)
{
	tutorialFlags_.isAllyKicked = true;
}

void TutorialScene::OnAllyKicked(AllyBase* kickedAlly)
{
	if (!kickedAlly) return;
	if (!mainCamera) return;

	tutorialFlags_.isAllyKicked = true;

	isKicking_ = true;
	currentKickedAlly_ = kickedAlly;
	allyLandTimer_ = 0.0f;

	cameraMode = Camera::MODE::ALLY_FOLLOW;
	mainCamera->SetFollow(&kickedAlly->GetTransform());
	mainCamera->ChangeMode(Camera::MODE::ALLY_FOLLOW);

	player_->SetControlEnabled(false);
}

void TutorialScene::ReturnToPlayerCamera(void)
{
	//プレイヤー追尾に戻す
	cameraMode = Camera::MODE::FOLLOW;
	mainCamera->SetFollow(&player_->GetTransform());
	mainCamera->ChangeMode(Camera::MODE::FOLLOW);
	player_->SetControlEnabled(true);
}

void TutorialScene::AllyCreate(void)
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
		std::shared_ptr<AllyBase> ally;

		// 種類ごとに生成
		switch (data.type)
		{
		case AllyBase::TYPE::RED:
			ally = std::make_shared<AllyRed>();
			break;

		case AllyBase::TYPE::BLUE:
			ally = std::make_shared<AllyBule>();
			break;

		case AllyBase::TYPE::BLACK:
			ally = std::make_shared<AllyBlack>();
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
		ally->SetTutorialScene(this);
		ally->SetPos(spawnPos);
		ally->SetPlayer(player_);
		ally->SetEnemy(&enemys_);
		ally->Init();

		Allys_.emplace_back(std::move(ally));
	}
}

void TutorialScene::EnemyCreate(void)
{
	enemySpawnTable_[1] = {
		{ VGet(X_ENEMY_POS, Y_ENEMY_POS, 2500), 2},
	};

}

bool TutorialScene::PauseMenu(void)
{
	InputManager& ins = InputManager::GetInstance();

	//ポーズ画面種類
	constexpr int GameBack = 0;
	constexpr int ShowControls = 1;
	constexpr int ShowAllies = 2;
	constexpr int Retry = 3;
	constexpr int GiveUp = 4;

	//TABキーでポーズのON/OFF切り替え（Menu中のみ）
	if (ins.IsTrgDown(KEY_INPUT_TAB) ||
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
		if (ins.IsTrgDown(KEY_INPUT_DOWN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_DOWN))
			pauseSelectIndex_ = (pauseSelectIndex_ + PAUSE_MENU_DOWN) % PAUSE_MENU_ITEM_COUNT;

		if (ins.IsTrgDown(KEY_INPUT_UP) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_TOP))
			pauseSelectIndex_ = (pauseSelectIndex_ + PAUSE_MENU_UP) % PAUSE_MENU_ITEM_COUNT;

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
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

			case ShowAllies:
				pauseState_ = PauseState::ShowAllies;
				break;

			case Retry:
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TUTORIAL);
				break;

			case GiveUp:
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
				break;
			}
		}
	}
	else
	{
		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
			pauseState_ = PauseState::PauseMenu;
	}

	return true;
}



void TutorialScene::SpawnEnemies(int stageNo)
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

		enemy->SetTutorialScene(this);
		enemy->SetPos(e.pos);
		enemy->SetPlayer(player_);
		enemy->SetAlly(&Allys_);
		enemy->Init();

		enemys_.push_back(enemy);
	}
}

bool TutorialScene::IsAnyAllyFlying(void) const
{
	for (auto& a : Allys_)
	{
		if (a->IsBlow())
			return true;
	}
	return false;
}

//今飛んでいる味方を返す（必要なら）
AllyBase* TutorialScene::GetFlyingAlly(void) const
{
	for (auto& a : Allys_)
	{
		if (a->IsBlow())
			return a.get();
	}
	return nullptr;
}

bool TutorialScene::IsAllEnemiesDefeated(void) const
{
	for (auto& e : enemys_)
	{
		if (!e) continue;
		if (e->IsAlive())
			return false;
	}
	return true;
}

void TutorialScene::RunTutorial(void)
{
	InputManager& ins = InputManager::GetInstance();

	Camera& cam = *camera_;
	Player& play = *player_;

	switch (step_)
	{
	case TutorialStep::STEP_01_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[1];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)){
			step_ = TutorialStep::STEP_02_MESSAGE;
		}
		break;

	case TutorialStep::STEP_02_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[2];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)){
			step_ = TutorialStep::STEP_03_MESSAGE;
		}
		break;

	case TutorialStep::STEP_03_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[3];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_04_MESSAGE;
		}
		break;

	case TutorialStep::STEP_04_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[4];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_05_MESSAGE;
		}
		break;

	case TutorialStep::STEP_05_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[5];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_06_MESSAGE;
		}
		break;

	case TutorialStep::STEP_06_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[6];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_07_MESSAGE;
		}
		break;

	case TutorialStep::STEP_07_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[7];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_08_MESSAGE;
		}
		break;

	case TutorialStep::STEP_08_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[8];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_09_MESSAGE;
		}
		break;

	case TutorialStep::STEP_09_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[9];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_10_MESSAGE;
		}
		break;

	case TutorialStep::STEP_10_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[10];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			player_->SetControlEnabled(true);
			cam.SetControlEnabled(false);
			moveStartPos_ = player_->GetTransform().pos;
			showMessage_ = false;
			step_ = TutorialStep::WAIT_MOVE;
		}
		break;

	case TutorialStep::WAIT_MOVE:
	{
		showMessage_ = false;
		isTutorialPlayerMove_ = true;
		player_->SetAttackEnabled(false);
		VECTOR pos = player_->GetTransform().pos;
		float dist = sqrtf(
			(pos.x - moveStartPos_.x) * (pos.x - moveStartPos_.x) +
			(pos.z - moveStartPos_.z) * (pos.z - moveStartPos_.z)
		);

		if (dist > 300.0f) {
			player_->SetControlEnabled(false);
			step_ = TutorialStep::STEP_11_MESSAGE;
		}
		break;
	}

	case TutorialStep::STEP_11_MESSAGE:
		showMessage_ = true;
		isTutorialPlayerMove_ = false;
		imgCurrentMessage_ = imgTutorialMsg_[11];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_12_MESSAGE;
		}
		break;

	case TutorialStep::STEP_12_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[12];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_13_MESSAGE;
		}
		break;

	case TutorialStep::STEP_13_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[13];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			cameraStartRotX_ = cam.GetRotX();
			cameraStartRotY_ = cam.GetRotY();

			showMessage_ = false;
			step_ = TutorialStep::WAIT_CAMERA;
		}
		break;

	case TutorialStep::WAIT_CAMERA:
	{
		showMessage_ = false;
		isTutorialCameraMove_ = true;
		player_->SetAttackEnabled(false);
		cam.SetControlEnabled(true);
		float dx = fabsf(cam.GetRotX() - cameraStartRotX_);
		float dy = fabsf(cam.GetRotY() - cameraStartRotY_);

		if (dx + dy > 0.4f) {
			cam.SetControlEnabled(false);
			step_ = TutorialStep::STEP_14_MESSAGE;
		}
		break;
	}

	case TutorialStep::STEP_14_MESSAGE:
		showMessage_ = true;
		isTutorialCameraMove_ = false;
		imgCurrentMessage_ = imgTutorialMsg_[14];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_15_MESSAGE;
		}
		break;

	case TutorialStep::STEP_15_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[15];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_16_MESSAGE;
		}
		break;

	case TutorialStep::STEP_16_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[16];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_17_MESSAGE;
		}
		break;

	case TutorialStep::STEP_17_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[17];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_18_MESSAGE;
		}
		break;

	case TutorialStep::STEP_18_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[18];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_19_MESSAGE;
		}
		break;

	case TutorialStep::STEP_19_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[19];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_20_MESSAGE;
		}
		break;

	case TutorialStep::STEP_20_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[20];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_21_MESSAGE;
		}
		break;

	case TutorialStep::STEP_21_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[21];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_22_MESSAGE;
		}
		break;

	case TutorialStep::STEP_22_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[22];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_23_MESSAGE;
		}
		break;

	case TutorialStep::STEP_23_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[23];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_24_MESSAGE;
		}
		break;

	case TutorialStep::STEP_24_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[24];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_25_MESSAGE;
		}
		break;

	case TutorialStep::STEP_25_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[25];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_26_MESSAGE;
		}
		break;

	case TutorialStep::STEP_26_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[26];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_27_MESSAGE;
		}
		break;

	case TutorialStep::STEP_27_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[27];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_28_MESSAGE;
		}
		break;

	case TutorialStep::STEP_28_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[28];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_29_MESSAGE;
		}
		break;

	case TutorialStep::STEP_29_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[29];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_30_MESSAGE;
		}
		break;

	case TutorialStep::STEP_30_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[30];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_31_MESSAGE;
		}
		break;

	case TutorialStep::STEP_31_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[31];
		player_->SetAttackEnabled(false);
		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::WAIT_ATTACK;
		}
		break;

	case TutorialStep::WAIT_ATTACK :
		showMessage_ = false;
		isTutorialAttack_ = true;
		player_->SetAttackEnabled(true);
		for (const auto& ally : Allys_) 
		{if (!ally) continue; if (ally->IsAttacking()) 
			{ step_ = TutorialStep::STEP_32_MESSAGE; break; }
		} 
		
		break;

	case TutorialStep::STEP_32_MESSAGE:
		showMessage_ = true;
		isTutorialAttack_ = false;
		imgCurrentMessage_ = imgTutorialMsg_[32];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_33_MESSAGE;
		}
		break;

	case TutorialStep::STEP_33_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[33];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_34_MESSAGE;
		}
		break;

	case TutorialStep::STEP_34_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[34];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::STEP_35_MESSAGE;
		}
		break;

	case TutorialStep::STEP_35_MESSAGE:
		showMessage_ = true;
		imgCurrentMessage_ = imgTutorialMsg_[35];

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			step_ = TutorialStep::END;
		}
		break;

	case TutorialStep::END:
		SceneManager::GetInstance()
			.ChangeScene(SceneManager::SCENE_ID::STAGE_SELECT);
		break;
	}

	// --- 全チュートリアル共通の制御 ---
	player_->SetControlEnabled(!showMessage_);
	cam.SetControlEnabled(!showMessage_);
}
