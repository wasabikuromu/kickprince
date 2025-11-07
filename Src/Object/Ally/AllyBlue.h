#pragma once
#include "../AllyBase.h"
#include "../shot.h"

class AllyBule : public AllyBase
{
public:

	static constexpr  float SPEED = 7.0f;
	static constexpr  int HP = 1;
	static constexpr float ATTACK_FIRE_FRAME = 30;

	//攻撃の位置オフセット
	static constexpr float ATTACK_FORWARD_OFFSET = 55.0f;	//前方向
	static constexpr float ATTACK_HEIGHT_OFFSET = 100.0f;	//高さ
	static constexpr float ATTACK_RADIUS_SIZE = 30.0f;		//攻撃範囲

	//攻撃判定フレーム
	const float ATTACK_START = 40.0f;
	const float ATTACK_END = 50.0f;

	AllyBule();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

	

protected:

	void UpdateIdle(void) override;
	void UpdateAttack(void) override;
	void CollisionAttack(void) override;
	void DrawShots(void) override;

private:

	std::vector<std::unique_ptr<Shot>> shots_;
};