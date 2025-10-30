#pragma once
#include "../AllyBase.h"


class AllyBule : public AllyBase
{
public:

	static constexpr  float SPEED = 7.0f;
	static constexpr  int HP = 1;

	//攻撃の位置オフセット
	static constexpr float ATTACK_FORWARD_OFFSET = 55.0f;	//前方向
	static constexpr float ATTACK_HEIGHT_OFFSET = 100.0f;	//高さ
	static constexpr float ATTACK_RADIUS_SIZE = 255.0f;		//攻撃範囲

	AllyBule();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

protected:

	void UpdateAttack(void) override;
	void CollisionAttack(void) override;

};