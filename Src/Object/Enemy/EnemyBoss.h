#pragma once
#include "../EnemyBase.h"

class EnemyBoss : public EnemyBase
{
public:

	//ボスの最大HP
	static constexpr int BOSS_MAX_HP = 15;

	//HPバー表示
	static constexpr int BOSS_HP_BAR_WIDTH = 600;
	static constexpr int BOSS_HP_BAR_HEIGHT = 20;
	static constexpr int BOSS_HP_BAR_Y = 80;
	static constexpr int BOSS_LABEL_OFFSET_X = -120;
	static constexpr int BOSS_LABEL_OFFSET_Y = -20;

	//サイズ
	static constexpr float DOG_SIZE = 0.8f;

	//ボスの球体座標
	static constexpr float COLLOSION_RADIUS = 500.0f;

	//当たり判定位置
	static constexpr VECTOR COLLISION_POS = { 0.0f, 300.0f , 0.0f };

	//アニメーション番号
	static constexpr int  BOSS_ANIM_IDLE_INDEX = 2;
	static constexpr int  BOSS_ANIM_RUN_INDEX = 4;
	static constexpr int  BOSS_ANIM_ATTACK_INDEX = 0;
	static constexpr int  BOSS_ANIM_DEATH_INDEX = 1;

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int red = 0xff0000;	  //赤

	EnemyBoss();

	//アニメーションロード用
	void InitAnimation(void) override;

	//パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

	//HPバー
	void DrawBossHpBar(void) override;
};
