#pragma once
#include <memory>
#include<map>
#include <functional>
#include <vector>
#include "ActorBase.h"

class AnimationController;
class GameScene;
class TutorialScene;
class Player;
class AllyBase;

class EnemyBase : public ActorBase
{
public:

	//デバッグ &　汎用
	static constexpr float ZERO = 0.0f;					//初期化の値用
	static constexpr float VALUE_ONE = 1.0f;
	static constexpr float VALUE_TWO = 2.0f;
	static constexpr int  VALUE_SIXTY = 60;

	//敵の共通用球体座標
	static constexpr float COLLOSION_RADIUS = 250.0f;

	//攻撃の位置オフセット
	static constexpr float ATTACK_FORWARD_OFFSET = 55.0f;	//前方向
	static constexpr float ATTACK_HEIGHT_OFFSET = 100.0f;	//高さ

	static constexpr float ATTACK_RADIUS_SIZE = 35.0f;		//攻撃範囲

	//アニメーションスピード
	static constexpr float ANIM_SPEED = 20.0f;

	//アニメーション番号
	static constexpr int   ANIM_IDLE_INDEX = 0;
	static constexpr int   ANIM_RUN_INDEX = 1;
	static constexpr int   ANIM_ATTACK_INDEX = 2;
	static constexpr int   ANIM_DAMAGE_INDEX = 3;
	static constexpr int   ANIM_DEATH_INDEX = 4;
	
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
		DOG,	//犬
		ONION,	//玉ねぎ
		VIRUS,	//ウィルス
		BOSS,	//ボス
		MAX
	};

	//状態管理(現在の状態)
	enum class STATE
	{
		NONE,
		IDLE,
		PLAY,
		ATTACK,
		DAMAGE,
		DEATH,
		DEAD_END,
		MAX
	};

	// アニメーション種別
	enum class ANIM_TYPE
	{
		NONE,
		IDLE,
		RUN,
		ATTACK,
		DAMAGE,
		DEATH,
		MAX
	};

	EnemyBase();	// コンストラクタ
	virtual ~EnemyBase(void);	// デストラクタ

	virtual void Init(void);			//初期処理(最初の１回のみ実行)
	virtual void InitAnimation(void) {};//アニメーションロード用
	virtual void SetParam(void){};		//パラメータ設定(純粋仮想関数)
	virtual void Update(void);			//更新処理(毎フレーム実行)
	virtual void Draw(void);			//描画処理(毎フレーム実行)
	virtual void DrawBossHpBar(void){};	//ボスのHPバー
	virtual void Release(void);			//解放処理(最後の１回のみ実行)

	void SetPos(VECTOR pos);	//座標の設定
	STATE GetState(void);		//状態獲得

	bool IsAlive(void);			//生存判定
	void SetAlive(bool alive);	//生存判定

	void Damage(int damage);	//ダメージを受ける

	TYPE GetEnemyType(void) const;
	TYPE enemyType_;	//敵のタイプ

	void SetCollisionPos(const VECTOR collision);	//衝突判定用の球体
	VECTOR GetCollisionPos(void)const;				//衝突用の中心座標の取得
	float GetCollisionRadius(void);					//衝突用の球体半径の取得

	bool IsDeadFinished(void);			//完全死亡

	void SetGameScene(GameScene* gscene);
	void SetTutorialScene(TutorialScene* tscene);

	void DrawDebug(void);	//デバッグ用

	void SetPlayer(std::shared_ptr<Player> player);
	void SetAlly(const std::vector<std::shared_ptr<AllyBase>>* ally);

protected:

	int dCount;

	std::shared_ptr<Player> player_;

	//ポインタ
	const std::vector<std::shared_ptr<AllyBase>>* ally_;

	GameScene* gScene_;
	TutorialScene* tScene_;

	std::unique_ptr<AnimationController> animationController_;	//アニメーション
	
	std::map<STATE, std::function<void(void)>> stateChanges_;	//状態管理(状態遷移時初期処理)
	std::function<void(void)> stateUpdate_;						//状態管理(更新ステップ)

	float speed_;		//移動速度
	
	VECTOR movePow_;	//移動量
	VECTOR movedPos_;	//移動後の座標
	VECTOR moveDiff_;	//フレームごとの移動値

	VECTOR collisionPos_;		//赤い球体の移動後座標
	VECTOR attackCollisionPos_; //紫の球体の移動後座標

	int hp_;		//体力
	int attackPow_; //攻撃力

	bool isAlive_;				//生存判定
	bool isAttack_ = false;		//攻撃判定

	STATE state_;			//状態管理
	ANIM_TYPE animtype_;	//アニメーションのタイプ確認用

	float collisionRadius_;				//衝突判定用の球体半径
	VECTOR collisionLocalPos_;			//衝突判定用の球体中心の調整座標

	float attackCollisionRadius_;		//攻撃判定用と攻撃範囲の球体半径
	VECTOR attackCollisionLocalPos_;	//攻撃判定用と攻撃範囲の調整座標

	//更新系
	void UpdateNone(void){};			//更新ステップ
	virtual void UpdateIdle(void);		//待機状態の更新
	virtual void UpdatePlay(void);		//移動時の更新処理
	virtual void UpdateDamage(void);	//ダメージ時の更新処理
	virtual void UpdateDeath(void);		//死んだ時の更新処理
	virtual void UpdateDeadEnd(void);	//完全死亡

	//状態遷移
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStateIdle(void);
	void ChangeStatePlay(void);
	void ChangeStateDamage(void);
	void ChangeStateDeath(void);
	void ChangeStateDeadEnd(void);

	void Collision(void);	//衝突判定
};