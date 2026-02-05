#include "game.h"
#include "SceneMain.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

Game::Game()
{    
    // 初始化指针
    nearStars_ = new Background();
    farStars_ = new Background();
    
    // 初始化其他成员变量
    isRunning_ = true;
    window_ = nullptr;
    renderer_ = nullptr;
    currentScene_ = nullptr;

    // 初始化窗口和帧率参数
    windowWidth_ = 720;
    windowHeight_ = 960;
    fps_ = 60;
}

Game::~Game()
{
    clean();
}

void Game::run()
{
    while(isRunning_){
        auto frameStartTime = SDL_GetTicks();

        SDL_Event event;
        handleEvent(&event);
        update();
        render();

        auto frameEndTime = SDL_GetTicks();
        auto diff = frameEndTime - frameStartTime;

        // 计算帧率
        if(diff < frameTime_){
            SDL_Delay(frameTime_ - diff);
            deltaTime_ = static_cast<float>(frameTime_) / 1000.0f;
        }
        else{
             deltaTime_ = static_cast<float>(diff) / 1000.0f;
        }               
    }
    
    // 输出日志
    SDL_Log("Game loop ended. isRunning = %d", isRunning_);

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, 
                          "Game Exit", 
                          "Game is exiting now.", 
                          window_);
}

void Game::init()
{
    frameTime_ = 1000 / fps_;
    
    // SDL初始化
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s\n", SDL_GetError());
        isRunning_ = false;
        return;
    }
   
    // 创建窗口
    window_ = SDL_CreateWindow("SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth_, windowHeight_, 0);
    if(window_ == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        isRunning_ = false;
        return;
    }

    // 创建渲染器
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(renderer_ == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        isRunning_ = false;
        return;
    }
       
    // SDL_Image初始化
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "IMG_Init Error: %s\n", SDL_GetError());
        isRunning_ = false;
        return;
    }

    // SDL_mixer初始化    
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Mix_Init Error: %s\n", SDL_GetError());
        isRunning_ = false;
        return;
    }

    // 打开音频设备
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Mix_OpenAudio Error: %s\n", SDL_GetError());
        isRunning_ = false;
        return;
    }
    
    Mix_VolumeMusic(64); // 设置音量

    // 初始化背景卷轴
    nearStars_->texture_ = IMG_LoadTexture(renderer_, "assets/image/Stars-A.png");
    if(nearStars_->texture_ == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "IMG_LoadTexture Error: %s\n", SDL_GetError());
        isRunning_ = false;
        return;
    }
    SDL_QueryTexture(nearStars_->texture_, nullptr, nullptr, &nearStars_->width_, &nearStars_->height_);

    farStars_->texture_ = IMG_LoadTexture(renderer_, "assets/image/Stars-B.png");
    if(farStars_->texture_ == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "IMG_LoadTexture Error: %s\n", SDL_GetError());
        isRunning_ = false;
        return;
    }
    SDL_QueryTexture(farStars_->texture_, nullptr, nullptr, &farStars_->width_, &farStars_->height_);
    farStars_->speed_ = 10;
        
    // 创建场景
    currentScene_ = new SceneMain();
    currentScene_->init();
}

void Game::clean()
{
    // 清理场景
    if(currentScene_ != nullptr){
        currentScene_->clean();
        delete currentScene_;
        currentScene_ = nullptr;
    }

    // 清理背景卷轴
    if(nearStars_->texture_ != nullptr){
        SDL_DestroyTexture(nearStars_->texture_);
        nearStars_->texture_ = nullptr;
    }
    
    if(farStars_->texture_ != nullptr){
        SDL_DestroyTexture(farStars_->texture_);
        farStars_->texture_ = nullptr;
    }

    // 清理Background对象
    delete nearStars_;
    nearStars_ = nullptr;
    
    delete farStars_;
    farStars_ = nullptr;

    // SDL_Image清理
    IMG_Quit();

    // SDL_mixer清理
    Mix_CloseAudio();
    Mix_Quit();
    
    // SDL清理
    if(renderer_ != nullptr){
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    
    if(window_ != nullptr){
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    
    SDL_Quit();
}

void Game::changeScene(Scene* scene)
{
    // 清理当前场景
    if(currentScene_ != nullptr){
        currentScene_->clean();
        delete currentScene_;
    }
    
    // 创建新场景并初始化
    currentScene_ = scene;
    currentScene_->init();
}

void Game::handleEvent(SDL_Event* event)
{
    while (SDL_PollEvent(event)){
        if(event->type == SDL_QUIT){ 
            isRunning_ = false;
            return;
        }
        currentScene_->handleEvent(event);
    } 
}

void Game::update()
{
    updateBackGround(deltaTime_);
    currentScene_->update(deltaTime_);
}

void Game::render()
{
    // 清理
    SDL_RenderClear(renderer_);
    
    // 渲染背景
    renderBackGround();
    
    // 渲染场景
    currentScene_->render();
    
    // 更新
    SDL_RenderPresent(renderer_);
}

void Game::updateBackGround(float dt)
{
    nearStars_->offset_ -= nearStars_->speed_ * dt;
    if(nearStars_->offset_ >= 0){
        nearStars_->offset_ = -static_cast<float>(nearStars_->height_);
    }

    farStars_->offset_ -= farStars_->speed_ * dt;
    if(farStars_->offset_ >= 0){
        farStars_->offset_ = -static_cast<float>(farStars_->height_);
    }
}

void Game::renderBackGround()
{
    // 远处的星星
    for(int posY = static_cast<int>(farStars_->offset_); posY < getWindowHeight(); posY += farStars_->height_){
        for(int posX = 0; posX < getWindowWidth(); posX += farStars_->width_){
            SDL_Rect dstRect = {posX, posY, farStars_->width_, farStars_->height_};
            SDL_RenderCopy(renderer_, farStars_->texture_, nullptr, &dstRect);
        }
    }

    // 近处的星星
    for(int posY = static_cast<int>(nearStars_->offset_); posY < getWindowHeight(); posY += nearStars_->height_){
        for(int posX = 0; posX < getWindowWidth(); posX += nearStars_->width_){
            SDL_Rect dstRect = {posX, posY, nearStars_->width_, nearStars_->height_};
            SDL_RenderCopy(renderer_, nearStars_->texture_, nullptr, &dstRect);
        }
    };
}
