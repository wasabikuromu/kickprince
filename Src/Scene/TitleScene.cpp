#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/GravityManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/AnimationController.h"
#include "../Object/SkyDome.h" 
#include "TitleScene.h"

TitleScene::TitleScene(void)
{
	imgTitle_ = -1;
	imgBackTitle_ = -1;
	imgUDCursor_ = -1;

	animationControllerPlayer_ = nullptr;
	animationControllerRed_ = nullptr;
	endLoadFlame_ = true;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	cnt = 0;

	// 重力制御の初期化
	GravityManager::GetInstance().Init();
	// 画像読み込み
	imgTitle_ = resMng_.Load(ResourceManager::SRC::TITLE).handleId_;					//タイトル名前画像
	imgBackTitle_ = resMng_.Load(ResourceManager::SRC::BACK_GROUND_TITLE).handleId_;	//タイトル背景
	imgGameStart_ = resMng_.Load(ResourceManager::SRC::GAME_START).handleId_;			//ゲームスタート
	imgGameEnd_ = resMng_.Load(ResourceManager::SRC::GAME_END).handleId_;				//ゲームを終了
	imgLookRule_ = resMng_.Load(ResourceManager::SRC::LOOK_RULE).handleId_;				//ルールを見る
	imgAbutton_ = resMng_.Load(ResourceManager::SRC::A_BUTTON).handleId_;				//Aボタン
	imgConfirmEnd_ = resMng_.Load(ResourceManager::SRC::CONFIRM_END).handleId_;			//本当に終了しますか？画像

	imgYes_ = resMng_.Load(ResourceManager::SRC::YES).handleId_;						//はい画像
	imgNo_ = resMng_.Load(ResourceManager::SRC::NO).handleId_;							//いいえ画像
	imgYesSel_ = resMng_.Load(ResourceManager::SRC::SELECT_YES).handleId_;				//選択中はい画像
	imgNoSel_ = resMng_.Load(ResourceManager::SRC::SELECT_NO).handleId_;				//選択中いいえ画像

	selectedIndex_ = 0;

	SetUseASyncLoadFlag(true);

	// 音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::TITLE_BGM, Sound::TIMES::LOOP);

	player_.SetModel(resMng_.Load(ResourceManager::SRC::PLAYER).handleId_);
	redAlly_.SetModel(resMng_.Load(ResourceManager::SRC::ALLY_RED).handleId_);
	blueAlly_.SetModel(resMng_.Load(ResourceManager::SRC::ALLY_BLUE).handleId_);
	blackAlly_.SetModel(resMng_.Load(ResourceManager::SRC::ALLY_BLACK).handleId_);
	enemy_.SetModel(resMng_.Load(ResourceManager::SRC::BOSS).handleId_);
	SetUseASyncLoadFlag(false);
	float size;

	// 定点カメラ
	mainCamera->ChangeMode(Camera::MODE::FIXED_POINT);
}

void TitleScene::NewFunction()
{
	//味方達
	//-------------------------------------------------------------------------
	redAlly_.SetModel(resMng_.Load(ResourceManager::SRC::ALLY_RED).handleId_);
	blueAlly_.SetModel(resMng_.Load(ResourceManager::SRC::ALLY_BLUE).handleId_);
	blackAlly_.SetModel(resMng_.Load(ResourceManager::SRC::ALLY_BLACK).handleId_);

	redAlly_.pos = { ALLY_POS_X,ALLY_RED_POS_Y,ALLY_RED_POS_Z };
	blueAlly_.pos = { ALLY_POS_X,ALLY_BLUE_POS_Y,ALLY_BLUE_POS_Z };
	blackAlly_.pos = { ALLY_POS_X,ALLY_BLACK_POS_Y,ALLY_BLACK_POS_Z };

	redAlly_.scl = { ALLY_SIZE,ALLY_SIZE,ALLY_SIZE };
	blueAlly_.scl = { ALLY_SIZE,ALLY_SIZE,ALLY_SIZE };
	blackAlly_.scl = { ALLY_SIZE,ALLY_SIZE,ALLY_SIZE };

	redAlly_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(ALLY_ROT), 0.0f);
	blueAlly_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(ALLY_ROT), 0.0f);
	blackAlly_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(ALLY_ROT), 0.0f);

	redAlly_.Update();
	blueAlly_.Update();
	blackAlly_.Update();
	//-------------------------------------------------------------------------

	//アニメーション
	//-------------------------------------------------------------------------
	animationControllerRedAlly_ = std::make_unique<AnimationController>(redAlly_.modelId);
	animationControllerBlueAlly_ = std::make_unique<AnimationController>(blueAlly_.modelId);
	animationControllerBlackAlly_ = std::make_unique<AnimationController>(blackAlly_.modelId);

	animationControllerRedAlly_->Add(0, Application::PATH_MODEL + "Ally/RedAlly.mv1", ANIMATION_TIME, ALLY_ANIM_NUM);
	animationControllerBlueAlly_->Add(0, Application::PATH_MODEL + "Ally/BuleAlly.mv1", ANIMATION_TIME, ALLY_ANIM_NUM);
	animationControllerBlackAlly_->Add(0, Application::PATH_MODEL + "Ally/BlackAlly.mv1", ANIMATION_TIME, ALLY_ANIM_NUM);

	animationControllerRedAlly_->Play(0, true);
	animationControllerBlueAlly_->Play(0, true);
	animationControllerBlackAlly_->Play(0, true);
	//-------------------------------------------------------------------------

	//プレイヤー
	//-------------------------------------------------------------------------
	player_.SetModel(resMng_.Load(ResourceManager::SRC::PLAYER).handleId_);
	player_.pos = { PLAYER_POS_X, PLAYER_POS_Y,PLAYER_POS_Z };
	player_.scl = { PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE };
	player_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(PLAYER_ROT), 0.0f);
	player_.Update();
	//-------------------------------------------------------------------------

	//アニメーション
	//-------------------------------------------------------------------------
	animationControllerPlayer_ = std::make_unique<AnimationController>(player_.modelId);
	animationControllerPlayer_->Add(0, Application::PATH_MODEL + "NPlayer/PPlayer.mv1", ANIMATION_TIME, PLAYER_ANIM_NUM);
	animationControllerPlayer_->Play(0, true);
	//-------------------------------------------------------------------------

	//敵
	//-------------------------------------------------------------------------
	enemy_.SetModel(resMng_.Load(ResourceManager::SRC::BOSS).handleId_);
	enemy_.pos = { ENEMY_POS_X, ENEMY_POS_Y, ENEMY_POS_Z };
	enemy_.scl = { ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE };
	enemy_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(ENEMY_ROT), 0.0f);
	enemy_.Update();
	//-------------------------------------------------------------------------

	//アニメーション
	//-------------------------------------------------------------------------
	animationControllerRed_ = std::make_unique<AnimationController>(enemy_.modelId);
	animationControllerRed_->Add(0, Application::PATH_MODEL + "Enemy/Boss/Boss.mv1", ANIMATION_TIME, ENEMY_ANIM_NUM);
	animationControllerRed_->Play(0, true);
	//-------------------------------------------------------------------------
}

void TitleScene::Update(void)
{
	cnt++;
	InputManager& ins = InputManager::GetInstance();

	//終了確認中の入力処理
	if (isConfirmingExit_)
	{
		// アニメーション進行
		if (confirmAnimFrame_ < CONFIRM_ANIM_DURATION) {
			confirmAnimFrame_++;
		}

		if (confirmAnimFrame_ >= CONFIRM_ANIM_DURATION) {
			// 入力受付はアニメーション終了後に
			if (ins.IsTrgDown(KEY_INPUT_LEFT)
				|| ins.IsTrgDown(KEY_INPUT_RIGHT)
				|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_RIGHT)
				|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_LEFT)) {
				SoundManager::GetInstance().Play(SoundManager::SRC::CURSOR_MOVE_SE, Sound::TIMES::ONCE);
				confirmIndex_ = 1 - confirmIndex_;
			}
			if (ins.IsTrgDown(KEY_INPUT_RETURN)||
				ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
				SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::ONCE);

				if (confirmIndex_ == 0) {
					Application::isRunning_ = false;
				}
				else {
					isConfirmingExit_ = false;
				}
			}
		}
		return;
	}

	//点滅更新
	blinkFrameCount_++;
	if (blinkFrameCount_ > ONE_SECOND_FRAME) {
		blinkFrameCount_ = 0;
	}

	//メニュー選択操作
	if (ins.IsTrgDown(KEY_INPUT_DOWN)||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_DOWN)) {
		SoundManager::GetInstance().Play(SoundManager::SRC::CURSOR_MOVE_SE, Sound::TIMES::ONCE);
		selectedIndex_ = (selectedIndex_ + 1) % MENU_SELECT;
	}
	else if (ins.IsTrgDown(KEY_INPUT_UP)||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_TOP)) {
		SoundManager::GetInstance().Play(SoundManager::SRC::CURSOR_MOVE_SE, Sound::TIMES::ONCE);
		selectedIndex_ = (selectedIndex_ + 2) % MENU_SELECT;
	}

	if (ins.IsTrgDown(KEY_INPUT_RETURN)||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		if (selectedIndex_ == 0) {
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::STAGE_SELECT);
			SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::ONCE);
		}
		else if (selectedIndex_ == 1) {
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TUTORIAL);
			SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::ONCE);
		}
		else if (selectedIndex_ == 2) {
			isConfirmingExit_ = true;
			confirmIndex_ = 1;
			confirmAnimFrame_ = 0;
			SoundManager::GetInstance().Play(SoundManager::SRC::WARNING_SE, Sound::TIMES::FORCE_ONCE);
		}
	}

	if (GetASyncLoadNum() != 0)
	{
		return;
	}
	if (endLoadFlame_)
	{
		endLoadFlame_ = false;
		NewFunction();
	}

	//キャラクターの移動
	if (!endLoadFlame_) {
		player_.Update();
		redAlly_.Update();
		blueAlly_.Update();
		blackAlly_.Update();
		enemy_.Update();

		animationControllerPlayer_->Update();
		animationControllerRedAlly_->Update();
		animationControllerBlueAlly_->Update();
		animationControllerBlackAlly_->Update();
		animationControllerRed_->Update();
	}
}

void TitleScene::Draw(void)
{
	int centerX = (Application::SCREEN_SIZE_X / 2);

	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, black, true);

	//タイトルロゴ表示
	DrawGraph(0, 0, imgBackTitle_, true);
	int titleW, titleH;
	GetGraphSize(imgTitle_, &titleW, &titleH);
	DrawRotaGraph(IMG_TITLE_WIDTH, IMG_TITLE_HEIGHT, IMG_TITLE_SIZE, 0, imgTitle_, true);

	//ゲームプレイ画像表示
	DrawGraph(IMG_MENU_X, GAME_STATE_Y, imgGameStart_, true);

	//ルールを見る画像表示
	DrawGraph(IMG_MENU_X, GAME_RULE_Y, imgLookRule_, true);

	//ゲームを終了画像表示
	DrawGraph(IMG_MENU_X, GAME_END_Y, imgGameEnd_, true);

	#pragma region		ボタン設定

	const int yGame = BASE_Y;
	const int yRule = BASE_Y + BUTTON_OFFSET;
	const int yExit = BASE_Y + BUTTON_OFFSET * 2;

	int phase = blinkFrameCount_ % ONE_SECOND_FRAME;
	int alpha = (phase < ONE_SECOND_FRAME / 2)
		? (WHITE_ALPHA * phase) / (ONE_SECOND_FRAME / 2)
		: WHITE_ALPHA - (WHITE_ALPHA * (phase - ONE_SECOND_FRAME / 2)) / (ONE_SECOND_FRAME / 2);

	int yPositions[MENU_SELECT] = { yGame, yRule, yExit };
	#pragma endregion

	// メニュー描画
	for (int i = 0; i < MENU_SELECT; ++i) {
		if (selectedIndex_ == i) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		}

		if (selectedIndex_ == i) {
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

	//GetNowCount() = 経過ミリ秒
	float alpha2 = (sinf(GetNowCount() * BLINK_SPEED) + 1.0f) * 0.5f;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(alpha2 * WHITE_ALPHA));

	//カーソル描画
	DrawRotaGraph(
		CURSOR_WIDTH,
		CURSOR_HEIGHT + (selectedIndex_ * INDEX),
		IMG_CURSOR_SIZE,
		0,
		imgAbutton_,
		true
	);

	//ブレンドモード解除
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


	//テキスト
	if (GetASyncLoadNum() != 0)
	{
		return;
	}

	// モデル描画
	if (!endLoadFlame_) {
		MV1DrawModel(player_.modelId);	
		MV1DrawModel(redAlly_.modelId);
		MV1DrawModel(blueAlly_.modelId);
		MV1DrawModel(blackAlly_.modelId);
		MV1DrawModel(enemy_.modelId);
	}

	if (isConfirmingExit_) {
		float t = static_cast<float>(confirmAnimFrame_) / CONFIRM_ANIM_DURATION;
		if (t > 1.0f) t = 1.0f;

		int alpha = static_cast<int>(GRAY_ALPHA * t);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, black, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawGraph(END_STRING_WIDTH, END_STRING_HEIGHT, imgConfirmEnd_, true);

		// 選択中で画像を切り替え
		if (confirmIndex_ == 0)
		{
			//はい選択中
			DrawGraph(YES_STRING_WIDTH, YES_STRING_HEIGHT, imgYesSel_, true);
			DrawGraph(NO_STRING_WIDTH, NO_STRING_HEIGHT, imgNo_, true);
		}
		else
		{
			//いいえ選択中
			DrawGraph(YES_STRING_WIDTH, YES_STRING_HEIGHT, imgYes_, true);
			DrawGraph(NO_STRING_WIDTH, NO_STRING_HEIGHT, imgNoSel_, true);
		}
	}
}

void TitleScene::Release(void)
{
	if (player_.modelId != -1)
	{
		MV1DeleteModel(player_.modelId);
		player_.modelId = -1;
	}

	SoundManager::GetInstance().Stop(SoundManager::SRC::TITLE_BGM);
	SoundManager::GetInstance().Stop(SoundManager::SRC::SET_SE);
	SoundManager::GetInstance().Stop(SoundManager::SRC::WARNING_SE);
}
