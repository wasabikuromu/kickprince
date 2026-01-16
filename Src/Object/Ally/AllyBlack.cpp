#include <EffekseerForDXLib.h>
#include "AllyBlack.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/SoundManager.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"

AllyBlack::AllyBlack():AllyBase()
{
}

void AllyBlack::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Ally/BlackAlly.mv1";

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

void AllyBlack::SetParam(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ALLY_BLACK));
	
	transform_.scl = { ALLY_SIZE,ALLY_SIZE,ALLY_SIZE };

	transform_.quaRotLocal = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f)
		,AsoUtility::Deg2RadF(DEGREE), 0.0f);

	transform_.dir = { AsoUtility::VECTOR_ZERO };						

	//チャージエフェクト
	effectAttackResId_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::EFF_BLACK_ATK).handleId_;

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

void AllyBlack::EffectAttack(void)
{
	//エフェクト再生
	effectAttackPlayId_ = PlayEffekseer3DEffect(effectAttackResId_);

	//エフェクトの大きさ
	SetScalePlayingEffekseer3DEffect(effectAttackPlayId_, EFFECT_SIZE, EFFECT_SIZE, EFFECT_SIZE);

	//エフェクトの位置
	SetPosPlayingEffekseer3DEffect(effectAttackPlayId_,
		transform_.pos.x, transform_.pos.y + EFFECT_Y_OFFSET, transform_.pos.z + EFFECT_Z_OFFSET);
}

void AllyBlack::UpdateAttack(void)
{
	if (!initFall_)
	{
		//攻撃開始時点で落下速度初期化
		initFall_ = true;
	}

	//ゆっくり落下処理
	velocity_.y -= ANIMATION_OUT_GRAVITY;				//下方向に加速
	transform_.pos.y += velocity_.y;					//位置に反映

	if (transform_.pos.y < defaultPos_.y)
	{
		transform_.pos.y = defaultPos_.y;
		velocity_.y = 0.0f;
	}

	//アニメーション終了で次の状態に遷移
	if (animationController_->IsEnd()) {
		SetActionFinished(true);
		isAttack_ = false;
		ChangeState(STATE::IDLE);

		shouldReturnCamera_ = true;
		returnCameraTimer_ = RETURN_CAMERA_TIME;
	}
}

void AllyBlack::CollisionAttack(void)
{
	if (state_ != STATE::ATTACK) return;

	//アニメーションのステップ数を取得
	const auto& anim = animationController_->GetPlayAnim();

	//攻撃範囲設定
	VECTOR forward = transform_.quaRot.GetForward();
	attackCollisionPos_ = VAdd(transform_.pos, VScale(forward, ATTACK_FORWARD_OFFSET));
	attackCollisionPos_.y += ATTACK_HEIGHT_OFFSET;

	//エネミーとの衝突判定
	if (anim.step >= ATTACK_START && anim.step <= ATTACK_END && isAttack_ && !isAttackEffectPlayed_)
	{
		EffectAttack();
		SoundManager::GetInstance().Play(SoundManager::SRC::BLACK_ATK, Sound::TIMES::FORCE_ONCE);

		isAttackEffectPlayed_ = true;

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

void AllyBlack::StartAttack(void)
{
	isAttack_ = true;
	isAttackEffectPlayed_ = false;
}

