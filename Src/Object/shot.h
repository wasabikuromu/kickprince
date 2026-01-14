#pragma once
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/ResourceManager.h"

class EnemyBase;

class Shot
{
public:
    static constexpr float DEFAULT_RADIUS = 150.0f;         //íeÇÃîºåa
    static constexpr float DEFAULT_SPEED = 40.0f;           //íeë¨
    static constexpr float MAX_DISTANCE = 5000.0f;          //è¡Ç¶ÇÈãóó£

    static constexpr int SHOT_ALPHA = 128;                  //îºìßñæ
    static constexpr int ALPHA = 255;                       //ïsìßñæ
    static constexpr int SHOT_SPHERE_DIVISION = 16;         //â~ÇÃäp
    static constexpr unsigned int SHOT_COLOR = 0x00FFFF;    //ÉVÉAÉì

    Shot(VECTOR pos, VECTOR dir, float speed, float damage);

    void Update();
    void Draw();
    bool CheckCollision(EnemyBase* enemy);
    bool IsDead() const { return isDead_; }

    //É^Å[ÉQÉbÉgê›íËóp
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