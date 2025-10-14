#pragma once
#include "../AllyBase.h"


class AllyBlack : public AllyBase
{
public:

	static constexpr  float SPEED = 4.0f;
	static constexpr  int HP = 1;

	AllyBlack();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;
};
