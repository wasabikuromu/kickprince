#pragma once
#include "../EnemyBase.h"

class EnemyVirus : public EnemyBase
{
public:

	//HP
	static constexpr int HP = 2;		

	//サイズ
	static constexpr float VIRUS_SIZE = 2.5f;
	
	//当たり判定位置
	static constexpr VECTOR COLLISION_POS = { 0.0f, 300.0f, 0.0f };

	EnemyVirus();

	//アニメーションロード用
	void InitAnimation(void) override;

	//パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

};
