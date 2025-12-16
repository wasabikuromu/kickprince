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

// 3D座標をスクリーン座標に変換
bool OverScene::WorldToScreen(const VECTOR& worldPos, VECTOR& screenPos)
{
	// 画面サイズ
	int screenW = Application::SCREEN_SIZE_X;
	int screenH = Application::SCREEN_SIZE_Y;

	// X座標を画面Xにマッピング（中央を画面中央に）
	screenPos.x = screenW / VALUE_TWO + static_cast<int>(worldPos.x);

	// Z座標を画面Yにマッピング（手前が下、適当にY軸調整）
	// Yはそのまま画面の下方向に使う例
	screenPos.y = screenH / VALUE_TWO - static_cast<int>(worldPos.z);

	// 簡易判定として画面内ならtrue
	if (screenPos.x < 0 || screenPos.x > screenW || screenPos.y < 0 || screenPos.y > screenH) {
		return false;
	}
	return true;
}

OverScene::OverScene(void)
{
	imgGameOver_ = -1;
	imgLightCircle_ = -1;

	animationController_ = nullptr;

	selectedIndex_ = 0;
	isMenuActive_ = false;

	maskLeftX_ = 0;
	maskRightX_ = 0;

	cheackCounter_ = 0;
}

OverScene::~OverScene(void)
{
}

void OverScene::Init(void)
{
	// 重力制御の初期化
	GravityManager::GetInstance().Init();

	// 画像読み込み
	imgGameOver_ = resMng_.Load(ResourceManager::SRC::GAMEOVER).handleId_;
	imgDieTree_ = resMng_.Load(ResourceManager::SRC::DIETREE).handleId_;
	imgLightCircle_ = resMng_.Load(ResourceManager::SRC::LIGHT).handleId_;

	imgReplay_ = LoadGraph("Data/Image/GameOver/Replay.png");
	imgSelectReplay_ = LoadGraph("Data/Image/GameOver/SelectRePlay.png");
	imgBackTitle_ = LoadGraph("Data/Image/GameOver/BackTitle.png");
	imgSelectBackTitle_ = LoadGraph("Data/Image/GameOver/SelectBackTitle.png");

	selectedIndex_ = 0;

	cheackCounter_ = 0;

	isMenuActive_ = false;

	SoundManager::GetInstance().Play(SoundManager::SRC::GAMEOVER_BGM, Sound::TIMES::ONCE);

	// キャラ
	player_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::PLAYER));
	player_.pos = { PLAYER_POS };
	player_.scl = { PLAYER_SIZE };
	player_.quaRot = Quaternion::Euler(0.0f, AsoUtility::Deg2RadF(0.0f), 0.0f);
	player_.Update();

	// アニメーションの設定
	std::string path = Application::PATH_MODEL + "NPlayer/";
	animationController_ = std::make_unique<AnimationController>(player_.modelId);
	animationController_->Add(0, path + "PPlayer.mv1", ANIM_SPEED,6);
	animationController_->Play(0);

	// 定点カメラ
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
			selectedIndex_ = 1 - selectedIndex_;
		}

		if (ins.IsTrgDown(KEY_INPUT_RETURN)||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
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
	// 背景真っ黒
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, black, true);

	// ゲームオーバー画像
	DrawGraph(Application::SCREEN_SIZE_X / VALUE_TWO - GAMEOVER_IMG_X_OFFSET, GAMEOVER_IMG_Y, imgGameOver_, true);

	// プレイヤーモデル描画
	MV1DrawModel(player_.modelId);

	// 光の描画（プレイヤー周辺）
	VECTOR screenPos;
	if (WorldToScreen(player_.pos, screenPos))
	{
		int alpha = LIGHT_ALPHA;
		SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);

		int w, h;
		GetGraphSize(imgLightCircle_, &w, &h);

		float scaleX = LIGHT_SCALE_X;
		float scaleY = LIGHT_SCALE_Y;

		int drawW = static_cast<int>(w * scaleX);
		int drawH = static_cast<int>(h * scaleY);
		int offsetY = LIGHT_OFFSET_Y;

		DrawExtendGraph(
			(int)screenPos.x - drawW / VALUE_TWO,
			(int)screenPos.y + offsetY - drawH / VALUE_TWO,
			(int)screenPos.x + drawW / VALUE_TWO,
			(int)screenPos.y + offsetY + drawH / VALUE_TWO,
			imgLightCircle_,
			TRUE
		);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (cheackCounter_ >= MENU_SELECT_TIME)
	{
		if (selectedIndex_ %2 == 1)
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