#include "StageSelectScene.h"
#include "../Manager/SceneManager.h"
#include <DxLib.h>

void StageSelectScene::Init(void)
{
    selectIndex_ = 0;
}

void StageSelectScene::Update(void)
{
    InputManager& ins = InputManager::GetInstance();

    if (ins.IsTrgDown(KEY_INPUT_DOWN))
        selectIndex_ = (selectIndex_ + 1) % maxStage_;

    if (ins.IsTrgDown(KEY_INPUT_UP))
        selectIndex_ = (selectIndex_ - 1 + maxStage_) % maxStage_;

    // 決定
    if (ins.IsTrgDown(KEY_INPUT_RETURN))
    {
        SceneManager::GetInstance().ChangeStageScene(
            SceneManager::SCENE_ID::GAME,
            selectIndex_ + 1
        );
    }
}

void StageSelectScene::Draw(void)
{
    DrawString(100, 50, "ステージを選択してください", GetColor(255, 255, 255));

    for (int i = 0; i < maxStage_; i++)
    {
        int y = 150 + i * 40;

        if (i == selectIndex_)
            DrawString(80, y, "→", GetColor(255, 255, 0));

        DrawFormatString(120, y, GetColor(255, 255, 255), "ステージ %d", i + 1);
    }
}

void StageSelectScene::Release(void)
{
}
