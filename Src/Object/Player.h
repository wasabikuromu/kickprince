#pragma once
#include <memory>
#include <map>
#include <functional>
#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "AllyBase.h"

class AnimationController;
class Collider;
class Capsule;

class Player : public ActorBase
{
public:

	//デバッグ &　汎用
	static constexpr float VALUE_TWO = 2.0f;
	static constexpr float VALUE_SIXTY = 60.0f;

	//プレイヤー
	static constexpr VECTOR PLAYER_POS = { -82.0f,170.0f, -1950.0f };	//初期位置
	static constexpr VECTOR CAPSULE_TOP = { 0.0f, 110.0f, 0.0f };		//カプセルの頂点
	static constexpr VECTOR CAPSULE_BOTTOM = { 0.0f, 30.0f, 0.0f };		//カプセルの足元
	static constexpr float PLAYER_SIZE = 0.01f;							//プレイヤーサイズ
	static constexpr float PLAYER_ROT_Y = 180.0f;						// Y軸回転(度数)
	static constexpr float COLLISION_RADIUS = 100.0f;					//衝突判定の半径
	static constexpr float CAPSULE_RADIUS = 20.0f;						//カプセルの半径
	static constexpr float ROT_FORWARD_DEG = 0.0f;						//プレイヤー角度(前)
	static constexpr float ROT_BACK_DEG = 180.0f;						//プレイヤー角度(後ろ)
	static constexpr float ROT_RIGHT_DEG = 90.0f;						//プレイヤー角度(右)
	static constexpr float ROT_LEFT_DEG = -90.0f;						//プレイヤー角度(左)

	//アニメーション関係
	static constexpr float ANIM_SPEED = 25.0f;

	//アニメーション番号
	static constexpr int   ANIM_T_POSE_INDEX = 0;
	static constexpr int   ANIM_IDLE_INDEX = 1;
	static constexpr int   ANIM_WALK_INDEX = 2;
	static constexpr int   ANIM_RUN_INDEX = 3;
	static constexpr int   ANIM_KICK_INDEX = 4;
	static constexpr int   ANIM_VICTORY_INDEX = 5;
	static constexpr int   ANIM_ROSE_INDEX = 6;

	//スピード
	static constexpr float SPEED_MOVE = 5.0f;
	static constexpr float SPEED_RUN = 10.0f;
	
	//回転完了までの時間
	static constexpr float TIME_ROT = 1.0f;

	//影の大きさ
	static constexpr float PLAYER_SHADOW_SIZE = 100.0f;
	static constexpr float PLAYER_SHADOW_HEIGHT = 300.0f;
	static constexpr float SHADOW_LIFT = 0.5f;					//ポリゴンを持ち上げる量
	static constexpr int   SHADOW_MAX_ALPHA = 128;				//影の最大不透明度
	static constexpr float SHADOW_UV_SCALE = 2.0f;				//UV計算用
	static constexpr float SHADOW_UV_CENTER = 0.5f;

	//煙エフェクト
	static constexpr float TERM_FOOT_SMOKE = 0.3f;		//煙エフェクト発生間隔
	static constexpr float FOOT_SMOKE_SCALE = 5.0f;		//煙エフェクトのスケール

	//チャージエフェクト
	static constexpr float CHARGE_EFFECT_BASE = 2.0f;
	static constexpr float CHARGE_EFFECT_SCALE = 10.0f;
	static constexpr float CHARGE_EFFECT_Y_SCALE = 30.0f;

	//ステ関連
	static constexpr int HP = 10;

	//地面との判定
	static constexpr float GROUND_CHECK_DISTANCE = 10.0f;			//地面チェック用パワー
	static constexpr float GRAVITY_ALIGN_DOT_THRESHOLD = 0.9f;		//地面の閾値
	static constexpr int MAX_PENETRATION_RESOLVE_ITERATIONS = 10;	//衝突試行回数

	//攻撃
	static constexpr int NORMAL_ATTACK = 2;				//通常攻撃
	static constexpr float ATTACK_RADIUS = 100.0f;		//通常攻撃判定の球半径
	static constexpr float ATTACK_FORWARD = 100.0f;		//通常攻撃位置の前方オフセット

	//攻撃判定フレーム
	const float ATTACK_START = 15.0f;
	const float ATTACK_END = 20.0f;

	//移動制限
	static constexpr float FIELDXMIN = -1100.0f;
	static constexpr float FIELDXMAX = 900.0f;
	static constexpr float FIELDZMIN = -2100.0f;
	static constexpr float FIELDZMAX = -1350.0f;

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int red = 0xff0000;	  //赤
	int green = 0x00ff00; //緑
	int blue = 0x0000ff;  //青
	int yellow = 0xffff00;//黄
	int purpl = 0x800080; //紫
	int gray = 0xaaaaaa;  //灰

	//色MAX値
	static constexpr int MAX_ALPHA = 255;

	//状態
	enum class STATE
	{
		NONE,
		PLAY,
	};

	//PLAY中の状態
	enum class PlayerState {
		NORMAL,
		DOWN,
	};

	PlayerState pstate_ = PlayerState::NORMAL;

	//アニメーション種別
	enum class ANIM_TYPE
	{
		NONE,
		IDLE,
		WALK,
		RUN,
		KICK,
		VICTORY,
		ROSE
	};

	//コンストラクタ
	Player(void);

	//デストラクタ
	~Player(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	//衝突判定に用いられるコライダ制御
	void AddCollider(std::weak_ptr<Collider> collider);
	void ClearCollider(void);

	//敵の情報を取得
	void SetEnemy(const std::vector<std::shared_ptr<AllyBase>>* enemys);

	//Posを取得
	VECTOR GetPos() const;
	void SetPos(const VECTOR& pos);

	//衝突用カプセルの取得
	const Capsule& GetCapsule(void) const;

	//衝突用の中心座標の取得
	VECTOR GetCollisionPos(void)const;

	//衝突用の球体半径の取得
	float GetCollisionRadius(void);

	//const Transform* GetTransform() const { return &transform_; }

	//操作可能？
	void SetControlEnabled(bool enabled);  // ← 追加
	bool IsControlEnabled(void) const;         // ← 追加

	//エネミーの衝突用座標
	const std::vector<std::shared_ptr<AllyBase>>& GetEnemyCollision(void) const;
	
	void Damage(int damage);	//ダメージ

	bool IsPreparingAttack(void) const;		//攻撃準中か
	bool IsKickReleased(void) const;		//攻撃後か
	bool ConsumeKickReleased(void);	//
	void SetAttackEnabled(bool e);

private:
	//アニメーション
	std::unique_ptr<AnimationController> animationController_;
	void InitAnimation(void);

	//状態管理
	STATE state_;
	std::map<STATE, std::function<void(void)>> stateChanges_;	// 状態管理(状態遷移時初期処理)
	std::function<void(void)> stateUpdate_;						// 状態管理(更新)

	//状態遷移
	void ChangeState(STATE state);
	void ChangeStatePlay(void);

	void UpdatePlay(void);				//stateがplayの状態のupdate
	void UpdateDown(float deltaTime);	//ダウン中の処理

	void SetTutorialPause(bool pause);
	bool isTutorialPaused_ = false;

	//kickのゲージ管理
	bool isCharging_ = false;     // チャージ中かどうか
	float chargeTime_ = 0.0f;     // チャージ時間（秒）
	const float maxChargeTime_ = 2.0f;  // 最大チャージ時間（例：2秒）
	bool isChargeIncreasing_ = true;

	//衝突
	std::unique_ptr<Capsule> capsule_;
	std::vector <std::weak_ptr<Collider>> colliders_;	//衝突判定に用いられるコライダ

	//衝突チェック
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	//衝突判定
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);

	//プレイヤーの動き
	void ProcessMove(void);			// 移動
	void ProcessAttack(void);		//攻撃モーション

	//攻撃判定
	void CollisionAttack(float chargeRate);
	bool attackReleased_ = false;
	bool attackEnable_ = true;

	//プレイヤーが持つ判定
	VECTOR collisionPos_;			//プレイヤーの当たり判定移動後座標
	float collisionRadius_;			// 衝突判定用の球体半径
	VECTOR collisionLocalPos_;		// 衝突判定用の球体中心の調整座標

	//回転
	void SetGoalRotate(double rotRad);
	void Rotate(void);

	//モーション終了
	bool IsEndLandingA(void);		// アタック終了

	//エフェクト
	void EffectFootSmoke(void);		//足煙エフェクト
	void EffectCharge(void);		//キックチャージ
	void EndCharge(void);			//チャージエフェクト終了

	//移動関連
	VECTOR moveDir_;				//移動方向
	VECTOR movePow_;				//移動量
	VECTOR movedPos_;				//移動後の座標
	VECTOR moveDiff_;				//フレームごとの移動値
	float speed_;					//移動スピード
	bool canMove_;					//移動が可能かどうか
	float stepRotTime_;				//回転補間の進行を管理するタイマー(残り時間)
	void CalcGravityPow(void);		//移動量の計算

	//操作可能
	bool controlEnabled_ = true;

	//回転
	Quaternion playerRotY_;
	Quaternion goalQuaRot_;

	VECTOR jumpPow_;

	//ステータス値
	int hp_;		//プレイヤーの体力

	//攻撃力
	int normalAttack_;	//2ダメージ

	//攻撃フラグ
	bool isAttack_;		//縦斬り
	float attackTimer_ = 0.0f;
	float attackDuration_ = 1.6f; // 攻撃アニメの長さ（調整可）
	
	//アイコンUI
	int imgGaugeFrame_;			//ゲージの枠

	//丸影
	int imgShadow_;

	//足煙エフェクト
	float stepFootSmoke_;
	int effectSmokeResId_;
	int effectSmokePleyId_;

	//チャージエフェクト
	int effectChargeResId_;
	int effectChargePlayId_;

	//ポインタ
	const std::vector<std::shared_ptr<AllyBase>>* ally_;

	//丸影描画
	void DrawShadow(void);

	//デバッグ処理
	void DrawDebug(void);

	//ガイド線
	void DrawGuideLine(void);

	//ゲージ描画
	void DrawChargeGauge(void);

	//インスタンス
	InputManager& ins_ = InputManager::GetInstance();
};