#pragma once
#include "SceneBase.h"
#include "../Manager/InputManager.h"

class StageSelectScene : public SceneBase
{
public:
    void Init(void) override;
    void Update(void) override;
    void Draw(void) override;
    void Release(void) override;

private:
    int selectIndex_ = 0;
    static constexpr int maxStage_ = 3;
};
