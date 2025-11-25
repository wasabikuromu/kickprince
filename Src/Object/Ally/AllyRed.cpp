#include "AllyRed.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"

AllyRed::AllyRed():AllyBase()
{
}

void AllyRed::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Ally/RedAlly.mv1";
	//std::string path = Application::PATH_MODEL + "Enemy/cactus/cactus.mv1";

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

void AllyRed::SetParam(void)
{
	//使用メモリ容量と読み込み時間の削減のため
	//モデルデータをいくつもメモリ上に存在させない
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::ALLY_RED));

	transform_.scl = { ALLY_SIZE,ALLY_SIZE,ALLY_SIZE };				// 大きさの設定
	transform_.quaRotLocal = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f)
			,AsoUtility::Deg2RadF(DEGREE), 0.0f);//クォータニオンをいじると向きが変わる
	transform_.dir = { AsoUtility::VECTOR_ZERO };						// 右方向に移動する

	attackPow_ = ATTACK_POWER;

	speed_ = SPEED;		//移動スピード

	isAlive_ = true;	//初期は生存状態

	hp_ = HP;	//HPの設定

	collisionRadius_ = COLLOSION_RADIUS;	//衝突判定用の球体半径
	collisionLocalPos_ = COLLISION_POS	;	//衝突判定用の球体中心の調整座標

	attackCollisionRadius_ = ATTACK_RADIUS_SIZE;		//攻撃判定用と攻撃範囲の球体半径

	// 初期状態
	ChangeState(STATE::IDLE);
}

void AllyRed::UpdateAttack(void)
{
	if (!initFall_)
	{
		//攻撃開始時点で落下速度初期化
		initFall_ = true;
	}

	//ゆっくり落下処理
	const float gravity = 0.3f;          //通常よりかなり小さい重力
	velocity_.y -= gravity;              //下方向に加速
	transform_.pos.y += velocity_.y;     //位置に反映

	//地面との接地判定
	if (transform_.pos.y < defaultPos_.y)
	{
		transform_.pos.y = defaultPos_.y;
		velocity_.y = 0.0f;
	}

	//アニメーション終了で次の状態に遷移
	if (animationController_->IsEnd() || state_ != STATE::ATTACK) {
		isAttack_ = false;
		ChangeState(STATE::IDLE);

		//数秒後にカメラ復帰予約
		shouldReturnCamera_ = true;
		returnCameraTimer_ = 2.0f;
	}
}

void AllyRed::CollisionAttack(void)
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
				isAttack_ = false;
			}
		}
	}

	//攻撃が終わったら次回用にリセット
	if (anim.step > ATTACK_END)
	{
		isAttack_ = true;
	}
}