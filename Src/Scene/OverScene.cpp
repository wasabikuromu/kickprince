#include <string>
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
#include "OverScene.h"

OverScene::OverScene(void)
{
	imgGameOver_ = -1;

	animationController_ = nullptr;

	selectedIndex_ = 0;
	isMenuActive_ = false;

	cheackCounter_ = 0;
}

OverScene::~OverScene(void)
{
}

void OverScene::Init(void)
{
	//重力制御の初期化
	GravityManager::GetInstance().Init();

	//画像読み込み
	imgGameOver_ = resMng_.Load(ResourceManager::SRC::GAMEOVER).handleId_;

	imgReplay_ = LoadGraph("Data/Image/GameOver/Replay.png");
	imgSelectReplay_ = LoadGraph("Data/Image/GameOver/SelectRePlay.png");
	imgBackTitle_ = LoadGraph("Data/Image/GameOver/BackTitle.png");
	imgSelectBackTitle_ = LoadGraph("Data/Image/GameOver/SelectBackTitle.png");

	selectedIndex_ = 0;

	cheackCounter_ = 0;

	isMenuActive_ = false;

	SoundManager::GetInstance().Play(SoundManager::SRC::GAMEOVER_BGM, Sound::TIMES::ONCE);

	//キャラ
	player_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::PLAYER));
	player_.pos = { PLAYER_POS };
	player_.scl = { PLAYER_SIZE };
	player_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(0.0f), 0.0f);
	player_.Update();

	//アニメーションの設定
	std::string path = Application::PATH_MODEL + "NPlayer/";
	animationController_ = std::make_unique<AnimationController>(player_.modelId);
	animationController_->Add(0, path + "PPlayer.mv1", ANIM_SPEED, ANIM_NUM);
	animationController_->Play(0);

	//定点カメラ
	mainCamera->ChangeMode(Camera::MODE::FIXED_POINT);

	cnt = 0;
}

void OverScene::Update(void)
{
	cnt++;
	InputManager& ins = InputManager::GetInstance();

	cheackCounter_++;

	if (cheackCounter_ >= MENU_SELECT_TIME)
	{
		if (ins.IsTrgDown(KEY_INPUT_UP) || ins.IsTrgDown(KEY_INPUT_DOWN)||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_TOP)||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_DOWN)) {
			SoundManager::GetInstance().Play(SoundManager::SRC::CURSOR_MOVE_SE, Sound::TIMES::ONCE);
			selectedIndex_ = 1 - selectedIndex_;
		}

		if (ins.IsTrgDown(KEY_INPUT_RETURN)||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
			SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE , Sound::TIMES::ONCE);
			if (selectedIndex_ == 0) {
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::STAGE_SELECT);
			}
			else {
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
		}
	}

	//一定時間がたてば強制的にタイトルに
	if (cheackCounter_ >= FORCE_TITLE_TIME) {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	animationController_->Update();
}

void OverScene::Draw(void)
{
	//背景真っ黒
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, black, true);

	//ゲームオーバー画像
	DrawGraph(Application::SCREEN_SIZE_X / VALUE_TWO - GAMEOVER_IMG_X_OFFSET, GAMEOVER_IMG_Y, imgGameOver_, true);

	//プレイヤーモデル描画
	MV1DrawModel(player_.modelId);

	if (cheackCounter_ >= MENU_SELECT_TIME)
	{
		if (selectedIndex_ %2 == 1)
		{
			DrawGraph(MENU_POS_X, MENU_PLAY_Y, imgReplay_, true);
			DrawGraph(MENU_POS_X, MENU_TITLE_Y, imgSelectBackTitle_, true);
		}
		else
		{
			DrawGraph(MENU_POS_X, MENU_PLAY_Y, imgSelectReplay_, true);
			DrawGraph(MENU_POS_X, MENU_TITLE_Y, imgBackTitle_, true);
		}
	}
}

void OverScene::Release(void)
{
	if (player_.modelId != -1)
	{
		MV1DeleteModel(player_.modelId);
		player_.modelId = -1;
	}
	SoundManager::GetInstance().Stop(SoundManager::SRC::GAMEOVER_BGM);
}