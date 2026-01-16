#include "EnemyBoss.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"

EnemyBoss::EnemyBoss() :EnemyBase()
{
}

void EnemyBoss::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Enemy/Boss/Boss.mv1";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);

	animationController_->Add((int)ANIM_TYPE::IDLE,		path, ANIM_SPEED, BOSS_ANIM_IDLE_INDEX);
	animationController_->Add((int)ANIM_TYPE::RUN,		path, ANIM_SPEED, BOSS_ANIM_RUN_INDEX);
	animationController_->Add((int)ANIM_TYPE::ATTACK,	path, ANIM_SPEED, BOSS_ANIM_ATTACK_INDEX);
	animationController_->Add((int)ANIM_TYPE::DEATH,	path, ANIM_SPEED, BOSS_ANIM_DEATH_INDEX);

	animationController_->Play((int)ANIM_TYPE::RUN);
}

void EnemyBoss::SetParam(void)
{
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::BOSS));

	transform_.scl = { DOG_SIZE, DOG_SIZE, DOG_SIZE };

	transform_.quaRotLocal = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f)
		,AsoUtility::Deg2RadF(0.0f), 0.0f);

	transform_.dir = { 0.0f, 0.0f, 0.0f };						

	hp_ = BOSS_MAX_HP;

	speed_ = 0.0f;		

	isAlive_ = true;	

	collisionRadius_ = COLLOSION_RADIUS;			//衝突判定用の球体半径
	collisionLocalPos_ = COLLISION_POS;				//衝突判定用の球体中心の調整座標

	attackCollisionRadius_ = COLLOSION_RADIUS;		//攻撃判定用と攻撃範囲の球体半径

	enemyType_ = TYPE::BOSS;

	imgBoss_ = resMng_.Load(ResourceManager::SRC::BOSS_IMG).handleId_;

	//初期状態
	ChangeState(STATE::PLAY);
}

void EnemyBoss::DrawBossHpBar(void)
{
	if (!isAlive_)
	{
		return;
	}

	int barX = (Application::SCREEN_SIZE_X - BOSS_HP_BAR_WIDTH) / 2;
	int barY = BOSS_HP_BAR_Y;

	float hpRate = static_cast<float>(hp_) / BOSS_MAX_HP;
	int hpDrawWidth = static_cast<int>(BOSS_HP_BAR_WIDTH * hpRate);

	//ラベル
	//SetFontSize(55);
	//DrawFormatString(barX + BOSS_LABEL_OFFSET_X, barY + BOSS_LABEL_OFFSET_Y, GetColor(0,0,0), "BOSS");
	//SetFontSize(16);

	DrawRotaGraph(barX + BOSS_LABEL_OFFSET_X, barY + BOSS_LABEL_OFFSET_Y, 1.0, 0.0, imgBoss_, true);

	//背景バー（黒）
	DrawBox(barX, barY, barX + BOSS_HP_BAR_WIDTH, barY + BOSS_HP_BAR_HEIGHT, black, true);

	//HPバー（赤）
	DrawBox(barX, barY, barX + hpDrawWidth, barY + BOSS_HP_BAR_HEIGHT, red, true);

	//枠線（白）
	DrawBox(barX, barY, barX + BOSS_HP_BAR_WIDTH, barY + BOSS_HP_BAR_HEIGHT, white, false);
}