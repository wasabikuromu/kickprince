#pragma once
#include <memory>
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class SceneManager;
class SkyDome;
class AnimationController;

class OverScene : public SceneBase
{
public:

	static constexpr float VALUE_TWO = 2.0f;

	//ゲームオーバー文字列
	static constexpr int GAMEOVER_IMG_X_OFFSET = 555;
	static constexpr int GAMEOVER_IMG_Y = 50;

	//プレイヤーモデル
	static constexpr VECTOR PLAYER_POS = { 0.0f, -200.0f, 0.0f };
	static constexpr VECTOR PLAYER_SIZE = { 0.018f, 0.018f, 0.018f };

	//アニメーションスピード
	static constexpr float ANIM_SPEED = 20.0f;

	//アニメーションナンバー
	static constexpr int ANIM_NUM = 6;

	//メニュー文字
	static constexpr int MENU_FONT_SIZE = 55;
	static constexpr int MENU_PLAY_Y = 800;
	static constexpr int MENU_TITLE_Y = 890;
	static constexpr int MENU_POS_X = 1470;

	//時間制御
	static constexpr int MENU_SELECT_TIME = 420;
	static constexpr int FORCE_TITLE_TIME = 3600;

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int yellow = 0xffff00;//黄

	OverScene(void);	// コンストラクタ
	~OverScene(void);	// デストラクタ

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;
private:
	int cnt;

	//画像
	int imgGameOver_;
	int imgCursor_[2];
	int imgReplay_;
	int imgSelectReplay_;
	int imgBackTitle_;
	int imgSelectBackTitle_;

	//メニュー
	int selectedIndex_;       //選択中のメニューインデックス
	bool isMenuActive_;

	//カウンター
	int cheackCounter_;

	static constexpr int maskWidthMax_ = 1600;  //画像横幅
	static constexpr int revealSpeed_ = 4;

	//キャラクター
	Transform player_;

	//アニメーション
	std::unique_ptr<AnimationController> animationController_;
};