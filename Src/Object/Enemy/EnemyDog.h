#pragma once
#include "../AllyBase.h"


class AllyBule : public AllyBase
{
public:

	static constexpr  float SPEED = 7.0f;
	static constexpr  int HP = 1;

	AllyBule();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;
};