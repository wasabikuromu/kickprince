#pragma once
#include <memory>
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class SceneManager;
class SkyDome;
class AnimationController;

class TitleScene : public SceneBase
{
public:

	//----------------------------------------------------------------------------------
	//味方
	static constexpr float ALLY_POS_X = -350.0f;		//味方のX座標

	static constexpr float ALLY_RED_POS_Y = -350.0f;	//赤味方のY座標
	static constexpr float ALLY_RED_POS_Z = 100.0f;		//赤味方のZ座標
	static constexpr float ALLY_BLUE_POS_Y = -250.0f;	//青味方のY座標
	static constexpr float ALLY_BLUE_POS_Z = 300.0f;	//青味方のZ座標
	static constexpr float ALLY_BLACK_POS_Y = -25.0f;	//黒味方のY座標
	static constexpr float ALLY_BLACK_POS_Z = 400.0f;	//黒味方のZ座標

	static constexpr float ALLY_SIZE = 1.0f;			//味方の大きさ

	static constexpr float ALLY_ROT = -80.0f;			//味方の向き

	static constexpr int ALLY_ANIM_NUM = 4;				//味方の行動アニメーション数

	//プレイヤー
	static constexpr float PLAYER_POS_X = -400.0f;		//プレイヤーのX座標
	static constexpr float PLAYER_POS_Y = -75.0f;		//プレイヤーのY座標
	static constexpr float PLAYER_POS_Z = 0.0f;			//プレイヤーのZ座標

	static constexpr float PLAYER_SIZE = 0.015f;		//プレイヤーの大きさ

	static constexpr float PLAYER_ROT = -80.0f;			//プレイヤーの向き

	static constexpr int PLAYER_ANIM_NUM = 1;			//プレイヤーの行動アニメーション数

	//敵
	static constexpr float ENEMY_POS_X = 400.0f;		//敵のX座標
	static constexpr float ENEMY_POS_Y = -230.0f;		//敵のY座標
	static constexpr float ENEMY_POS_Z = 0.0f;			//敵のZ座標

	static constexpr float ENEMY_SIZE = 0.35;			//敵の大きさ

	static constexpr float ENEMY_ROT = 80.0f;			//敵の向き

	static constexpr int ENEMY_ANIM_NUM = 2;			//敵の行動アニメーション数

	//アニメーション速度
	static constexpr float ANIMATION_TIME = 25.0f;
	//----------------------------------------------------------------------------------

	static constexpr int ONE_SECOND_FRAME = 60;			//1秒

	static constexpr int MENU_SELECT = 3;				//メニュー数

	static constexpr float IMG_TITLE_SIZE = 1.0f;		//タイトル画像の大きさ
	static constexpr int IMG_TITLE_WIDTH = 1100;		//タイトル画像のX
	static constexpr int IMG_TITLE_HEIGHT = 250;		//タイトル画像のY

	static constexpr int IMG_MENU_X = 740;				//メニューのX

	static constexpr int GAME_STATE_Y = 730;			//ゲームプレイ画像のY
	static constexpr int GAME_RULE_Y = 830;				//ゲームルール画像のY	
	static constexpr int GAME_END_Y = 930;				//ゲーム終了画像のY

	static constexpr int WHITE_ALPHA = 255;				//ホワイト
	static constexpr int GRAY_ALPHA = 220;				//グレー

	static constexpr int BASE_Y = 450;					//ボタンのベース場所
	static constexpr int BUTTON_OFFSET = 120;			//ボタンのオフセット位置

	static constexpr int CURSOR_WIDTH = 710;			//カーソルのX
	static constexpr int CURSOR_HEIGHT = 800;			//カーソルのY

	static constexpr int INDEX = 100;					//下にずらす部分
	static constexpr float IMG_CURSOR_SIZE = 0.55;		//カーソルの大きさ

	static constexpr int END_STRING_WIDTH = 400;		//ゲームを終了するかの画像のX
	static constexpr int END_STRING_HEIGHT = 120;		//ゲームを終了するかの画像のY

	static constexpr int YES_STRING_WIDTH = 220;		//はい画像X
	static constexpr int YES_STRING_HEIGHT = 640;		//はい画像Y

	static constexpr int NO_STRING_WIDTH = 990;			//いいえ画像X
	static constexpr int NO_STRING_HEIGHT = 640;		//いいえ画像Y

	static constexpr float BLINK_SPEED = 0.003f;

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int red = 0xff0000;	  //赤
	int green = 0x00ff00; //緑
	int blue = 0x0000ff;  //青
	int yellow = 0xffff00;//黄
	int purpl = 0x800080; //紫

	//コンストラクタ
	TitleScene(void);

	//デストラクタ
	~TitleScene(void);

	void Init(void) override;
	void NewFunction();
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:
	int cnt;
	int cnt_3D;

	//画像
	int imgTitle_;
	int imgBackTitle_;
	int imgGameStart_;
	int imgGameEnd_;
	int imgLookRule_;
	int imgUDCursor_;
	int imgConfirmEnd_;
	int imgYes_;
	int imgNo_;
	int imgYesSel_;
	int imgNoSel_;
	int imgEnd_;
	int imgAbutton_;
	int imgP2_[2];

	int selectedIndex_;

	int blinkFrameCount_ = 0;

	//終了確認用
	bool isConfirmingExit_ = false;		
	int confirmIndex_ = 1;				
	int confirmAnimFrame_ = 0;				
	const int CONFIRM_ANIM_DURATION = 20;

	//アニメーション
	Transform player_;		//プレイヤー
	Transform redAlly_;		//赤味方
	Transform blueAlly_;	//青味方
	Transform blackAlly_;	//黒味方
	Transform enemy_;		//敵
	
	//アニメーションコントローラー
	std::unique_ptr<AnimationController> animationControllerPlayer_;
	std::unique_ptr<AnimationController> animationControllerRedAlly_;
	std::unique_ptr<AnimationController> animationControllerBlueAlly_;
	std::unique_ptr<AnimationController> animationControllerBlackAlly_;
	std::unique_ptr<AnimationController> animationControllerRed_;

	bool endLoadFlame_;
};