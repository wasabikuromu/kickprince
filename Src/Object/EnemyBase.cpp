#include <DxLib.h>
#include <string>
#include <vector>
#include "../Application.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/SoundManager.h"
#include "../Scene/GameScene.h"
#include "../Utility/AsoUtility.h"
#include "Common/AnimationController.h"
#include "ActorBase.h"
#include "Player.h"
#include "AllyBase.h"
#include "EnemyBase.h"

EnemyBase::EnemyBase() 
	: 
	gScene_(nullptr),
	movePow_(AsoUtility::VECTOR_ZERO)
{
	animationController_ = nullptr;

	state_ = STATE::NONE;

	attackPow_ = VALUE_ONE;	//攻撃力

	//状態管理
	stateChanges_.emplace(
		STATE::NONE, std::bind(&EnemyBase::ChangeStateNone, this));
	stateChanges_.emplace(
		STATE::IDLE, std::bind(&EnemyBase::ChangeStateIdle, this));
	stateChanges_.emplace(
		STATE::PLAY, std::bind(&EnemyBase::ChangeStatePlay, this));
	stateChanges_.emplace(
		STATE::DAMAGE, std::bind(&EnemyBase::ChangeStateDamage, this));
	stateChanges_.emplace(
		STATE::DEATH, std::bind(&EnemyBase::ChangeStateDeath, this));
	stateChanges_.emplace(
		STATE::DEAD_END, std::bind(&EnemyBase::ChangeStateDeadEnd, this));
}

EnemyBase::~EnemyBase(void)
{
	MV1DeleteModel(transform_.modelId);
}

void EnemyBase::Init(void)
{
 	SetParam();
	InitAnimation();

	dCount = 0;

	animationController_->Play((int)ANIM_TYPE::IDLE, true);
}

void EnemyBase::Update(void)
{
	if (!isAlive_)
	{
		return;
	}

	transform_.Update();

	//アニメーション再生
	animationController_->Update();

	//更新ステップ
	if (stateUpdate_)
	{
		stateUpdate_();
	}
}

#pragma region StateごとのUpdate

void EnemyBase::UpdateIdle(void)
{
	animationController_->Play((int)ANIM_TYPE::IDLE, false);
	if (animationController_->IsEnd() || state_ != STATE::IDLE)
	{
		
	}
}

void EnemyBase::UpdatePlay(void)
{
	if (!isAlive_)
	{
		return;
	}

	//衝突判定
	Collision();
}

void EnemyBase::UpdateDamage(void)
{
	animationController_->Play((int)ANIM_TYPE::DAMAGE, false);
	if (animationController_->IsEnd())
	{
		ChangeState(STATE::IDLE);
	}
}

void EnemyBase::UpdateDeath(void)
{
	animationController_->Play((int)ANIM_TYPE::DEATH, false);

	if (animationController_->IsEnd())
	{
		isAlive_ = false;
		ChangeState(STATE::DEAD_END);
	}
}

void EnemyBase::UpdateDeadEnd(void)
{
}

#pragma endregion


void EnemyBase::Draw(void)
{
	if (!isAlive_)
	{
		return;
	}

	Collision();

	//モデル反映
	MV1SetScale(transform_.modelId, transform_.scl);
	MV1SetPosition(transform_.modelId, transform_.pos);

	MV1DrawModel(transform_.modelId);

	//DrawDebug();
}

void EnemyBase::Release(void)
{
	MV1DeleteModel(transform_.modelId);
}

void EnemyBase::SetPos(VECTOR pos)
{
	transform_.pos = pos;
}

EnemyBase::STATE EnemyBase::GetState(void)
{
	return state_;
}

bool EnemyBase::IsAlive(void)
{
	return isAlive_;
}

void EnemyBase::SetAlive(bool alive)
{
	isAlive_ = alive;
}

EnemyBase::TYPE EnemyBase::GetEnemyType(void) const
{
	return enemyType_;
}

void EnemyBase::Damage(int damage)
{
	hp_ -= damage;
	if (hp_ <= 0)
	{
		hp_ = 0;
	}

	SoundManager::GetInstance().Play(SoundManager::SRC::E_DAMAGE_SE, Sound::TIMES::FORCE_ONCE);
	isAttack_ = false;

	if (hp_ <= 0 && isAlive_)
	{
		ChangeState(STATE::DEATH);
		SoundManager::GetInstance().Play(SoundManager::SRC::E_DOWN_SE, Sound::TIMES::ONCE);
	}
	else if (hp_ > 0 && isAlive_ && enemyType_ != TYPE::BOSS)
	{
		ChangeState(STATE::DAMAGE);
	}
}

#pragma region コリジョン

void EnemyBase::Collision(void)
{
	//現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	//移動
	moveDiff_ = VSub(movedPos_, transform_.pos);
	transform_.pos = movedPos_;

	collisionPos_ = VAdd(transform_.pos, collisionLocalPos_);
}

void EnemyBase::SetCollisionPos(const VECTOR collision)
{
	collisionPos_ = collision;
}

VECTOR EnemyBase::GetCollisionPos(void)const
{
	return VAdd(collisionLocalPos_, transform_.pos);
}

float EnemyBase::GetCollisionRadius(void)
{
	return collisionRadius_;
}

bool EnemyBase::IsDeadFinished(void)
{
	return !isAlive_ && state_ == STATE::DEAD_END;
}
#pragma endregion

void EnemyBase::SetGameScene(GameScene* gscene)
{
	gScene_ = gscene;
}

void EnemyBase::SetTutorialScene(TutorialScene* tscene)
{
	tScene_ = tscene;
}

#pragma region Stateの切り替え

void EnemyBase::ChangeState(STATE state)
{
	//状態変更
	state_ = state;

	//各状態遷移の初期処理
	stateChanges_[state_]();
}

void EnemyBase::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdateNone, this);
}

void EnemyBase::ChangeStateIdle(void)
{
	animationController_->Play((int)ANIM_TYPE::IDLE, true);
	stateUpdate_ = std::bind(&EnemyBase::UpdateIdle, this);
}
void EnemyBase::ChangeStatePlay(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdatePlay, this);
}

void EnemyBase::ChangeStateDamage(void)
{
	animationController_->Play((int)ANIM_TYPE::DAMAGE, false);
	stateUpdate_ = std::bind(&EnemyBase::UpdateDamage, this);
}

void EnemyBase::ChangeStateDeath(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdateDeath, this);
}

void EnemyBase::ChangeStateDeadEnd(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdateDeadEnd, this);
}

#pragma endregion

void EnemyBase::SetPlayer(std::shared_ptr<Player> player)
{
	player_ = player;
}

void EnemyBase::SetAlly(const std::vector<std::shared_ptr<AllyBase>>* ally)
{
	ally_ = ally;
}

void EnemyBase::DrawDebug(void)
{
	VECTOR v;
	VECTOR s;
	VECTOR a;

	// キャラ基本情報
	//-------------------------------------------------------
	// キャラ座標
	v = transform_.pos;
	DrawFormatString(20, 120, white, "キャラ座標 ： (%0.2f, %0.2f, %0.2f)",v.x, v.y, v.z);

	s = collisionPos_;
	DrawSphere3D(s, collisionRadius_, 8, black, black, false);
	DrawFormatString(20, 280, white, "enemyスフィア座標 ： (%0.2f, %0.2f, %0.2f)",s.x, s.y, s.z);
	DrawFormatString(20, 210, white, "エネミーの移動速度 ： %0.2f", speed_);
	
	a = attackCollisionPos_;
	DrawSphere3D(a, attackCollisionRadius_, 8, yellow, yellow, false);

}