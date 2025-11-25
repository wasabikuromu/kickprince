#pragma once
#include "SceneBase.h"
#include "../Manager/InputManager.h"

class StageSelectScene : public SceneBase
{
public:
    static constexpr float BLINK_SPEED = 0.006f;

    void Init(void) override;
    void Update(void) override;
    void Draw(void) override;
    void Release(void) override;

private:
    int selectIndex_ = 0;

    static constexpr int maxStage_ = 5;

    //‰æ‘œ
    int imgPlzSelectStage_;
    int imgStage1_;
    int imgStage2_;
    int imgStage3_;
    int imgStage4_;
    int imgStage5_;
    int imgSelectUI_;
};
