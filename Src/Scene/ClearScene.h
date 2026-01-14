#pragma once
#include <memory>
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class AnimationController;
class SceneManager;

class ClearScene : public SceneBase
{
public:
	ClearScene(void);	//コンストラクタ
	~ClearScene(void);	//デストラクタ

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;
private:

	static constexpr float VALUE_TWO = 2.0f;

	//プレイヤー
	static constexpr VECTOR PLAYER_POS = { 120.0f, -300.0f, 0.0f };		//プレイヤーの位置
	static constexpr VECTOR PLAYER_SIZE = { 0.018f, 0.018f, 0.018f };	//プレイヤーの大きさ

	//味方
	static constexpr VECTOR RED_ALLY_INIT_POS = { -490.0f, -200.0f, 50.0f };		//赤味方の位置
	static constexpr VECTOR BLUE_ALLY_INIT_POS = { -290.0f, -200.0f, 50.0f };		//青味方の位置
	static constexpr VECTOR BLACK_ALLY_INIT_POS = { -90.0f, -200.0f, 50.0f };		//黒味方の位置

	static constexpr float ALLY_ROT = 90.0f;										//味方の角度
	static constexpr float ALLY_INIT_ROT_Y_DEG = -20.0f;							//味方の角度

	static constexpr VECTOR ALLY_INIT_SCL = { 0.8f, 0.8f, 0.8f };					//味方のサイズ

	//アニメーション用定数
	static constexpr int FADE_SPEED = 2;
	static constexpr float ANIM_SPEED = 32.0f;
	static constexpr int ANIM_NUM = 5;

	//時間制御
	static constexpr int MENU_SELECT_TIME = 420;
	static constexpr int AUTO_RETURN_FRAME = 3600;

	//アルファ値の最大値
	static constexpr int ALPHA_MAX = 255;

	//メニュー文字
	static constexpr int MENU_PLAY_Y = 800;
	static constexpr int MENU_TITLE_Y = 890;
	static constexpr int MENU_POS_X = 1470;

	//Clear画像
	static constexpr int CLEAR_IMG_Y = 200;
	static constexpr float CLEAR_IMG_SCALE = 1.3f;

	//メッセージ
	static constexpr int MESSAGE_FONT_SIZE = 128;
	static constexpr int MESSAGE_X_OFFSET = 5;
	static constexpr float MESSAGE_X_SCALE = 5.5f;

	//画像
	int imgClear_;
	int imgBackGameClaer_;
	int imgClearWolrd_;

	int imgReplay_;
	int imgSelectReplay_;
	int imgBackTitle_;
	int imgSelectBackTitle_;

	//メニュー
	int selectedIndex_;       //選択中のメニューインデックス

	//カウンター
	int cheackCounter_;

	//アニメーション用
	//-------------------------------------
	int clearAlpha_;     //透明度
	int fadeSpeed_;      //フェードインの速度

	//味方キャラクター
	Transform redAlly_;
	Transform blueAlly_;
	Transform blackAlly_;

	//プレイヤーキャラクター
	Transform player_;

	//敵のアニメーション
	std::unique_ptr<AnimationController> animationControllerRed_;
	std::unique_ptr<AnimationController> animationControllerBlue_;
	std::unique_ptr<AnimationController> animationControllerBlack_;

	//プレイヤーのアニメーション
	std::unique_ptr<AnimationController> animationControllerPlayer_;

	//アニメーション終わり
	bool isAnimEnd_;

	//-------------------------------------
};