#include "EnemyVirus.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"

EnemyVirus::EnemyVirus() :EnemyBase()
{
}

void EnemyVirus::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Enemy/virus/virus.mv1";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);

	animationController_->Add((int)ANIM_TYPE::IDLE,		path, ANIM_SPEED, ANIM_IDLE_INDEX);
	animationController_->Add((int)ANIM_TYPE::RUN,		path, ANIM_SPEED, ANIM_RUN_INDEX);
	animationController_->Add((int)ANIM_TYPE::ATTACK,	path, ANIM_SPEED, ANIM_ATTACK_INDEX);
	animationController_->Add((int)ANIM_TYPE::DAMAGE,	path, ANIM_SPEED, ANIM_DAMAGE_INDEX);
	animationController_->Add((int)ANIM_TYPE::DEATH,	path, ANIM_SPEED, ANIM_DEATH_INDEX);

	animationController_->Play((int)ANIM_TYPE::RUN);
}

void EnemyVirus::SetParam(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::VIRUS));

	transform_.scl = { VIRUS_SIZE, VIRUS_SIZE, VIRUS_SIZE };			

	transform_.quaRotLocal = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f)
		,AsoUtility::Deg2RadF(0.0f), 0.0f);

	transform_.dir = { AsoUtility::VECTOR_ZERO };						

	hp_ = HP;

	speed_ = 0.0f;

	isAlive_ = true;

	collisionRadius_ = COLLOSION_RADIUS;			//Õ“Ë”»’è—p‚Ì‹…‘Ì”¼Œa
	collisionLocalPos_ = COLLISION_POS;				//Õ“Ë”»’è—p‚Ì‹…‘Ì’†S‚Ì’²®À•W

	attackCollisionRadius_ = ATTACK_RADIUS_SIZE;	//UŒ‚”»’è—p‚ÆUŒ‚”ÍˆÍ‚Ì‹…‘Ì”¼Œa

	enemyType_ = TYPE::VIRUS;

	//‰Šúó‘Ô
	ChangeState(STATE::PLAY);
}
