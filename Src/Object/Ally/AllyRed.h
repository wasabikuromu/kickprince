#pragma once
#include "../AllyBase.h"

class AllyRed : public AllyBase
{
public:

	static constexpr  float SPEED = 6.0f;
	static constexpr  int HP = 1;

	//攻撃の位置オフセット
	static constexpr float ATTACK_FORWARD_OFFSET = 200.0f;	// 前方向
	static constexpr float ATTACK_HEIGHT_OFFSET = 100.0f;	// 高さ
	static constexpr float ATTACK_RADIUS_SIZE = 200.0f;		// 攻撃範囲

	//攻撃判定フレーム
	const float ATTACK_START = 30.0f;
	const float ATTACK_END = 35.0f;

	//攻撃力
	const int ATTACK_POWER = 5;

	AllyRed();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

	//エフェクト
	void EffectAttack(void) override;

protected:

	void UpdateAttack(void) override;
	void CollisionAttack(void) override;

};
