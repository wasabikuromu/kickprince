#include <DxLib.h>
#include <string>
#include <vector>
#include "../Application.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/GravityManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/SoundManager.h"
#include "../Scene/GameScene.h"
#include "../Utility/AsoUtility.h"
#include "Common/AnimationController.h"
#include "Common/Capsule.h"
#include "ActorBase.h"
#include "../Object/EnemyBase.h"
#include "Player.h"
#include "AllyBase.h"

AllyBase::AllyBase() 
	: 
	scene_(nullptr),
	movePow_(AsoUtility::VECTOR_ZERO),
	isBlowedEnd_(false)   // 初期値
{
	animationController_ = nullptr;

	jumpPow_ = AsoUtility::VECTOR_ZERO;

	state_ = STATE::NONE;

	attackPow_ = VALUE_ONE;	//攻撃力

	//状態管理
	stateChanges_.emplace(
		STATE::NONE, std::bind(&AllyBase::ChangeStateNone, this));
	stateChanges_.emplace(
		STATE::IDLE, std::bind(&AllyBase::ChangeStateIdle, this));
	stateChanges_.emplace(
		STATE::PLAY, std::bind(&AllyBase::ChangeStatePlay, this));
	stateChanges_.emplace(
		STATE::ATTACK, std::bind(&AllyBase::ChangeStateAttack, this));
	stateChanges_.emplace(
		STATE::BLOW, std::bind(&AllyBase::ChangeStateBlow, this));

	//衝突チェック
	gravHitPosDown_ = AsoUtility::VECTOR_ZERO;
	gravHitPosUp_ = AsoUtility::VECTOR_ZERO;
}

AllyBase::~AllyBase(void)
{
}

void AllyBase::Init(void)
{
 	SetParam();
	InitAnimation();

	damageCnt_ = 0;

	//カプセルコライダ
	capsule_ = std::make_unique<Capsule>(transform_);
	capsule_->SetLocalPosTop(CAPSULE_TOP);
	capsule_->SetLocalPosDown(CAPSULE_BOTTOM);
	capsule_->SetRadius(CAPSULE_RADIUS);

	//衝突判定用の球体半径
	collisionRadius_ = COLLISION_RADIUS;

	//衝突判定用の球体中心の調整座標
	collisionLocalPos_ = { 0.0f, capsule_->GetCenter().y, 0.0f };

	//丸影画像
	imgShadow_ = resMng_.Load(
		ResourceManager::SRC::SHADOW).handleId_;
}

void AllyBase::Update(void)
{
	//更新ステップ
	stateUpdate_();

	if (!isAlive_)
	{
		return;
	}

	transform_.Update();

	//アニメーション再生
	animationController_->Update();


}

#pragma region StateごとのUpdate

void AllyBase::UpdateIdle(void)
{
	animationController_->Play((int)ANIM_TYPE::IDLE, true);

	// 吹っ飛び済みなら攻撃処理に戻さない
	if (isBlowedEnd_)
	{
		return; // ずっと待機
	}

	// 通常の敵だけ攻撃判定を見る
	if (animationController_->IsEnd() || state_ != STATE::IDLE)
	{
		AttackCollisionPos();
	}
}

void AllyBase::UpdatePlay(void)
{
	if (!isAlive_)
	{
		return;
	}

	//重力による移動量
	CalcGravityPow();

	//衝突判定
	Collision();

	//攻撃範囲に入ったかを見る
	AttackCollisionPos();
}

void AllyBase::UpdateAttack(void)
{
	animationController_->Play((int)ANIM_TYPE::ATTACK, false);

	//攻撃タイミング
	if (!isAttack_ && isAttack_P)
	{
		isAttack_ = true; //多重ヒット防止用フラグ
		isAttack_P = false;
	}
	else if (!isAttack_ && isAttack_T)
	{
		isAttack_ = true;
		isAttack_T = false;
	}

	 //アニメーション終了で次の状態に遷移
	if (animationController_->IsEnd() || state_ != STATE::ATTACK) {
		isAttack_ = false;
		CheckHitAttackHit();
		ChangeState(STATE::IDLE);
	}
}

void AllyBase::UpdateBlow(void)
{    
	// 吹っ飛びアニメ（飛んでる間ずっと再生）
	animationController_->Play((int)ANIM_TYPE::SKY, true);

	// 重力
	velocity_.y -= 9.8f * 0.1f;

	// 移動
	transform_.pos = VAdd(transform_.pos, velocity_);

	// 落下して地面に着いたら
	if (transform_.pos.y <= 0 && velocity_.y < 0)   // ← 下降中かつ地面に着いた時だけ
	{
		transform_.pos.y = 0;
		velocity_ = AsoUtility::VECTOR_ZERO;
		isBlow_ = false;

		isBlowedEnd_ = true;
		ChangeState(STATE::IDLE);
	}
}
#pragma endregion

void AllyBase::Draw(void)
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
	DrawDamage();

	DrawShadow();

	//デッバグ
	DrawDebug();

	//視野範囲の描画
	//DrawDebugSearchRange();
}

void AllyBase::Release(void)
{
	MV1DeleteModel(transform_.modelId);
}

void AllyBase::SetPos(VECTOR pos)
{
	transform_.pos = pos;
}

AllyBase::STATE AllyBase::GetState(void)
{
	return state_;
}

bool AllyBase::IsAlive(void)
{
	return isAlive_;
}

void AllyBase::SetAlive(bool alive)
{
	isAlive_ = alive;
}

AllyBase::TYPE AllyBase::GetEnemyType(void) const
{
	return enemyType_;
}

void AllyBase::Damage(int damage)
{
	hp_ -= damage;
	SoundManager::GetInstance().Play(SoundManager::SRC::E_DAMAGE_SE, Sound::TIMES::FORCE_ONCE);
	isAttack_ = false;

	if (hp_ <= 0 && isAlive_)
	{
		isBlow_ = true;

		// --- 吹っ飛び方向をZ方向に固定 ---
		// +Z方向（奥）に飛ばす
		VECTOR dir = VGet(0.0f, 0.0f, 1.0f);
		// -Z方向（手前）に飛ばす場合は下の1行に変更
		// VECTOR dir = VGet(0.0f, 0.0f, -1.0f);

		// 前方向の速度（水平スピード）
		VECTOR forwardVel = VScale(dir, 40.0f);

		// 上方向の初速（高さ）
		VECTOR upVel = VGet(0.0f, 60.0f, 0.0f);

		// 合成して最終速度を設定
		velocity_ = VAdd(forwardVel, upVel);

		ChangeState(STATE::BLOW);
	}
}

void AllyBase::DrawDamage()
{

}


#pragma region コリジョン

void AllyBase::Collision(void)
{
	//現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	//衝突(カプセル)
	CollisionCapsule();

	//衝突(重力)
	CollisionGravity();

	//移動
	moveDiff_ = VSub(movedPos_, transform_.pos);
	transform_.pos = movedPos_;

	collisionPos_ = VAdd(transform_.pos, collisionLocalPos_);
}

void AllyBase::CollisionGravity(void)
{
	//ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

	//重力方向
	VECTOR dirGravity = grvMng_.GetDirGravity();

	//重力方向の反対
	VECTOR dirUpGravity = grvMng_.GetDirUpGravity();

	//重力の強さ
	float gravityPow = grvMng_.GetPower();

	float checkPow = 10.0f;

	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));

	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));

	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));

	for (const auto c : colliders_)
	{
		//地面との衝突
		auto hit = MV1CollCheck_Line(
			c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		//if (hit.HitFlag > 0)
		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.9f)
		{
			//衝突地点から、少し上に移動

			//地面と衝突している

			//movedPos_に押し戻し座標を設定
			//押し戻し座標については、dxlib のhit構造体の中にヒントアリ
			//衝突地点情報が格納されている
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));
		}
	}
}

void AllyBase::CollisionCapsule(void)
{
	//カプセルを移動させる
	Transform trans = Transform(transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(*capsule_, trans);
	//カプセルとの衝突判定
	for (const auto c : colliders_)
	{
		auto hits = MV1CollCheck_Capsule(
			c.lock()->modelId_, -1,
			cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());
		//衝突した複数のポリゴンと衝突回避するまで、
		//プレイヤーの位置を移動させる
		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];
			//地面と異なり、衝突回避位置が不明なため、何度か移動させる
			//この時、移動させる方向は、移動前座標に向いた方向であったり、
			//衝突したポリゴンの法線方向だったりする
			for (int tryCnt = 0; tryCnt < 10; tryCnt++)
			{
				//再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
				//最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
				int pHit = HitCheck_Capsule_Triangle(
					cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]);
				if (pHit)
				{
					//法線の方向にちょっとだけ移動させる
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));
					//カプセルも一緒に移動させる
					trans.pos = movedPos_;
					trans.Update();
					continue;
				}
				break;
			}
		}
		//検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
}

void AllyBase::CalcGravityPow(void)
{
	// 重力方向
	VECTOR dirGravity = grvMng_.GetDirGravity();

	// 重力の強さ
	float gravityPow = grvMng_.GetPower();

	// 重力
	// 重力を作る
	// メンバ変数 jumpPow_ に重力計算を行う(加速度)
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

	// 内積
	float dot = VDot(dirGravity, jumpPow_);
	if (dot >= 0.0f)
	{
		// 重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
		jumpPow_ = gravity;
	}
}

void AllyBase::DrawShadow(void)
{
	int i, j;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;
	int ModelHandle;

	//ライティングを無効にする
	SetUseLighting(FALSE);

	//Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	//テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	//影を落とすモデルの数だけ繰り返し
	for (const auto c : colliders_)
	{
		//地面との衝突
		auto hit = MV1CollCheck_Line(
			c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		//プレイヤーの直下に存在する地面のポリゴンを取得
		HitResDim = MV1CollCheck_Capsule(
			c.lock()->modelId_,
			-1,
			transform_.pos,
			VAdd(transform_.pos, VGet(0.0f, -ALLY_SHADOW_HEIGHT, 0.0f)), ALLY_SHADOW_SIZE);

		//頂点データで変化が無い部分をセット
		Vertex[0].dif = GetColorU8(255, 255, 255, 255);
		Vertex[0].spc = GetColorU8(0, 0, 0, 0);
		Vertex[0].su = 0.0f;
		Vertex[0].sv = 0.0f;
		Vertex[1] = Vertex[0];
		Vertex[2] = Vertex[0];

		//球の直下に存在するポリゴンの数だけ繰り返し
		HitRes = HitResDim.Dim;
		for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
		{
			//ポリゴンの座標は地面ポリゴンの座標
			Vertex[0].pos = HitRes->Position[0];
			Vertex[1].pos = HitRes->Position[1];
			Vertex[2].pos = HitRes->Position[2];

			//ちょっと持ち上げて重ならないようにする
			SlideVec = VScale(HitRes->Normal, SHADOW_LIFT);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			//ポリゴンの不透明度を設定する
			Vertex[0].dif.a = 0;
			Vertex[1].dif.a = 0;
			Vertex[2].dif.a = 0;
			if (HitRes->Position[0].y > transform_.pos.y - ALLY_SHADOW_HEIGHT)
				Vertex[0].dif.a = SHADOW_MAX_ALPHA * (1.0f - fabs(HitRes->Position[0].y - transform_.pos.y) / ALLY_SHADOW_HEIGHT);

			if (HitRes->Position[1].y > transform_.pos.y - ALLY_SHADOW_HEIGHT)
				Vertex[1].dif.a = SHADOW_MAX_ALPHA * (1.0f - fabs(HitRes->Position[1].y - transform_.pos.y) / ALLY_SHADOW_HEIGHT);

			if (HitRes->Position[2].y > transform_.pos.y - ALLY_SHADOW_HEIGHT)
				Vertex[2].dif.a = SHADOW_MAX_ALPHA * (1.0f - fabs(HitRes->Position[2].y - transform_.pos.y) / ALLY_SHADOW_HEIGHT);

			//ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
			Vertex[0].u = (HitRes->Position[0].x - transform_.pos.x) / (ALLY_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[0].v = (HitRes->Position[0].z - transform_.pos.z) / (ALLY_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[1].u = (HitRes->Position[1].x - transform_.pos.x) / (ALLY_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[1].v = (HitRes->Position[1].z - transform_.pos.z) / (ALLY_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[2].u = (HitRes->Position[2].x - transform_.pos.x) / (ALLY_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[2].v = (HitRes->Position[2].z - transform_.pos.z) / (ALLY_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;

			//影ポリゴンを描画
			DrawPolygon3D(Vertex, 1, imgShadow_, TRUE);
		}

		//検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(HitResDim);
	}

	//ライティングを有効にする
	SetUseLighting(TRUE);

	//Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);
}

void AllyBase::SetCollisionPos(const VECTOR collision)
{
	collisionPos_ = collision;
}

VECTOR AllyBase::GetCollisionPos(void)const
{
	return VAdd(collisionLocalPos_, transform_.pos);
}

float AllyBase::GetCollisionRadius(void)
{
	return collisionRadius_;
}
#pragma endregion

void AllyBase::AttackCollisionPos(void)
{
	//プレイヤーとの衝突判定
	//攻撃の方向（エネミー）
	VECTOR forward = transform_.quaRot.GetForward();
	//攻撃の開始位置と終了位置
	attackCollisionPos_ = VAdd(transform_.pos, VScale(forward, ATTACK_FORWARD_OFFSET));
	attackCollisionPos_.y += ATTACK_HEIGHT_OFFSET;  // 攻撃の高さ調整

	//プレイヤーを見る
	EnemyToPlayer();
}

void AllyBase::EnemyToPlayer(void)
{
	//プレイヤーの当たり判定とサイズ
	playerCenter_ = player_->GetCollisionPos();
	playerRadius_ = player_->GetCollisionRadius();

	if (AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, playerCenter_, playerRadius_)
			&& player_->pstate_ != Player::PlayerState::DOWN)
	{
		isAttack_P = true;
		ChangeState(STATE::ATTACK);
	}
	else if (!AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, playerCenter_, playerRadius_)
		|| player_->pstate_ == Player::PlayerState::DOWN)
	{
		ChangeState(STATE::PLAY);
	}
}

void AllyBase::CheckHitAttackHit(void)
{
	//プレイヤーの当たり判定とサイズ
	playerCenter_ = player_->GetCollisionPos();
	playerRadius_ = player_->GetCollisionRadius();

	if(AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_,playerCenter_, playerRadius_))
	{
		player_->Damage(attackPow_);
	}

	//for (const auto& enemy : *ally_)
	//{
	//	///if (!enemy || !enemy->IsAlive()) continue;

	//	//敵の当たり判定とサイズ
	//	VECTOR enemyPos = enemy->GetCollisionPos();
	//	float enemyRadius = enemy->GetCollisionRadius();

	//	//球体同士の当たり判定
	//	if (AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, enemyPos, enemyRadius))
	//	{
	//		enemy->Damage(1);
	//		//1体のみヒット
	//		break;
	//	}
	//}

	if (isAttack_ || enemy_)
	{
		//エネミーとの衝突判定

		for (const auto& enemy : *enemy_)
		{
			if (!enemy || !enemy->IsAlive()) continue;

			//敵の当たり判定とサイズ
			VECTOR enemyPos = enemy->GetCollisionPos();
			float enemyRadius = enemy->GetCollisionRadius();

			//球体同士の当たり判定
			if (AsoUtility::IsHitSpheres(attackCollisionPos_, attackCollisionRadius_, enemyPos, enemyRadius))
			{
				player_->Damage(attackPow_);
				//1体のみヒット
				break;
			}
		}
	}
}

void AllyBase::SetGameScene(GameScene* scene)
{
	scene_ = scene;
}

#pragma region Stateの切り替え

void AllyBase::ChangeState(STATE state)
{
	//状態変更
	state_ = state;

	//各状態遷移の初期処理
	stateChanges_[state_]();
}

void AllyBase::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&AllyBase::UpdateNone, this);
}

void AllyBase::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&AllyBase::UpdateIdle, this);
}
void AllyBase::ChangeStatePlay(void)
{
	stateUpdate_ = std::bind(&AllyBase::UpdatePlay, this);
}

void AllyBase::ChangeStateAttack(void)
{
	stateUpdate_ = std::bind(&AllyBase::UpdateAttack, this);
}

void AllyBase::ChangeStateBlow(void)
{
	stateUpdate_ = std::bind(&AllyBase::UpdateBlow, this);
}

#pragma endregion

void AllyBase::SetPlayer(std::shared_ptr<Player> player)
{
	player_ = player;
}

void AllyBase::DrawDebug(void)
{

#ifdef _DEBUG

	VECTOR v;
	VECTOR s;
	VECTOR a;

	//キャラ基本情報
	//-------------------------------------------------------
	//キャラ座標
	v = transform_.pos;
	DrawFormatString(20, 120, white, "キャラ座標 ： (%0.2f, %0.2f, %0.2f)", v.x, v.y, v.z);

	s = collisionPos_;
	DrawSphere3D(s, collisionRadius_, 8, black, black, false);
	DrawFormatString(20, 180, white, "スフィア座標 ： (%0.2f, %0.2f, %0.2f)", s.x, s.y, s.z);
	DrawFormatString(20, 210, white, "エネミーの移動速度 ： %0.2f", speed_);

	a = attackCollisionPos_;
	DrawSphere3D(a, attackCollisionRadius_, 8, yellow, yellow, false);

#endif //DEBUG
}

void AllyBase::DrawDebugSearchRange(void)
{
	VECTOR centerPos = transform_.pos;

	//プレイヤーの座標
	VECTOR playerPos = player_->GetTransform().pos; // プレイヤーオブジェクトの参照を持っている想定

	//プレイヤーと敵の距離（XZ平面）
	float dx = playerPos.x - centerPos.x;
	float dz = playerPos.z - centerPos.z;
	float distance = sqrtf(dx * dx + dz * dz);

	//範囲内か判定
	bool inRange = (distance <= VIEW_RANGE);

	float angleStep = AsoUtility::FULL_ROTATION_RAD / VALUE_SIXTY;

	for (int i = VALUE_ZERO; i < VALUE_SIXTY; ++i)
	{
		float angle1 = angleStep * i;
		float angle2 = angleStep * (i + VALUE_ONE);

		VECTOR p1 = {
			centerPos.x + VIEW_RANGE * sinf(angle1),
			centerPos.y,
			centerPos.z + VIEW_RANGE * cosf(angle1)
		};
		VECTOR p2 = {
			centerPos.x + VIEW_RANGE * sinf(angle2),
			centerPos.y,
			centerPos.z + VIEW_RANGE * cosf(angle2)
		};
	}
}