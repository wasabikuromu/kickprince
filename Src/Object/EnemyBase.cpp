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

	// 状態管理
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

	is1damage = false;
	is2damage = false;
	is4damage = false;
	is8damage = false;
	is16damage = false;
	is32damage = false;
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

	// 更新ステップ
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
		//AttackCollisionPos();
	}
}

void EnemyBase::UpdatePlay(void)
{
	if (!isAlive_)
	{
		return;
	}

	// 衝突判定
	Collision();
}

//void EnemyBase::UpdateAttack(void)
//{
//	animationController_->Play((int)ANIM_TYPE::ATTACK, false);
//
//	// 攻撃タイミング
//	if (!isAttack_ && isAttack_P)
//	{
//		isAttack_ = true; // 多重ヒット防止用フラグ
//		isAttack_P = false;
//	}
//	else if (!isAttack_ && isAttack_T)
//	{
//		isAttack_ = true;
//		isAttack_T = false;
//	}
//
//	 //アニメーション終了で次の状態に遷移
//	if (animationController_->IsEnd() || state_ != STATE::ATTACK) {
//		isAttack_ = false;
//		//CollisionAttack();
//		ChangeState(STATE::IDLE);
//	}
//}

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

	// モデル反映
	MV1SetScale(transform_.modelId, transform_.scl);
	MV1SetPosition(transform_.modelId, transform_.pos);

	MV1DrawModel(transform_.modelId);

	//デッバグ
	//DrawDebug();

	// 視野範囲の描画
	//DrawDebugSearchRange();
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

	if (damage == 1)is1damage=true;
	if (damage == 2)is2damage=true;
	if (damage == 4)is4damage=true;
	if (damage == 8)is8damage=true;
	if (damage == 16)is16damage=true;
	if (damage == 32)is32damage=true;
}

#pragma region コリジョン

void EnemyBase::Collision(void)
{
	// 現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	// 移動
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

//void EnemyBase::AttackCollisionPos(void)
//{
//	//プレイヤーとの衝突判定
//	// 攻撃の方向（エネミー）
//	VECTOR forward = transform_.quaRot.GetForward();
//	// 攻撃の開始位置と終了位置
//	attackCollisionPos_ = VAdd(transform_.pos, VScale(forward, ATTACK_FORWARD_OFFSET));
//	attackCollisionPos_.y += ATTACK_HEIGHT_OFFSET;  // 攻撃の高さ調整
//
//	//プレイヤーを見る
//	EnemyToPlayer();
//}
//
//void EnemyBase::EnemyToPlayer(void)
//{
//	//プレイヤーの当たり判定とサイズ
//	playerCenter_ = player_->GetCollisionPos();
//	playerRadius_ = player_->GetCollisionRadius();
//
//	if (AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, playerCenter_, playerRadius_)
//			&& player_->pstate_ != Player::PlayerState::DOWN)
//	{
//		isAttack_P = true;
//		ChangeState(STATE::ATTACK);
//	}
//	else if (!AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, playerCenter_, playerRadius_)
//		|| player_->pstate_ == Player::PlayerState::DOWN)
//	{
//		ChangeState(STATE::PLAY);
//	}
//}
//
//void EnemyBase::CollisionAttack(void)
//{
//	//プレイヤーの当たり判定とサイズ
//	playerCenter_ = player_->GetCollisionPos();
//	playerRadius_ = player_->GetCollisionRadius();
//
//	if(AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_,playerCenter_, playerRadius_))
//	{
//		player_->Damage(attackPow_);
//	}
//
//	for (const auto& ally : *ally_)
//	{
//		if (!ally || !ally->IsAlive()) continue;
//
//		//敵の当たり判定とサイズ
//		VECTOR allyPos = ally->GetCollisionPos();
//		float allyRadius = ally->GetCollisionRadius();
//
//		//球体同士の当たり判定
//		if (AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, allyPos, allyRadius))
//		{
//			//ally_->Damage(attackPow_);
//			//1体のみヒット
//			break;
//		}
//	}
//}

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
	// 状態変更
	state_ = state;

	// 各状態遷移の初期処理
	stateChanges_[state_]();
}

void EnemyBase::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdateNone, this);
}

void EnemyBase::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdateIdle, this);
}
void EnemyBase::ChangeStatePlay(void)
{
	stateUpdate_ = std::bind(&EnemyBase::UpdatePlay, this);
}

//void EnemyBase::ChangeStateAttack(void)
//{
//	stateUpdate_ = std::bind(&EnemyBase::UpdateAttack, this);
//}

void EnemyBase::ChangeStateDamage(void)
{
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
	//// キャラ座標
	v = transform_.pos;
	DrawFormatString(20, 120, white, "キャラ座標 ： (%0.2f, %0.2f, %0.2f)",v.x, v.y, v.z);

	s = collisionPos_;
	DrawSphere3D(s, collisionRadius_, 8, black, black, false);
	DrawFormatString(20, 280, white, "enemyスフィア座標 ： (%0.2f, %0.2f, %0.2f)",s.x, s.y, s.z);
	DrawFormatString(20, 210, white, "エネミーの移動速度 ： %0.2f", speed_);
	
	a = attackCollisionPos_;
	DrawSphere3D(a, attackCollisionRadius_, 8, yellow, yellow, false);

}