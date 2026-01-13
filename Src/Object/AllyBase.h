#pragma once
#include <memory>
#include<map>
#include <functional>
#include <vector>
#include "ActorBase.h"
#include"../Object/EnemyBase.h"

class AnimationController;
class GameScene;
class TutorialScene;
class Player;
class Capsule;

class AllyBase : public ActorBase
{
public:
	//味方の大きさ
	static constexpr float ALLY_SIZE = 0.7f;

	//デバッグ &　汎用
	static constexpr float VALUE_TWO = 2.0f;
	static constexpr float VALUE_SIXTY = 60.0f;

	//影の大きさ
	static constexpr float ALLY_SHADOW_SIZE = 100.0f;
	static constexpr float ALLY_SHADOW_HEIGHT = 300.0f;
	static constexpr float SHADOW_LIFT = 0.5f;					//ポリゴンを持ち上げる量
	static constexpr int   SHADOW_MAX_ALPHA = 128;				//影の最大不透明度
	static constexpr float SHADOW_UV_SCALE = 2.0f;				//UV計算用
	static constexpr float SHADOW_UV_CENTER = 0.5f;

	static constexpr VECTOR CAPSULE_TOP = { 0.0f, 30.0f, 0.0f };		//カプセルの頂点
	static constexpr VECTOR CAPSULE_BOTTOM = { 0.0f,  -100.0f, 0.0f };	//カプセルの足元
	static constexpr float CAPSULE_RADIUS = 20.0f;						//カプセルの半径
	static constexpr float COLLISION_RADIUS = 100.0f;					//衝突判定の半径

	static constexpr float GROUND_CHECK_DISTANCE = 10.0f;			//地面チェック用パワー
	static constexpr float GRAVITY_ALIGN_DOT_THRESHOLD = 0.9f;		//地面の閾値
	static constexpr int MAX_PENETRATION_RESOLVE_ITERATIONS = 10;	//衝突試行回数

	//色MAX値
	static constexpr int MAX_COLOR = 255;

	//重力
	static constexpr float GRAVITY = 9.8f;

	//向き
	static constexpr float DEGREE = 180.0f;

	//停止許容
	static constexpr float STOP_VELOCITY_EPSILON = 0.01f;

	//ふっとびスピード
	static constexpr float FLY_SPEED = 40.0f;

	//ふっとびパワー
	static constexpr float POWER = 65.0f;

	//味方の共通用球体座標
	static constexpr float COLLOSION_RADIUS = 300.0f;
	static constexpr VECTOR COLLISION_POS = { 0.0f, 150.0f, 0.0f };
	
	//攻撃の位置オフセット
	static constexpr float ATTACK_FORWARD_OFFSET = 55.0f;	// 前方向
	static constexpr float ATTACK_HEIGHT_OFFSET = 100.0f;	// 高さ

	//アニメーションスピード
	static constexpr float ANIM_SPEED = 25.0f;

	//アニメーション番号
	static constexpr int   ANIM_T_POSE_INDEX = 0;
	static constexpr int   ANIM_IDLE_INDEX = 1;
	static constexpr int   ANIM_WALK_INDEX = 2;
	static constexpr int   ANIM_RUN_INDEX = 3;
	static constexpr int   ANIM_ATTACK_INDEX = 4;
	static constexpr int   ANIM_SKY_INDEX = 5;
	static constexpr int   ANIM_ROLL_INDEX = 6;

	//アニメーション後の重力
	static constexpr float ANIMATION_OUT_GRAVITY = 0.15f;

	//カメラ復帰
	static constexpr float RETURN_CAMERA_TIME = 2.0f;

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int red = 0xff0000;	  //赤
	int green = 0x00ff00; //緑
	int blue = 0x0000ff;  //青
	int yellow = 0xffff00;//黄
	int purpl = 0x800080; //紫

	//敵の種類
	enum class TYPE
	{
		RED,		//赤い味方
		BLUE,		//青い味方
		BLACK,		//黒い味方
		MAX
	};

	//状態管理
	enum class STATE
	{
		NONE,
		IDLE,
		PLAY,
		ATTACK, 
		BLOW,
		MAX
	};

	//アニメーション種別
	enum class ANIM_TYPE
	{
		NONE,
		IDLE,
		WALK,
		RUN,
		ATTACK,
		SKY,
		ROLL,
		MAX
	};

	AllyBase();	//コンストラクタ
	virtual ~AllyBase(void);	//デストラクタ

	virtual void Init(void);			//初期処理(最初の１回のみ実行)
	virtual void InitAnimation(void) {};//アニメーションロード用
	virtual void SetParam(void){};		//パラメータ設定(純粋仮想関数)
	virtual void Update(void);			//更新処理(毎フレーム実行)
	virtual void Draw(void);			//描画処理(毎フレーム実行)
	virtual void DrawBossHpBar(void){};	//ボスのHPバー
	virtual void Release(void);			//解放処理(最後の１回のみ実行)

	virtual void EffectAttack(void) {};			//キックチャージ

	//球の生成
	virtual void DrawShots(void);

	void SetPos(VECTOR pos);	//座標の設定
	STATE GetState(void);		//状態獲得

	bool IsAlive(void);			//生存判定
	void SetAlive(bool alive);	//生存判定

	//吹っ飛び中かどうかを返す
	bool IsBlow(void) const;

	//ダメージを受ける
	void Damage(int damage,float chargeRate);

	//吹っ飛び中攻撃
	void TriggerAttackWhileBlow(void);

	TYPE GetEnemyType(void) const;
	TYPE enemyType_;	//敵のタイプ
	
	void SetCollisionPos(const VECTOR collision);//衝突判定用の球体
	VECTOR GetCollisionPos(void)const;	//衝突用の中心座標の取得
	float GetCollisionRadius(void);		//衝突用の球体半径の取得

	void SetGameScene(GameScene* gscene);
	void SetTutorialScene(TutorialScene* tscene);

	void DrawDebug(void);	//デバッグ

	void SetPlayer(std::shared_ptr<Player> player);
	void SetEnemy(const std::vector<std::shared_ptr<EnemyBase>>* enemys);

	bool IsAttacking(void) const;

	//攻撃終了
	bool IsActionFinished(void) const;
	
	//完全停止
	bool IsStoppedCompletely(void) const;

	void SetActionFinished(bool finished);
	
protected:
	VECTOR defaultPos_ = VGet(0, 0, 0);  //初期位置
	VECTOR velocity_ = VGet(0, 0, 0);  //吹っ飛び中の速度
	bool isBlow_ = false;            //吹っ飛び中フラグ

	bool isKicked_ = false;    //追加：蹴られ中フラグ

	bool actionFinished_ = false;

	int damageCnt_;

	std::shared_ptr<Player> player_;
	
	GameScene* gScene_;
	TutorialScene* tScene_;

	const std::vector<std::shared_ptr<EnemyBase>>* enemy_;

	//アニメーション
	std::unique_ptr<AnimationController> animationController_;
	
	//状態管理
	STATE state_;
	std::map<STATE, std::function<void(void)>> stateChanges_;//状態管理(状態遷移時初期処理)
	std::function<void(void)> stateUpdate_;					 //状態管理(更新ステップ)
	
	float speed_;	//移動速度
	
	VECTOR movePow_;	//移動量
	VECTOR movedPos_;	//移動後の座標
	VECTOR moveDiff_;	//フレームごとの移動値
	VECTOR collisionPos_;		//赤い球体の移動後座標
	VECTOR attackCollisionPos_; //紫の球体の移動後座標

	float returnCameraTimer_ = 0.0f;
	bool shouldReturnCamera_ = false;

	int hp_;	// 体力
	int attackPow_; //攻撃力

	//エフェクト
	int effectAttackResId_;
	int effectAttackPlayId_;

	VECTOR jumpPow_;

	bool initFall_; //攻撃落下初期化フラグ

	bool isAlive_;	//生存判定
	bool isAttack_ = false;		//攻撃判定

	bool hasGivenDamage_ = false;   //攻撃中にもうダメージを与えたか
	float attackHitTiming_ = 0.0f;  //攻撃モーションのどのタイミングで当たるか（例：0.4秒）
	float attackElapsed_ = 0.0f;    //攻撃開始からの経過時間

	bool isGrounded_ = false;
	
	ANIM_TYPE animtype_; //アニメーションのタイプ確認用

	float collisionRadius_;		//衝突判定用の球体半径
	VECTOR collisionLocalPos_;	//衝突判定用の球体中心の調整座標

	float attackCollisionRadius_;	 //攻撃判定用と攻撃範囲の球体半径
	VECTOR attackCollisionLocalPos_; //攻撃判定用と攻撃範囲の調整座標

	VECTOR playerCenter_;	//プレイヤーの球体の座標を取得
	float playerRadius_;	//プレイヤーの球体の半径を取得
	VECTOR p_Diff_;			//プレイヤーの位置差分
	float p_Dis_;			//プレイヤーまでの距離
	float p_RadiusSum_;		//プレイヤーとの衝突半径の合計

	VECTOR enemyCenter_;	//プレイヤーの球体の座標を取得
	float enemyRadius_;	//プレイヤーの球体の半径を取得

	bool isBlowedEnd_; // 一度吹っ飛び終わったか

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

	//更新系
	void UpdateNone(void){};		//更新ステップ
	virtual void UpdateIdle(void);	//待機状態の更新
	virtual void UpdatePlay(void);	//移動時の更新処理
	virtual void UpdateAttack(void);//アタック時の更新処理
	virtual void UpdateBlow(void);	//飛ぶとき

	virtual void StartAttack(void);     // 攻撃開始時（共通）

	void DrawDamage();	//ダメージ画像の描画

	//攻撃関係
	virtual void CollisionAttack(void);	//攻撃が当たったか確認する

	//状態遷移
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStateIdle(void);
	void ChangeStatePlay(void);
	void ChangeStateAttack(void);
	void ChangeStateBlow(void);

	void CalcGravityPow(void);		//移動量の計算

	void DrawShadow(void);	//影

	//丸影
	int imgShadow_;

};