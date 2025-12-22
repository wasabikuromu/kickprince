#pragma once
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/ResourceManager.h"

class EnemyBase; // 前方宣言

class Shot
{
public:
    static constexpr float DEFAULT_RADIUS = 150.0f;      //弾の半径
    static constexpr float DEFAULT_SPEED = 40.0f;       //弾速
    static constexpr float MAX_DISTANCE = 5000.0f;      //消える距離

    Shot(VECTOR pos, VECTOR dir, float speed, float damage);

    void Update();
    void Draw();
    bool CheckCollision(EnemyBase* enemy);
    bool IsDead() const { return isDead_; }

    //ターゲット設定用
    void SetTarget(const std::vector<std::shared_ptr<EnemyBase>>& enemies);

private:
    VECTOR pos_;
    VECTOR dir_;
    float speed_;
    float damage_;
    float radius_;
    bool isDead_;
    int modelId_;
};