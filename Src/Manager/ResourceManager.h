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
		TUTORIAL_TEXT_BG,
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
		PAUSEOPE,
		PAUSEITEM,
		GAMECLEAR,
		CLEARWOLEDBORN,
		PRESS_KEY,
		BACK_GAMECLEAR,
		GAMEOVER,
		DIETREE,
		REPLAY,
		ENDGAME,
		CONFIRM_END,
		YES,
		NO,
		SELECT_YES,
		SELECT_NO,
		OPE_GEAR,
		POWER_UP_ICON,
		SPEED_UP_ICON,
		ROTA_ATTACK_ICON,
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
		FALL_PLANET,
		FLAT_PLANET_01,
		FLAT_PLANET_02,
		LAST_PLANET,
		SPECIAL_PLANET,
		FOOT_SMOKE,
		WARP_STAR,
		WARP_STAR_ROT_EFF,
		WARP_ORBIT,
		BLACK_HOLE,
		GOAL_STAR,
		TANK_BODY,
		TANK_BARREL,
		TANK_WHEEL,
		//アイテム
		WATER,
		POWER,
		SPEED,
		HEAL,
		MUTEKI,
		ALL,
		//エネミー
		DOG,
		SABO,
		MIMIC,
		MUSH,
		ONION,
		TOGE,
		VIRUS,
		BOSS,
		//エフェクト
		TREE_RANGE,
		EFF_POWER,
		EFF_SPEED,
		EFF_HEAL
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
