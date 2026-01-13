#pragma once
#include "../EnemyBase.h"

class EnemyDog : public EnemyBase
{
public:

	//HP
	static constexpr int HP = 10;

	//サイズ
	static constexpr float DOG_SIZE = 2.0f;

	//当たり判定位置
	static constexpr VECTOR COLLISION_POS = { 0.0f, 60.0f , 0.0f };		

	EnemyDog();

	//アニメーションロード用
	void InitAnimation(void) override;

	//パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

};

