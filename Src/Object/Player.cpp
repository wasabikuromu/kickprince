#include <string>
#include <vector>
#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/GravityManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "Common/AnimationController.h"
#include "Common/Capsule.h"
#include "Common/Collider.h"
#include "Player.h"

Player::Player(void)
{
	animationController_ = nullptr;
	ally_ = nullptr;
	
	state_ = STATE::NONE;

	//足煙エフェクト
	effectSmokeResId_ = -1;
	effectSmokePleyId_ = -1;

	//チャージエフェクト
	effectChargeResId_ = -1;
	effectChargePlayId_ = -1;

	//衝突チェック
	gravHitPosDown_ = AsoUtility::VECTOR_ZERO;
	gravHitPosUp_ = AsoUtility::VECTOR_ZERO;

	jumpPow_ = AsoUtility::VECTOR_ZERO;

	//丸影
	imgShadow_ = -1;

	//攻撃の初期化
	normalAttack_ = NORMAL_ATTACK;
	isAttack_ = false;

	//ステ関連
	hp_ = HP;

	//移動が可能かどうか
	canMove_ = true;

	//操作可能
	controlEnabled_ = true;

	//状態管理
	stateChanges_.emplace(
		STATE::PLAY, std::bind(&Player::ChangeStatePlay, this));
}

Player::~Player(void)
{
}

void Player::Init(void)
{
	//モデルの基本設定
	transform_.SetModel(resMng_.Load(
		ResourceManager::SRC::PLAYER).handleId_);
	transform_.scl = { PLAYER_SIZE,PLAYER_SIZE,PLAYER_SIZE };
	transform_.pos = PLAYER_POS;
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal = Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(PLAYER_ROT_Y), 0.0f });
	transform_.Update();

	//丸影画像
	imgShadow_ = resMng_.Load(ResourceManager::SRC::SHADOW).handleId_;

	//ゲージ枠
	imgGaugeFrame_ = resMng_.Load(ResourceManager::SRC::GAUGE_FRAME).handleId_;

	//足煙エフェクト
	effectSmokeResId_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::FOOT_SMOKE).handleId_;

	//チャージエフェクト
	effectChargeResId_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::EFF_CHARGE).handleId_;

	//アニメーションの設定
	InitAnimation();

	//カプセルコライダ
	capsule_ = std::make_unique<Capsule>(transform_);
	capsule_->SetLocalPosTop(CAPSULE_TOP);
	capsule_->SetLocalPosDown(CAPSULE_BOTTOM);
	capsule_->SetRadius(CAPSULE_RADIUS);

	//衝突判定用の球体半径
	collisionRadius_ = COLLISION_RADIUS;

	//衝突判定用の球体中心の調整座標
	collisionLocalPos_ = { 0.0f, capsule_->GetCenter().y, 0.0f};

	//初期状態
	ChangeState(STATE::PLAY);
}

void Player::Update(void)
{
	if (isCharging_)
	{
		EffectCharge();
	}
	else
	{
		if (effectChargePlayId_ != -1)
		{
			StopEffekseer3DEffect(effectChargePlayId_);
			effectChargePlayId_ = -1;
		}
	}

	if (isTutorialPaused_) {
		//動きを完全停止
		movePow_ = VGet(0, 0, 0);

		//歩きアニメを止める
		animationController_->Play((int)ANIM_TYPE::IDLE);

		return;
	}

	//通常時の更新
	stateUpdate_();
	transform_.Update();
	animationController_->Update();
	UpdateDown(1.0f);
}

void Player::UpdateDown(float deltaTime)
{
	auto& ins = InputManager::GetInstance();
}

void Player::SetTutorialPause(bool pause)
{
	isTutorialPaused_ = pause;
}

void Player::Draw(void)
{
	MV1DrawModel(transform_.modelId);	//モデルの描画
	DrawShadow();						//丸影描画
	//DrawDebug();						//デバッグ用描画
	DrawGuideLine();					//ガイド線描画
	DrawChargeGauge();					//ゲージ描画
}

void Player::AddCollider(std::weak_ptr<Collider> collider)
{
	colliders_.push_back(collider);
}

void Player::ClearCollider(void)
{
	colliders_.clear();
}

void Player::SetEnemy(const std::vector<std::shared_ptr<AllyBase>>* enemys)
{
	ally_ = enemys;
}

VECTOR Player::GetPos() const
{
	return transform_.pos;
}

void Player::SetPos(const VECTOR& pos)
{
	transform_.pos = pos;
}

const Capsule& Player::GetCapsule(void) const
{
	return *capsule_;
}

VECTOR Player::GetCollisionPos(void) const
{
	return VAdd(collisionLocalPos_, transform_.pos);
}

float Player::GetCollisionRadius(void)
{
	return collisionRadius_;
}

void Player::SetControlEnabled(bool enabled)
{
	controlEnabled_ = enabled;
}

bool Player::IsControlEnabled(void) const
{
	return controlEnabled_;
}

const std::vector<std::shared_ptr<AllyBase>>& Player::GetEnemyCollision(void) const
{
	return *ally_;
}

void Player::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "NPlayer/PPlayer.mv1";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	
	animationController_->Add((int)ANIM_TYPE::NONE,		path, ANIM_SPEED, ANIM_T_POSE_INDEX);
	animationController_->Add((int)ANIM_TYPE::IDLE,		path, ANIM_SPEED, ANIM_IDLE_INDEX);
	animationController_->Add((int)ANIM_TYPE::WALK,		path, ANIM_SPEED, ANIM_WALK_INDEX);
	animationController_->Add((int)ANIM_TYPE::RUN,		path, ANIM_SPEED, ANIM_RUN_INDEX);
	animationController_->Add((int)ANIM_TYPE::KICK,		path, ANIM_SPEED, ANIM_KICK_INDEX);
	animationController_->Add((int)ANIM_TYPE::VICTORY,	path, ANIM_SPEED, ANIM_VICTORY_INDEX);
	animationController_->Add((int)ANIM_TYPE::ROSE,		path, ANIM_SPEED, ANIM_ROSE_INDEX);

	animationController_->Play((int)ANIM_TYPE::IDLE);
}

void Player::ChangeState(STATE state)
{
	//状態変更
	state_ = state;

	//各状態遷移の初期処理
	stateChanges_[state_]();
}

void Player::ChangeStatePlay(void)
{
	stateUpdate_ = std::bind(&Player::UpdatePlay, this);
}

void Player::UpdatePlay(void)
{
	//操作不可
	if (!controlEnabled_ && !isAttack_)
	{
		movePow_ = AsoUtility::VECTOR_ZERO;

		//IDLEアニメは流す
		animationController_->Play((int)ANIM_TYPE::IDLE);
		return;
	}

	//移動処理
	ProcessMove();

	//移動方向に応じた回転
	Rotate();

	//攻撃処理
	ProcessAttack();

	//衝突判定
	Collision();

	//重力による移動量
	CalcGravityPow();

	//現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	//移動
	transform_.pos = movedPos_;

	//移動方向への回転
	auto moveRot = Quaternion::LookRotation(moveDir_);

	//向きを滑らかに
	transform_.quaRot = Quaternion::Slerp(transform_.quaRot, moveRot, 0.2f);

	//歩きエフェクト
	EffectFootSmoke();
}

void Player::DrawShadow(void)
{
	int i, j;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;
	int ModelHandle;

	//ライティングを無効にする
	SetUseLighting(false);

	//Ｚバッファを有効にする
	SetUseZBuffer3D(true);

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
			VAdd(transform_.pos, VGet(0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f)), PLAYER_SHADOW_SIZE);

		//頂点データで変化が無い部分をセット
		Vertex[0].dif = GetColorU8(MAX_ALPHA, MAX_ALPHA, MAX_ALPHA, MAX_ALPHA);
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
			if (HitRes->Position[0].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[0].dif.a = SHADOW_MAX_ALPHA * (1.0f - fabs(HitRes->Position[0].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT);

			if (HitRes->Position[1].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[1].dif.a = SHADOW_MAX_ALPHA * (1.0f - fabs(HitRes->Position[1].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT);

			if (HitRes->Position[2].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[2].dif.a = SHADOW_MAX_ALPHA * (1.0f - fabs(HitRes->Position[2].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT);

			//ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
			Vertex[0].u = (HitRes->Position[0].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[0].v = (HitRes->Position[0].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[1].u = (HitRes->Position[1].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[1].v = (HitRes->Position[1].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[2].u = (HitRes->Position[2].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;
			Vertex[2].v = (HitRes->Position[2].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * SHADOW_UV_SCALE) + SHADOW_UV_CENTER;

			//影ポリゴンを描画
			DrawPolygon3D(Vertex, 1, imgShadow_, true);
		}

		//検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(HitResDim);
	}

	//ライティングを有効にする
	SetUseLighting(true);

	//Ｚバッファを無効にする
	SetUseZBuffer3D(false);
}

void Player::DrawDebug(void)
{
#ifdef _DEBUG

	VECTOR v;

	//キャラ基本情報
	//-------------------------------------------------------
	//キャラ座標
	v = transform_.pos;
	DrawFormatString(0, 0, black, "Player座標 ： (%0.2f, %0.2f, %0.2f)%d", v.x, v.y, v.z, hp_);
	//-------------------------------------------------------

	//衝突
	DrawLine3D(gravHitPosUp_, gravHitPosDown_, black);

	capsule_->Draw();
#endif
}

void Player::DrawGuideLine(void)
{
}

void Player::DrawChargeGauge()
{
	//------------------------------------------------------------------
	//位置
	const int DRAW_X = 1200;
	const int DRAW_Y = 550;

	//チャージ率
	float rate = chargeTime_ / maxChargeTime_;
	if (rate < 0.0f) rate = 0.0f;
	if (rate > 1.0f) rate = 1.0f;

	//ゲージ形状
	const int   INNER_HEIGHT = 450;
	const float BOTTOM_WIDTH = 40.0f;
	const float TOP_WIDTH = 99.0f;
	const float WIDTH_SLOPE = (TOP_WIDTH - BOTTOM_WIDTH) / INNER_HEIGHT;

	const int FRAME_WIDTH = 110;
	const int FRAME_HEIGHT = 460;

	const int OFFSET_Y = (FRAME_HEIGHT - INNER_HEIGHT) / 2;
	const int OFFSET_X = (FRAME_WIDTH - static_cast<int>(BOTTOM_WIDTH)) / 2;

	const int FILLED_HEIGHT = static_cast<int>(INNER_HEIGHT * rate);

	const int FRAME_OFFSET_X = 29;		//枠画像と中身のXズレ補正
	const int EMPTY_ALPHA = 128;        //半透明
	const int NO_BLEND_ALPHA = 255;     //不透明

	//------------------------------------------------------------------

	//塗られている部分（グラデーション）
	for (int i = 0; i < FILLED_HEIGHT; i++)
	{
		const float width = BOTTOM_WIDTH + WIDTH_SLOPE * i;

		const int y = DRAW_Y + (INNER_HEIGHT - i) + OFFSET_Y;
		const int left = DRAW_X + OFFSET_X;
		const int right = left + static_cast<int>(width);

		const float t = static_cast<float>(i) / INNER_HEIGHT;

		int r = 255;
		int g = 255;
		int b = 0;

		if (t < 0.5f)
		{
			float k = t / 0.5f;
			r = static_cast<int>(255 * k);
			g = 255;
		}
		else
		{
			float k = (t - 0.5f) / 0.5f;
			r = 255;
			g = static_cast<int>(255 * (1.0f - k));
		}

		DrawBox(left, y, right, y + 1, GetColor(r, g, b), true);
	}

	//空の部分（グレー）
	const unsigned int EMPTY_COLOR = GetColor(100, 100, 100);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, EMPTY_ALPHA);

	for (int i = FILLED_HEIGHT; i < INNER_HEIGHT; i++)
	{
		const float width = BOTTOM_WIDTH + WIDTH_SLOPE * i;

		const int y = DRAW_Y + (INNER_HEIGHT - i) + OFFSET_Y;
		const int left = DRAW_X + OFFSET_X;
		const int right = left + static_cast<int>(width);

		DrawBox(left, y, right, y + 1, EMPTY_COLOR, true);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, NO_BLEND_ALPHA);

	//枠
	DrawGraph(DRAW_X + FRAME_OFFSET_X, DRAW_Y, imgGaugeFrame_, true);
}

void Player::ProcessMove(void)
{
	//方向量をゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	//X軸回転を除いた、重力方向に垂直なカメラ角度(XZ平面)を取得
	Quaternion cameraRot = mainCamera->GetQuaRotOutX();

	//方向
	VECTOR dir = AsoUtility::VECTOR_ZERO;

	double rotRad = 0;

	//ゲームパッドの接続数で処理を分ける

	if (!isAttack_ && !isCharging_ && IsEndLandingA())
	{
		if (GetJoypadNum() == 0)
		{
			if (ins_.IsNew(KEY_INPUT_W))
			{
				dir = cameraRot.GetForward();
				rotRad = AsoUtility::Deg2RadF(ROT_FORWARD_DEG);
			}
			if (ins_.IsNew(KEY_INPUT_S))
			{
				dir = cameraRot.GetBack();
				rotRad = AsoUtility::Deg2RadF(ROT_BACK_DEG);
			}
			if (ins_.IsNew(KEY_INPUT_D))
			{
				dir = cameraRot.GetRight();
				rotRad = AsoUtility::Deg2RadF(ROT_RIGHT_DEG);
			}
			if (ins_.IsNew(KEY_INPUT_A))
			{
				dir = cameraRot.GetLeft();
				rotRad = AsoUtility::Deg2RadF(ROT_LEFT_DEG);
			}
		}
		else
		{
			//接続されているゲームパッド１の情報を取得
			InputManager::JOYPAD_IN_STATE padState =
				ins_.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

			//アナログキーの入力値から方向を取得
			dir = ins_.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

			//カメラがクォータニオンを採用している場合
			//アナログキーをカメラ方向に回転
			dir = cameraRot.PosAxis(dir);
		}
		if (!AsoUtility::EqualsVZero(dir))
		{
			//移動量
			speed_ = SPEED_MOVE;
			if (ins_.IsNew(KEY_INPUT_LSHIFT)||
				ins_.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RB))
			{
				speed_ = SPEED_RUN;
			}

			moveDir_ = dir;

			//移動量
			movePow_ = VScale(dir, speed_);

			//回転処理IDLE
			SetGoalRotate(rotRad);

			//アニメーション
			if (ins_.IsNew(KEY_INPUT_LSHIFT) ||
				ins_.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RB))
			{
				animationController_->Play((int)ANIM_TYPE::RUN);
			}
			else
			{
				animationController_->Play((int)ANIM_TYPE::WALK);
			}
		}
		else
		{
			animationController_->Play((int)ANIM_TYPE::IDLE);
		}
	}

	//次フレームの想定位置
	VECTOR nextPos;
	nextPos.x = transform_.pos.x + movePow_.x;
	nextPos.y = transform_.pos.y + movePow_.y;
	nextPos.z = transform_.pos.z + movePow_.z;

	//X方向の制限
	if (nextPos.x < FIELDXMIN || nextPos.x > FIELDXMAX)
	{
		movePow_.x = 0;
	}

	//Z方向の制限
	if (nextPos.z < FIELDZMIN || nextPos.z > FIELDZMAX)
	{
		movePow_.z = 0;
	}
}

void Player::SetGoalRotate(double rotRad)
{
	constexpr double ROTATE_RESET_THRESHOLD = 0.1;
	VECTOR cameraRot = mainCamera->GetAngles();

	Quaternion axis =
		Quaternion::AngleAxis(
			(double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);

	//現在設定されている回転との角度差を取る
	double angleDiff = Quaternion::Angle(axis, goalQuaRot_);

	//しきい値
	if (angleDiff > ROTATE_RESET_THRESHOLD)
	{
		stepRotTime_ = TIME_ROT;
	}

	goalQuaRot_ = axis;
}

void Player::Rotate(void)
{
	stepRotTime_ -= scnMng_.GetDeltaTime();

	//回転の球面補間
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);
}

void Player::Collision(void)
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

void Player::CollisionGravity(void)
{
	//ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

	//重力方向
	VECTOR dirGravity = grvMng_.GetDirGravity();

	//重力方向の反対
	VECTOR dirUpGravity = grvMng_.GetDirUpGravity();

	//重力の強さ
	float gravityPow = grvMng_.GetPower();

	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));

	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, GROUND_CHECK_DISTANCE * VALUE_TWO));

	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, GROUND_CHECK_DISTANCE));

	for (const auto c : colliders_)
	{
		//地面との衝突
		auto hit = MV1CollCheck_Line(
			c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > GRAVITY_ALIGN_DOT_THRESHOLD)
		{
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, VALUE_TWO));
		}
	}
}

void Player::CollisionCapsule(void)
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
			for (int tryCnt = 0; tryCnt < MAX_PENETRATION_RESOLVE_ITERATIONS; tryCnt++)
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

void Player::CollisionAttack(float chargeRate)
{
	//すでにヒットしていたら何もしない
	if (hitChecked_) return;

	//攻撃してないor味方いないなら何もしない
	if (!isAttack_ || !ally_) return;

	const auto& anim = animationController_->GetPlayAnim();

	//攻撃判定フレーム外
	if (anim.step < ATTACK_START || anim.step > ATTACK_END) return;

	float attackRadius = ATTACK_RADIUS;

	VECTOR forward = transform_.quaRot.GetForward();
	VECTOR attackPos = VAdd(transform_.pos, VScale(forward, ATTACK_FORWARD));

	for (const auto& ally : *ally_)
	{
		if (!ally || !ally->IsAlive()) continue;

		VECTOR allyPos = ally->GetCollisionPos();
		float allyRadius = ally->GetCollisionRadius();

		if (AsoUtility::IsHitSpheres(
			attackPos, attackRadius,
			allyPos, allyRadius))
		{
			ally->Damage(normalAttack_, chargeRate);

			//結果だけ記録
			hitChecked_ = true;
			kickedAlly_ = true;

			SoundManager::GetInstance().Play(SoundManager::SRC::KICK, Sound::TIMES::FORCE_ONCE);

			return;
		}
	}
}

void Player::CalcGravityPow(void)
{
	//重力方向
	VECTOR dirGravity = grvMng_.GetDirGravity();

	//重力の強さ
	float gravityPow = grvMng_.GetPower();

	//重力
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

	//内積
	float dot = VDot(dirGravity, jumpPow_);
	if (dot >= 0.0f)
	{
		jumpPow_ = gravity;
	}
}

void Player::ProcessAttack(void)
{
	//入力
	bool isPress = ins_.IsTrgDown(KEY_INPUT_E) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT);	//押した瞬間

	bool isHold = CheckHitKey(KEY_INPUT_E) ||
		ins_.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT);       //押しっぱなし

	bool isRelease = (!CheckHitKey(KEY_INPUT_E)) &&
		ins_.IsPadBtnTrgUp(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT);		//離した瞬間

	//チュートリアルで攻撃禁止のときは何もさせない
	if (!attackEnable_) {
		isCharging_ = false;
		isAttack_ = false;
		attackReleased_ = false;
		return;
	}

	if (isAttack_)
	{
		float chargeRate = chargeTime_ / maxChargeTime_;

		//ヒット判定
		CollisionAttack(chargeRate);

		//アニメ終了で攻撃終了
		if (animationController_->IsEnd())
		{
			isAttack_ = false;

			if (!kickedAlly_)
			{
				controlEnabled_ = true;
				animationController_->Play((int)ANIM_TYPE::IDLE, true);
			}
		}

		return;
	}

	//チャージ中は移動できない
	bool isMoving = (fabs(movePow_.x) > 0.01f || fabs(movePow_.z) > 0.01f);

	//②チャージ開始（押した瞬間のみ）
	if (!isCharging_ && isPress && !isMoving)
	{
		isCharging_ = true;
		isChargeIncreasing_ = true;
		chargeTime_ = 0.0f;
		animationController_->Play((int)ANIM_TYPE::IDLE, true);

	}

	//③チャージ中
	if (isCharging_)
	{
		//チャージ
		EffectCharge();
		float dt = scnMng_.GetDeltaTime();

		if (isChargeIncreasing_)
		{
			chargeTime_ += dt;
			if (chargeTime_ >= maxChargeTime_)
			{
				chargeTime_ = maxChargeTime_;
				isChargeIncreasing_ = false;
			}
		}
		else
		{
			chargeTime_ -= dt;
			if (chargeTime_ <= 0.0f)
			{
				chargeTime_ = 0.0f;
				isChargeIncreasing_ = true;
			}
		}

		DrawGuideLine();
		DrawChargeGauge();
	}

	//④チャージ解除
	if (isCharging_ && isRelease)
	{
		isCharging_ = false;

		//1フレーム限定ではなく保持する
		attackReleased_ = true;

		//味方
		kickedAlly_ = false;

		//キック
		isAttack_ = true;
		attackTimer_ = 0.0f;
		hitChecked_ = false;

		animationController_->Play((int)ANIM_TYPE::KICK, false);

		float chargeRate = chargeTime_ / maxChargeTime_;
		CollisionAttack(chargeRate);

		controlEnabled_ = false;
	}
}

bool Player::IsEndLandingA(void)
{
	bool ret = true;
	int animType = animationController_->GetPlayType();

	//攻撃アニメじゃない = 攻撃してない→ true
	if (animType != (int)ANIM_TYPE::KICK && !animationController_->IsEnd())
	{
		return ret;
	}

	//KICK中でアニメーションが終わった→true
	if (animationController_->IsEnd())
	{
		return ret;
	}

	//KICK中でアニメーション再生中→false
	return false;
}

void Player::Damage(int damage)
{
	if (pstate_ == PlayerState::DOWN) return;
	hp_ -= damage;

	//SE
	SoundManager::GetInstance().Play(SoundManager::SRC::P_DAMAGE_SE, Sound::TIMES::FORCE_ONCE);

	if (hp_ <= 0) {
		hp_ = 0;

		//player死んだら終わり
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);

		//SE
		SoundManager::GetInstance().Play(SoundManager::SRC::P_DOWN_SE, Sound::TIMES::ONCE);
	}
}

bool Player::IsPreparingAttack(void) const
{
	return isCharging_;
}

bool Player::IsKickReleased(void) const
{
	return attackReleased_;
}

bool Player::ConsumeKickReleased(void)
{
	if (attackReleased_)
	{
		attackReleased_ = false;
		return true;
	}
	return false;
}

void Player::SetAttackEnabled(bool e)
{
	attackEnable_ = e;
}

void Player::EffectFootSmoke(void)
{
	float len = AsoUtility::MagnitudeF(moveDiff_);

	stepFootSmoke_ -= scnMng_.GetDeltaTime();

	if (stepFootSmoke_ < 0.0f && len >= 1.0f)
	{
		stepFootSmoke_ = TERM_FOOT_SMOKE;

		//エフェクト再生
		effectSmokePleyId_ = PlayEffekseer3DEffect(effectSmokeResId_);

		//エフェクトの大きさ
		SetScalePlayingEffekseer3DEffect(effectSmokePleyId_, FOOT_SMOKE_SCALE, FOOT_SMOKE_SCALE, FOOT_SMOKE_SCALE);

		//エフェクトの位置
		SetPosPlayingEffekseer3DEffect(effectSmokePleyId_,transform_.pos.x, transform_.pos.y, transform_.pos.z);
	}
}

void Player::EffectCharge(void)
{
	//まだ再生していない場合のみ再生
	if (effectChargePlayId_ == -1)
	{
		effectChargePlayId_ = PlayEffekseer3DEffect(effectChargeResId_);
	}

	const float scale =
		(chargeTime_ + CHARGE_EFFECT_BASE) * CHARGE_EFFECT_SCALE;

	//スケール更新
	SetScalePlayingEffekseer3DEffect(
		effectChargePlayId_,
		scale,
		CHARGE_EFFECT_Y_SCALE * chargeTime_,
		scale);

	//位置更新
	SetPosPlayingEffekseer3DEffect(
		effectChargePlayId_,
		transform_.pos.x,
		transform_.pos.y,
		transform_.pos.z);
}

void Player::EndCharge(void)
{
	isCharging_ = false;

	if (effectChargePlayId_ != -1)
	{
		StopEffekseer3DEffect(effectChargePlayId_);
		effectChargePlayId_ = -1;
	}
}
