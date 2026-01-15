#pragma once
#include "../AllyBase.h"

class AllyBlack : public AllyBase
{
public:

	//スピード
	static constexpr float	SPEED = 4.0f;

	//HP
	static constexpr int HP = 1;

	//攻撃の位置オフセット
	static constexpr float ATTACK_FORWARD_OFFSET = 55.0f;	//前方向
	static constexpr float ATTACK_HEIGHT_OFFSET = 100.0f;	//高さ
	static constexpr float ATTACK_RADIUS_SIZE = 150.0f;		//攻撃範囲

	//攻撃判定フレーム
	const float ATTACK_START = 47.0f;
	const float ATTACK_END = 55.0f;

	//攻撃力
	const int ATTACK_POWER = 10;

	//エフェクトサイズ
	const float EFFECT_SIZE = 30.0f;

	//エフェクトの位置オフセット
	static constexpr float EFFECT_Y_OFFSET = 130.0f;
	static constexpr float EFFECT_Z_OFFSET = 200.0f;

	AllyBlack();

	//アニメーションロード用
	void InitAnimation(void) override;

	//パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

	//エフェクト
	void EffectAttack(void) override;

protected:

	void UpdateAttack(void) override;
	void CollisionAttack(void) override;

	void StartAttack(void);

	bool isAttackEffectPlayed_ = false;
};
