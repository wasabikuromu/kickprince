#include "StageSelectScene.h"
#include "../Manager/SoundManager.h"
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

    //音楽
    SoundManager::GetInstance().Play(SoundManager::SRC::STAGE_SELECT_BGM, Sound::TIMES::LOOP);
}

void StageSelectScene::Update(void)
{
    InputManager& ins = InputManager::GetInstance();

    if (ins.IsTrgDown(KEY_INPUT_DOWN) ||
        ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_DOWN))
    {
        SoundManager::GetInstance().Play(SoundManager::SRC::CURSOR_MOVE_SE, Sound::TIMES::ONCE);
        selectIndex_ = (selectIndex_ + 1) % maxStage_;
    }
       
    if (ins.IsTrgDown(KEY_INPUT_UP) ||
        ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::D_TOP))
    {
        SoundManager::GetInstance().Play(SoundManager::SRC::CURSOR_MOVE_SE, Sound::TIMES::ONCE);
        selectIndex_ = (selectIndex_ - 1 + maxStage_) % maxStage_;
    }
        
    //決定
    if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
        ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
    {
        SoundManager::GetInstance().Play(SoundManager::SRC::SET_SE, Sound::TIMES::ONCE);
        SceneManager::GetInstance().ChangeStageScene(
            SceneManager::SCENE_ID::GAME,
            selectIndex_ + 1
        );
    }
}

void StageSelectScene::Draw(void)
{

    //背景・固定UI描画
    DrawGraph(0, 0, imgBackGround_, true);
    DrawGraph(SELEXT_X, 0, imgPlzSelectStage_, true);

    //ステージ配置座標
    const int stagePosX[STAGE_COUNT] = { STAGE_1_X, STAGE_2_X, STAGE_3_X, STAGE_4_X, STAGE_5_X };
    const int stagePosY[STAGE_COUNT] = { STAGE_UP_Y, STAGE_DOWN_Y, STAGE_UP_Y, STAGE_DOWN_Y, STAGE_UP_Y };

    //ステージ画像配列
    const int stageImgs[STAGE_COUNT] =
    {
        imgStage1_,
        imgStage2_,
        imgStage3_,
        imgStage4_,
        imgStage5_
    };

    //選択UIの点滅演出
    const float blinkAlpha =(sinf(GetNowCount() * BLINK_SPEED) + 1.0f) * 0.5f;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA,static_cast<int>(blinkAlpha * 255.0f));

    //選択中ステージ位置にUIを描画
    DrawRotaGraph(stagePosX[selectIndex_], stagePosY[selectIndex_], 1.0, 0.0, imgSelectUI_, true);

    //ブレンドモード解除
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

    //ステージ画像描画
    for (int i = 0; i < STAGE_COUNT; ++i)
    {
        DrawRotaGraph(stagePosX[i], stagePosY[i], 1.0, 0.0, stageImgs[i], true);
    }
}

void StageSelectScene::Release(void)
{
    SoundManager::GetInstance().Stop(SoundManager::SRC::STAGE_SELECT_BGM);
}
