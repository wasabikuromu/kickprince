#pragma once
#include "../AllyBase.h"

class AllyRed : public AllyBase
{
public:

	static constexpr  float SPEED = 6.0f;
	static constexpr  int HP = 1;

	AllyRed();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;
};
