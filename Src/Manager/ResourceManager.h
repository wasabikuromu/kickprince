#pragma once
#include <memory>
#include <map>
#include <string>
#include "Resource.h"

class ResourceManager
{

public:

	// リソース名
	enum class SRC
	{
		//画像(UI)
		TITLE,
		BACK_GROUND_TITLE,
		GAME_START,
		GAME_END,
		LOOK_RULE,
		A_BUTTON,
		ENTER,
		PUSH,
		PLZ_SELECT_STAGE,
		STAGE_1,
		STAGE_2,
		STAGE_3,
		STAGE_4,
		STAGE_5,
		BOSS_IMG,
		//チュートリアル
		TUTORIAL_MSG_01,
		TUTORIAL_MSG_02,
		TUTORIAL_MSG_03,
		TUTORIAL_MSG_04,
		TUTORIAL_MSG_05,
		TUTORIAL_MSG_06,
		TUTORIAL_MSG_07,
		TUTORIAL_MSG_08,
		TUTORIAL_MSG_09,
		TUTORIAL_MSG_10,
		TUTORIAL_MSG_11,
		TUTORIAL_MSG_12,
		TUTORIAL_MSG_13,
		TUTORIAL_MSG_14,
		TUTORIAL_MSG_15,
		TUTORIAL_MSG_16,
		TUTORIAL_MSG_17,
		TUTORIAL_MSG_18,
		TUTORIAL_MSG_19,
		TUTORIAL_MSG_20,
		TUTORIAL_MSG_21,
		TUTORIAL_MSG_22,
		TUTORIAL_MSG_23,
		TUTORIAL_MSG_24,
		TUTORIAL_MSG_25,
		TUTORIAL_MSG_26,
		TUTORIAL_MSG_27,
		TUTORIAL_MSG_28,
		TUTORIAL_MSG_29,
		TUTORIAL_MSG_30,
		TUTORIAL_MSG_31,
		TUTORIAL_MSG_32,
		TUTORIAL_MSG_33,
		TUTORIAL_MSG_34,
		TUTORIAL_MSG_35,
		//ポーズ
		PAUSE_1,
		PAUSE_2,
		PAUSE_3,
		PAUSE_4,
		PAUSE_5,
		//ポーズ選択用画像
		SELECT_PAUSE_1,
		SELECT_PAUSE_2,
		SELECT_PAUSE_3,
		SELECT_PAUSE_4,
		SELECT_PAUSE_5,
		SELECT_UI,
		GAUGE_FRAME,
		CURSOR,
		PLAY,
		RULE,
		GAMEUI_1,
		NICE_KICK,
		NEXT_STAGE,
		SELECT_STAGE,
		BACK_TITLE,
		UPDOWN,
		GOTITLE,
		GOGAME,
		ITEMTEACH,
		OPERATION,
		PAUSEOPE1,
		PAUSEOPE2,
		PAUSEALLY,
		GAMECLEAR,
		CLEARWOLEDBORN,
		PRESS_KEY,
		BACK_GAMECLEAR,
		GAMEOVER,
		REPLAY,
		ENDGAME,
		CONFIRM_END,
		YES,
		NO,
		SELECT_YES,
		SELECT_NO,
		OPE_GEAR_KEYBOARD,
		OPE_GEAR_CONTROLLER,
		//3Dモデル
		ALLY_RED,
		ALLY_BLUE,
		ALLY_BLACK,
		SPEECH_BALLOON,
		PLAYER,
		SHADOW,
		LIGHT,
		SKY_DOME,
		MAIN_PLANET,
		FOOT_SMOKE,
		//エネミー
		DOG,
		ONION,
		VIRUS,
		BOSS,
		//エフェクト
		EFF_CHARGE,
		EFF_RED_ATK,
		EFF_BLUE_ATK,
		EFF_BLACK_ATK,
	};

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ResourceManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 解放(シーン切替時に一旦解放)
	void Release(void);

	// リソースの完全破棄
	void Destroy(void);

	// リソースのロード
	const Resource& Load(SRC src);

	// リソースの複製ロード(モデル用)
	int LoadModelDuplicate(SRC src);

private:

	// 静的インスタンス
	static ResourceManager* instance_;

	// リソース管理の対象
	std::map<SRC, std::unique_ptr<Resource>> resourcesMap_;

	// 読み込み済みリソース
	std::map<SRC, Resource&> loadedMap_;

	Resource dummy_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = default;
	~ResourceManager(void) = default;

	// 内部ロード
	Resource& _Load(SRC src);

};
