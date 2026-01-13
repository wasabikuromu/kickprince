#include "EnemyOnion.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"

EnemyOnion::EnemyOnion() :EnemyBase()
{
}

void EnemyOnion::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Enemy/onion/onion.mv1";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);

	animationController_->Add((int)ANIM_TYPE::IDLE,		path, ANIM_SPEED, ANIM_IDLE_INDEX);
	animationController_->Add((int)ANIM_TYPE::RUN,		path, ANIM_SPEED, ANIM_RUN_INDEX);
	animationController_->Add((int)ANIM_TYPE::ATTACK,	path, ANIM_SPEED, ANIM_ATTACK_INDEX);
	animationController_->Add((int)ANIM_TYPE::DAMAGE,	path, ANIM_SPEED, ANIM_DAMAGE_INDEX);
	animationController_->Add((int)ANIM_TYPE::DEATH,	path, ANIM_SPEED, ANIM_DEATH_INDEX);

	animationController_->Play((int)ANIM_TYPE::RUN);
}

void EnemyOnion::SetParam(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ONION));

	transform_.scl = { ONION_SIZE, ONION_SIZE, ONION_SIZE };

	transform_.quaRotLocal = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f), AsoUtility::Deg2RadF(0.0f), 0.0f);

	transform_.dir = { AsoUtility::VECTOR_ZERO };	

	hp_ = HP;

	speed_ = 0.0f;

	isAlive_ = true;

	collisionRadius_ = COLLOSION_RADIUS;			//衝突判定用の球体半径
	collisionLocalPos_ = COLLISION_POS;				//衝突判定用の球体中心の調整座標

	attackCollisionRadius_ = ATTACK_RADIUS_SIZE;	//攻撃判定用と攻撃範囲の球体半径

	enemyType_ = TYPE::ONION;

	//初期状態
	ChangeState(STATE::PLAY);
}