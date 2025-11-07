#include "Shot.h"
#include "../Manager/ResourceManager.h"
#include "EnemyBase.h"
#include "../Utility/AsoUtility.h"

Shot::Shot(VECTOR pos, VECTOR dir, float speed, float damage)
    : pos_(pos), dir_(dir), speed_(speed), damage_(damage),
    radius_(DEFAULT_RADIUS), isDead_(false)
{
    modelId_ = -1; // 無効ID
}

void Shot::Update()
{
    pos_ = VAdd(pos_, VScale(dir_, speed_));

    // 範囲外になったら削除
    if (pos_.z > MAX_DISTANCE || pos_.y < -100.0f)
        isDead_ = true;
}

void Shot::Draw()
{
    SetDrawBlendMode(DX_BLENDMODE_ADD, 255);

    unsigned int color = GetColor(100, 200, 255);
    DrawSphere3D(pos_, radius_, 16, color, TRUE, true);

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

bool Shot::CheckCollision(EnemyBase* enemy)
{
    if (!enemy || !enemy->IsAlive()) return false;

    VECTOR enemyPos = enemy->GetCollisionPos();
    float enemyRadius = enemy->GetCollisionRadius();

    if (AsoUtility::IsHitSpheres(pos_, radius_, enemyPos, enemyRadius))
    {
        enemy->Damage(damage_);
        isDead_ = true; // 1体ヒットで消える
        return true;
    }
    return false;
}

//ターゲット設定
void Shot::SetTarget(const std::vector<std::shared_ptr<EnemyBase>>& enemies)
{
    float minDist = 999999.0f;
    VECTOR nearestDir = dir_; //デフォルトは元の方向

    for (const auto& enemy : enemies)
    {
        if (!enemy || !enemy->IsAlive()) continue;

        VECTOR toEnemy = VSub(enemy->GetCollisionPos(), pos_);
        float dist = VSize(toEnemy);
        if (dist < minDist)
        {
            minDist = dist;
            nearestDir = VNorm(toEnemy);
        }
    }

    dir_ = nearestDir; //一番近い敵の方向に差し替え
}
