#pragma once
#include "../EnemyBase.h"

class EnemyOnion : public EnemyBase
{
public:

	//HP
	static constexpr int HP = 5;

	//サイズ
	static constexpr float ONION_SIZE = 2.5f;	

	//当たり判定位置
	static constexpr VECTOR COLLISION_POS = { 0.0f, 60.0f , 0.0f };	

	EnemyOnion();

	//アニメーションロード用
	void InitAnimation(void) override;

	//パラメータ設定(純粋仮想関数)
	void SetParam(void) override;
};
