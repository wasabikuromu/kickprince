#pragma once
#include "SceneBase.h"
#include "../Manager/InputManager.h"

class StageSelectScene : public SceneBase
{
public:
    static constexpr float BLINK_SPEED = 0.006f;    //点滅

    static constexpr int   SELEXT_X = 230;          //ステージ選択のX

    static constexpr int STAGE_COUNT = 5;           //ステージ数

    static constexpr int STAGE_1_X = 200;           //ステージ1のX
    static constexpr int STAGE_2_X = 580;           //ステージ2のX
    static constexpr int STAGE_3_X = 960;           //ステージ3のX
    static constexpr int STAGE_4_X = 1340;          //ステージ4のX
    static constexpr int STAGE_5_X = 1720;          //ステージ5のX
    static constexpr int STAGE_UP_Y = 350;          //ステージの高いほうY
    static constexpr int STAGE_DOWN_Y = 850;        //ステージの低いほうY    

    void Init(void) override;
    void Update(void) override;
    void Draw(void) override;
    void Release(void) override;

private:
    int selectIndex_ = 0;

    static constexpr int maxStage_ = 5;

    //画像
    int imgPlzSelectStage_;
    int imgBackGround_;
    int imgStage1_;
    int imgStage2_;
    int imgStage3_;
    int imgStage4_;
    int imgStage5_;
    int imgSelectUI_;
};
