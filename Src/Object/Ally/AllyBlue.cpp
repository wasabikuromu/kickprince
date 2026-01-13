#include <EffekseerForDXLib.h>
#include "AllyBlue.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/SoundManager.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"

AllyBule::AllyBule() :AllyBase()
{
}

void AllyBule::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Ally/BuleAlly.mv1";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);

	animationController_->Add((int)ANIM_TYPE::NONE,		path, ANIM_SPEED, ANIM_T_POSE_INDEX);
	animationController_->Add((int)ANIM_TYPE::IDLE,		path, ANIM_SPEED, ANIM_IDLE_INDEX);
	animationController_->Add((int)ANIM_TYPE::WALK,		path, ANIM_SPEED, ANIM_WALK_INDEX);
	animationController_->Add((int)ANIM_TYPE::RUN,		path, ANIM_SPEED, ANIM_RUN_INDEX);
	animationController_->Add((int)ANIM_TYPE::ATTACK,	path, ANIM_SPEED, ANIM_ATTACK_INDEX);
	animationController_->Add((int)ANIM_TYPE::SKY,		path, ANIM_SPEED, ANIM_SKY_INDEX);
	animationController_->Add((int)ANIM_TYPE::ROLL,		path, ANIM_SPEED, ANIM_ROLL_INDEX);

	animationController_->Play((int)ANIM_TYPE::RUN);
}

void AllyBule::SetParam(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ALLY_BLUE));

	transform_.scl = { ALLY_SIZE,ALLY_SIZE,ALLY_SIZE };

	transform_.quaRotLocal = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f)
		, AsoUtility::Deg2RadF(DEGREE), 0.0f);

	transform_.dir = { AsoUtility::VECTOR_ZERO };

	//チャージエフェクト
	effectAttackResId_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::EFF_BLUE_ATK).handleId_;

	hp_ = HP;

	speed_ = SPEED;

	attackPow_ = ATTACK_POWER;

	isAlive_ = true;

	collisionRadius_ = COLLOSION_RADIUS;				//衝突判定用の球体半径
	collisionLocalPos_ = COLLISION_POS;					//衝突判定用の球体中心の調整座標

	attackCollisionRadius_ = ATTACK_RADIUS_SIZE;		//攻撃判定用と攻撃範囲の球体半径

	//初期状態
	ChangeState(STATE::IDLE);
}

void AllyBule::EffectAttack(void)
{
	//エフェクト再生
	effectAttackPlayId_ = PlayEffekseer3DEffect(effectAttackResId_);

	//エフェクトの大きさ
	SetScalePlayingEffekseer3DEffect(effectAttackPlayId_, EFFECT_SIZE, EFFECT_SIZE, EFFECT_SIZE);

	//エフェクトの位置
	SetPosPlayingEffekseer3DEffect(effectAttackPlayId_,
		transform_.pos.x, transform_.pos.y + EFFECT_Y_OFFSET, transform_.pos.z + EFFECT_Z_OFFSET);

	//エフェクトの回転
	SetRotationPlayingEffekseer3DEffect(effectAttackPlayId_, 0.0f, AsoUtility::Deg2RadF(DEGREE), 0.0f);
}

void AllyBule::UpdateIdle(void)
{
	//攻撃後の落下処理
	if (initFall_)
	{
		velocity_.y -= ANIMATION_OUT_GRAVITY;
		transform_.pos.y += velocity_.y;

		//地面まで落下
		if (transform_.pos.y <= defaultPos_.y)
		{
			transform_.pos.y = defaultPos_.y;
			velocity_.y = 0.0f;
			initFall_ = false;
		}
	}

	//Idle中のアニメーション
	if (!initFall_)
	{
		animationController_->Play((int)ANIM_TYPE::IDLE);
	}
}

void AllyBule::UpdateAttack(void)
{
	const auto& anim = animationController_->GetPlayAnim();

	//攻撃中は空中で静止
	velocity_.y = 0.0f;

	//アニメーション途中でショット発射
	if (isAttack_ && anim.step >= ATTACK_FIRE_FRAME && anim.step < ATTACK_FIRE_FRAME + 1)
	{
		VECTOR forward = transform_.quaRot.GetForward();
		VECTOR spawnPos = transform_.pos;
		spawnPos.y += ATTACK_HEIGHT_OFFSET;

		auto shot = std::make_unique<Shot>(spawnPos, forward, Shot::DEFAULT_SPEED, attackPow_);

		//一番近い敵を狙う
		shot->SetTarget(*enemy_);
		shots_.push_back(std::move(shot));

		isAttack_ = false;
	}
	
	//ショット更新
	for (auto& s : shots_)
		s->Update();

	//敵との当たり判定
	for (auto& s : shots_)
	{
		for (const auto& enemy : *enemy_)
		{
			if (s->CheckCollision(enemy.get()))
				break;
		}
	}

	//死亡ショット削除
	shots_.erase(
		std::remove_if(shots_.begin(), shots_.end(),
			[](auto& s) { return s->IsDead(); }),
		shots_.end()
	);

	//攻撃アニメーション終了時
	if (animationController_->IsEnd())
	{
		initFall_ = true;			
		velocity_.y = 0.0f;					

		SetActionFinished(true);

		isAttack_ = false;
		ChangeState(STATE::IDLE);

		shouldReturnCamera_ = true;
		returnCameraTimer_ = RETURN_CAMERA_TIME;
	}
}

void AllyBule::CollisionAttack(void)
{
	if (state_ != STATE::ATTACK) return;

	//アニメーションのステップ数を取得
	const auto& anim = animationController_->GetPlayAnim();

	//攻撃範囲設定
	VECTOR forward = transform_.quaRot.GetForward();
	attackCollisionPos_ = VAdd(transform_.pos, VScale(forward, ATTACK_FORWARD_OFFSET));
	attackCollisionPos_.y += ATTACK_HEIGHT_OFFSET;
	
	if (anim.step >= ATTACK_START && anim.step <= ATTACK_END)
	{
		SoundManager::GetInstance().Play(SoundManager::SRC::BLUE_ATK, Sound::TIMES::FORCE_ONCE);
	}

	//エネミーとの衝突判定
	if (anim.step >= ATTACK_START && anim.step <= ATTACK_END && isAttack_)
	{
		//EffectAttack();

		for (const auto& enemy : *enemy_)
		{
			if (!enemy || !enemy->IsAlive()) continue;

			VECTOR enemyPos = enemy->GetCollisionPos();
			float enemyRadius = enemy->GetCollisionRadius();

			if (AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, enemyPos, enemyRadius))
			{
				enemy->Damage(attackPow_);
				isAttack_ = false;
				break;
			}
		}
	}

	//攻撃が終わったら次回用にリセット
	if (anim.step > ATTACK_END) 
	{
		isAttack_ = true;
	}
}

void AllyBule::DrawShots(void)
{
    for (auto& s : shots_)
        s->Draw();
}


