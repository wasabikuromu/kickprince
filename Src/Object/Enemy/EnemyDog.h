#pragma once
#include "../EnemyBase.h"

class EnemyDog : public EnemyBase
{
public:

	static constexpr int HP = 6;

	EnemyDog();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

};

