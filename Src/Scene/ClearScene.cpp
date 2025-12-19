#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Common/AnimationController.h"
#include "ClearScene.h"

ClearScene::ClearScene(void)
{
	imgClear_ = -1;
	imgBackGameClaer_ = -1;
	imgClearWolrd_ = -1;
	imgReplay_ = -1;

	cheackCounter_ = 0;

	animationControllerRed_ = nullptr;
	animationControllerBlue_ = nullptr;
	animationControllerBlack_ = nullptr;
}

ClearScene::~ClearScene(void)
{
}

void ClearScene::Init(void)
{

	// 画像読み込み
	imgClear_ = resMng_.Load(ResourceManager::SRC::GAMECLEAR).handleId_;
	imgBackGameClaer_ = resMng_.Load(ResourceManager::SRC::BACK_GAMECLEAR).handleId_;
	imgClearWolrd_ = resMng_.Load(ResourceManager::SRC::CLEARWOLEDBORN).handleId_;
	imgReplay_ = resMng_.Load(ResourceManager::SRC::REPLAY).handleId_;

	imgReplay_ = LoadGraph("Data/Image/GameOver/Replay.png");
	imgSelectReplay_ = LoadGraph("Data/Image/GameOver/SelectRePlay.png");
	imgBackTitle_ = LoadGraph("Data/Image/GameOver/BackTitle.png");
	imgSelectBackTitle_ = LoadGraph("Data/Image/GameOver/SelectBackTitle.png");

	cheackCounter_ = 0;
	selectedIndex_ = 0;

	// 音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::GAMECLEAR_BGM, Sound::TIMES::LOOP);

	//アニメーション用です
	//---------------------------------------------
	//メッセージ画像のサイズと位置取得
	//画像サイズ取得

	GetGraphSize(imgClearWolrd_, &imgW_, &imgH_);
	messageX_ = Application::SCREEN_SIZE_X / VALUE_TWO - imgW_ / VALUE_TWO;
	messageY_ = MESSAGE_Y;

	//マスクの初期位置（完全に隠れている状態）
	maskLeftX_ = messageX_;
	maskSpeed_ = MASK_SPEED;

	//フェード処理
	clearAlpha_ = 0;
	fadeSpeed_ = FADE_SPEED;

	//presskey用
	pressKeyY_ = Application::SCREEN_SIZE_Y + PRESS_KEY_OFFSET_Y;
	targetPressKeyY_ = PRESS_KEY_TARGET_Y;
	pressKeyAlpha_ = 0;	
	isPressKeyAnimStart_ = false;
	isPressKeyAnimEnd_ = false;

	//プレイヤー
	player_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::PLAYER));
	player_.pos = { PLAYER_POS };
	player_.scl = { PLAYER_SIZE };
	player_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(0.0f), 0.0f);
	player_.Update();

	//プレイヤーのアニメーション
	std::string path = Application::PATH_MODEL + "NPlayer/";
	animationControllerPlayer_ = std::make_unique<AnimationController>(player_.modelId);
	animationControllerPlayer_->Add(0, path + "PPlayer.mv1", ANIM_SPEED, 5);
	animationControllerPlayer_->Play(0);

	//赤
	redAlly_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::ALLY_RED));
	redAlly_.pos = RED_ALLY_INIT_POS;
	redAlly_.scl = ALLY_INIT_SCL;
	redAlly_.quaRot = Quaternion::Euler(90.0f, AsoUtility::Deg2RadF(ALLY_INIT_ROT_Y_DEG), 0.0f);
	redAlly_.Update();

	//赤い味方のアニメーション
	std::string path1 = Application::PATH_MODEL + "Ally/";
	animationControllerRed_ = std::make_unique<AnimationController>(redAlly_.modelId);
	animationControllerRed_->Add(0, path1 + "RedAlly.mv1", -ALLY_INIT_ROT_Y_DEG, 5);
	animationControllerRed_->Play(0);

	//青
	blueAlly_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::ALLY_BLUE));
	blueAlly_.pos = BLUE_ALLY_INIT_POS;
	blueAlly_.scl = ALLY_INIT_SCL;
	blueAlly_.quaRot = Quaternion::Euler(90.0f, AsoUtility::Deg2RadF(ALLY_INIT_ROT_Y_DEG), 0.0f);
	blueAlly_.Update();

	//青い味方のアニメーション
	animationControllerBlue_ = std::make_unique<AnimationController>(blueAlly_.modelId);
	animationControllerBlue_->Add(0, path1 + "BuleAlly.mv1", -ALLY_INIT_ROT_Y_DEG, 5);
	animationControllerBlue_->Play(0);

	//黒
	blackAlly_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::ALLY_BLACK));
	blackAlly_.pos = BLACK_ALLY_INIT_POS;
	blackAlly_.scl = ALLY_INIT_SCL;
	blackAlly_.quaRot = Quaternion::Euler(90.0f, AsoUtility::Deg2RadF(ALLY_INIT_ROT_Y_DEG), 0.0f);
	blackAlly_.Update();

	//黒い味方のアニメーション
	animationControllerBlack_ = std::make_unique<AnimationController>(blackAlly_.modelId);
	animationControllerBlack_->Add(0, path1 + "BlackAlly.mv1", -ALLY_INIT_ROT_Y_DEG, 5);
	animationControllerBlack_->Play(0);
	isAnimEnd_ = false;

	// 定点カメラ
	mainCamera->ChangeMode(Camera::MODE::FIXED_POINT);
}

void ClearScene::Update(void)
{
	InputManager& ins = InputManager::GetInstance();
	cheackCounter_++;

	// アニメーション更新
	if (maskLeftX_ < messageX_ + imgW_) {
		maskLeftX_ += maskSpeed_;
		if (maskLeftX_ > messageX_ + imgW_) {
			maskLeftX_ = messageX_ + imgW_;
		}
	}

	if (clearAlpha_ < ALPHA_MAX) {
		clearAlpha_ += fadeSpeed_;
		if (clearAlpha_ > ALPHA_MAX) clearAlpha_ = ALPHA_MAX;
	}

	// アニメーション終了チェック
	if (!isAnimEnd_ && maskLeftX_ >= messageX_ + imgW_ && clearAlpha_ >= ALPHA_MAX) {
		isAnimEnd_ = true;
		isPressKeyAnimStart_ = true;
	}

	if (cheackCounter_ >= MENU_SELECT_TIME)
	{
		if (ins.IsTrgDown(KEY_INPUT_UP) || ins.IsTrgDown(KEY_INPUT_DOWN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_TOP) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_DOWN)) {
			SoundManager::GetInstance().Play(SoundManager::SRC::CURSOR_MOVE_SE, Sound::TIMES::ONCE);
			selectedIndex_ = 1 - selectedIndex_;
		}

		if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::ONCE);
			if (selectedIndex_ == 0) {
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::STAGE_SELECT);
			}
			else {
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
		}
	}

	// 強制遷移
	if (cheackCounter_ >= AUTO_RETURN_FRAME) {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	redAlly_.Update();
	blueAlly_.Update();
	blackAlly_.Update();

	player_.Update();
	animationControllerPlayer_->Update();
	animationControllerRed_->Update();
	animationControllerBlue_->Update();
	animationControllerBlack_->Update();
}

void ClearScene::Draw(void)
{
	// 背景を描く
	DrawGraph(0, 0, imgBackGameClaer_, true);

	DrawRotaGraph(Application::SCREEN_SIZE_X/ VALUE_TWO, CLEAR_IMG_Y, CLEAR_IMG_SCALE, 0,imgClear_,true);

	// メッセージの上に背景で覆う（横方向）
	int horizontalMaskW = messageX_ + imgW_ - maskLeftX_;
	if (horizontalMaskW > 0) {
		DrawRectGraph(
			maskLeftX_, messageY_,							//表示先（画面上）
			maskLeftX_, messageY_, horizontalMaskW, imgH_,	//背景画像の一部
			imgBackGameClaer_, TRUE, FALSE
		);
	}

	if (cheackCounter_ >= MENU_SELECT_TIME)
	{
		if (selectedIndex_ % 2 == 1)
		{
			DrawGraph(MENU_POS_X, MENU_PLAY_Y, imgReplay_, true);
			DrawGraph(MENU_POS_X, MENU_TITLE_Y, imgSelectBackTitle_, true);
		}
		else
		{
			SetFontSize(MENU_FONT_SIZE);
			DrawGraph(MENU_POS_X, MENU_PLAY_Y, imgSelectReplay_, true);
			DrawGraph(MENU_POS_X, MENU_TITLE_Y, imgBackTitle_, true);
		}
	}

	//モデル
	MV1DrawModel(redAlly_.modelId);
	MV1DrawModel(blueAlly_.modelId);
	MV1DrawModel(blackAlly_.modelId);

	MV1DrawModel(player_.modelId);
}

void ClearScene::Release(void)
{
	DeleteGraph(imgClear_);
	DeleteGraph(imgBackGameClaer_);
	DeleteGraph(imgClearWolrd_);
	DeleteGraph(imgReplay_);


	SoundManager::GetInstance().Stop(SoundManager::SRC::GAMECLEAR_BGM);
}
