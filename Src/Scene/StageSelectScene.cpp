#include "StageSelectScene.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include <DxLib.h>

void StageSelectScene::Init(void)
{
    selectIndex_ = 0;
    //画像
    imgPlzSelectStage_ = resMng_.Load(ResourceManager::SRC::PLZ_SELECT_STAGE).handleId_;
    imgBackGround_ = LoadGraph("Data/Image/SeleStageGame.png");
    imgStage1_ = resMng_.Load(ResourceManager::SRC::STAGE_1).handleId_;
    imgStage2_ = resMng_.Load(ResourceManager::SRC::STAGE_2).handleId_;
    imgStage3_ = resMng_.Load(ResourceManager::SRC::STAGE_3).handleId_;
    imgStage4_ = resMng_.Load(ResourceManager::SRC::STAGE_4).handleId_;
    imgStage5_ = resMng_.Load(ResourceManager::SRC::STAGE_5).handleId_;
    imgSelectUI_ = resMng_.Load(ResourceManager::SRC::SELECT_UI).handleId_;
}

void StageSelectScene::Update(void)
{
    InputManager& ins = InputManager::GetInstance();

    if (ins.IsTrgDown(KEY_INPUT_DOWN) ||
        ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_DOWN))
        selectIndex_ = (selectIndex_ + 1) % maxStage_;

    if (ins.IsTrgDown(KEY_INPUT_UP) ||
        ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_TOP))
        selectIndex_ = (selectIndex_ - 1 + maxStage_) % maxStage_;

    //決定
    if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
        ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
    {
        SceneManager::GetInstance().ChangeStageScene(
            SceneManager::SCENE_ID::GAME,
            selectIndex_ + 1
        );
    }
}

void StageSelectScene::Draw(void)
{
    DrawGraph(0, 0, imgBackGround_, true);

    DrawGraph(230, 0, imgPlzSelectStage_, true);

    // ステージ座標配列
    const int stagePosX[5] = { 200, 580, 960, 1340, 1720 };
    const int stagePosY[5] = { 350, 850, 350, 850, 350 };

    float alpha2 = (sinf(GetNowCount() * BLINK_SPEED) + 1.0f) * 0.5f;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(alpha2 * 255));

    // 選択UIをステージ画像の位置に合わせて描画
    DrawRotaGraph(stagePosX[selectIndex_], stagePosY[selectIndex_], 1.0, 0.0, imgSelectUI_, true);

    //ブレンドモード解除
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // ステージ画像描画
    DrawRotaGraph(stagePosX[0], stagePosY[0], 1.0, 0.0, imgStage1_, true);
    DrawRotaGraph(stagePosX[1], stagePosY[1], 1.0, 0.0, imgStage2_, true);
    DrawRotaGraph(stagePosX[2], stagePosY[2], 1.0, 0.0, imgStage3_, true);
    DrawRotaGraph(stagePosX[3], stagePosY[3], 1.0, 0.0, imgStage4_, true);
    DrawRotaGraph(stagePosX[4], stagePosY[4], 1.0, 0.0, imgStage5_, true);
}

void StageSelectScene::Release(void)
{
}
