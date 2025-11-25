#include "AllyBlue.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"



AllyBule::AllyBule() :AllyBase()
{
}

void AllyBule::InitAnimation(void)
{

	std::string path = Application::PATH_MODEL + "Ally/BuleAlly.mv1";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);

	animationController_->Add((int)ANIM_TYPE::NONE, path, ANIM_SPEED, ANIM_T_POSE_INDEX);
	animationController_->Add((int)ANIM_TYPE::IDLE, path, ANIM_SPEED, ANIM_IDLE_INDEX);
	animationController_->Add((int)ANIM_TYPE::WALK, path, ANIM_SPEED, ANIM_RUN_INDEX);
	animationController_->Add((int)ANIM_TYPE::RUN, path, ANIM_SPEED, ANIM_RUN_INDEX);
	animationController_->Add((int)ANIM_TYPE::ATTACK, path, ANIM_SPEED, ANIM_ATTACK_INDEX);
	animationController_->Add((int)ANIM_TYPE::SKY, path, ANIM_SPEED, ANIM_SKY_INDEX);
	animationController_->Add((int)ANIM_TYPE::ROLL, path, ANIM_SPEED, ANIM_ROLL_INDEX);

	animationController_->Play((int)ANIM_TYPE::RUN);
}

void AllyBule::SetParam(void)
{
	// 使用メモリ容量と読み込み時間の削減のため
	// モデルデータをいくつもメモリ上に存在させない
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ALLY_BLUE));

	transform_.scl = { ALLY_SIZE,ALLY_SIZE,ALLY_SIZE };				// 大きさの設定
	transform_.quaRotLocal = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f)
		, AsoUtility::Deg2RadF(DEGREE), 0.0f);//クォータニオンをいじると向きが変わる
	transform_.dir = { AsoUtility::VECTOR_ZERO };						// 右方向に移動する

	speed_ = SPEED;		// 移動スピード

	attackPow_ = ATTACK_POWER;

	isAlive_ = true;	// 初期は生存状態

	hp_ = HP;	// HPの設定

	collisionRadius_ = COLLOSION_RADIUS;	// 衝突判定用の球体半径
	collisionLocalPos_ = COLLISION_POS;	// 衝突判定用の球体中心の調整座標

	attackCollisionRadius_ = ATTACK_RADIUS_SIZE;		// 攻撃判定用と攻撃範囲の球体半径

	// 初期状態
	ChangeState(STATE::IDLE);
}

void AllyBule::UpdateIdle(void)
{
	//攻撃後の落下処理
	if (initFall_)
	{
		const float gravity = 0.3f;
		velocity_.y -= gravity;
		transform_.pos.y += velocity_.y;

		// 地面まで落下
		if (transform_.pos.y <= defaultPos_.y)
		{
			transform_.pos.y = defaultPos_.y;
			velocity_.y = 0.0f;
			initFall_ = false; // 落下完了
		}
	}

	// Idle中のアニメーション（再生中に落下がないときは普通に待機）
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
	if (animationController_->IsEnd() || state_ != STATE::ATTACK)
	{
		initFall_ = true;					// ← ここで落下開始！
		velocity_.y = 0.0f;					// 落下初速度
		ChangeState(STATE::IDLE);			// Idleに戻して落下処理へ
		shouldReturnCamera_ = true;
		returnCameraTimer_ = 2.0f;
		isAttack_ = true;
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
	
	//エネミーとの衝突判定
	//攻撃可能フレーム範囲内 かつ isAttack_ が true のときのみ処理
	if (anim.step >= ATTACK_START && anim.step <= ATTACK_END && isAttack_)
	{
		for (const auto& enemy : *enemy_)
		{
			if (!enemy || !enemy->IsAlive()) continue;

			VECTOR enemyPos = enemy->GetCollisionPos();
			float enemyRadius = enemy->GetCollisionRadius();

			if (AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, enemyPos, enemyRadius))
			{
				enemy->Damage(attackPow_);
				isAttack_ = false;  // 1回だけにする
				break;
			}
		}
	}

	// 攻撃が終わったら次回用にリセット
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


