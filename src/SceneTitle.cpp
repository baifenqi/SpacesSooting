// src/SceneTitle.cpp
#include "SceneTitle.h"
#include "SceneMain.h"  
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

SceneTitle::SceneTitle() : game_(Game::getInstance()) {
    selectedOption_ = START_GAME;
    keyPressed_ = false;
    keyCooldown_ = 0.0f;
    backgroundOffset_ = 0.0f;
    backgroundSpeed_ = 50.0f; // 背景滚动速度
    titlePulse_ = 0.0f;
    titlePulseSpeed_ = 2.0f; // 标题脉动速度
    
    // 初始化随机数种子
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // 初始化星星背景
    for (int i = 0; i < 100; i++) {
        Star star;
        star.x = static_cast<float>(std::rand() % game_.getWindowWidth());
        star.y = static_cast<float>(std::rand() % game_.getWindowHeight());
        star.size = static_cast<float>(std::rand() % 3 + 1);
        star.brightness = static_cast<float>(std::rand() % 100) / 100.0f;
        star.twinkleSpeed = static_cast<float>(std::rand() % 100 + 50) / 100.0f;
        stars_.push_back(star);
    }
}

SceneTitle::~SceneTitle() {
    clean();
}

void SceneTitle::init() {
    
}

void SceneTitle::update(float deltaTime) {
    // 更新背景动画
    backgroundOffset_ += backgroundSpeed_ * deltaTime;
    
    // 更新标题脉动效果
    titlePulse_ += titlePulseSpeed_ * deltaTime;
    if (titlePulse_ > 2.0f * M_PI) {
        titlePulse_ -= static_cast<float>(2.0f * M_PI);
    }
    
    // 更新星星闪烁
    for (auto& star : stars_) {
        star.brightness += star.twinkleSpeed * deltaTime;
        if (star.brightness > 1.0f || star.brightness < 0.2f) {
            star.twinkleSpeed *= -1;
        }
    }
    
    // 更新按键冷却
    if (keyPressed_) {
        keyCooldown_ -= deltaTime;
        if (keyCooldown_ <= 0) {
            keyPressed_ = false;
        }
    }
}

void SceneTitle::render() {
    // 清空屏幕为深蓝色背景
    SDL_SetRenderDrawColor(game_.getRenderer(), 10, 10, 40, 255);
    SDL_RenderClear(game_.getRenderer());
    
    // 绘制星星背景
    for (const auto& star : stars_) {
        Uint8 brightness = static_cast<Uint8>(star.brightness * 255);
        SDL_SetRenderDrawColor(game_.getRenderer(), brightness, brightness, brightness, 255);
        
        SDL_Rect starRect = {
            static_cast<int>(star.x),
            static_cast<int>(star.y),
            static_cast<int>(star.size),
            static_cast<int>(star.size)
        };
        
        SDL_RenderFillRect(game_.getRenderer(), &starRect);
    }
    
    // 重置渲染颜色为白色，避免影响后续绘制
    SDL_SetRenderDrawColor(game_.getRenderer(), 255, 255, 255, 255);
    
    // 绘制标题
    TTF_Font* titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 48);
    if (titleFont != nullptr) {
        // 计算标题脉动效果
        float scale = 1.0f + 0.05f * sin(titlePulse_);
        
        // 渲染标题文字
        const char* titleText = "SPACE SHOOTER";
        SDL_Color titleColor = {255, 255, 255, 255};
        
        SDL_Surface* titleSurface = TTF_RenderText_Solid(titleFont, titleText, titleColor);
        if (titleSurface != nullptr) {
            SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(game_.getRenderer(), titleSurface);
            
            int titleWidth = titleSurface->w;
            int titleHeight = titleSurface->h;
            
            // 应用脉动效果
            SDL_Rect titleRect = {
                static_cast<int>((game_.getWindowWidth() - titleWidth * scale) / 2),
                static_cast<int>(100 + (1.0f - scale) * 50), // 脉动时轻微上下移动
                static_cast<int>(titleWidth * scale),
                static_cast<int>(titleHeight * scale)
            };
            
            SDL_RenderCopy(game_.getRenderer(), titleTexture, nullptr, &titleRect);
            
            SDL_DestroyTexture(titleTexture);
            SDL_FreeSurface(titleSurface);
        }
        
        TTF_CloseFont(titleFont);
    }
    
    // 渲染菜单选项
    TTF_Font* menuFont = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 24);
    if (menuFont != nullptr) {
        const char* menuOptions[] = {"Start Game", "Quit"};
        SDL_Color normalColor = {255, 255, 255, 255};
        SDL_Color selectedColor = {255, 255, 0, 255}; // 黄色表示选中
        
        for (int i = 0; i < NUM_OPTIONS; i++) {
            SDL_Color color = (i == selectedOption_) ? selectedColor : normalColor;
            
            SDL_Surface* textSurface = TTF_RenderText_Solid(menuFont, menuOptions[i], color);
            if (textSurface != nullptr) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(game_.getRenderer(), textSurface);
                
                int textWidth = textSurface->w;
                int textHeight = textSurface->h;
                
                // 为选中的菜单项添加一个小箭头
                int arrowOffset = 0;
                if (i == selectedOption_) {
                    arrowOffset = 20;
                    
                    // 绘制小箭头
                    SDL_SetRenderDrawColor(game_.getRenderer(), 255, 255, 0, 255);
                    SDL_Point arrowPoints[] = {
                        {static_cast<int>((game_.getWindowWidth() - textWidth) / 2 - arrowOffset), 
                         static_cast<int>(400 + i * 60 + textHeight / 2)},
                        {static_cast<int>((game_.getWindowWidth() - textWidth) / 2 - arrowOffset + 10), 
                         static_cast<int>(400 + i * 60 + textHeight / 2 - 5)},
                        {static_cast<int>((game_.getWindowWidth() - textWidth) / 2 - arrowOffset + 10), 
                         static_cast<int>(400 + i * 60 + textHeight / 2 + 5)}
                    };
                    SDL_RenderDrawLines(game_.getRenderer(), arrowPoints, 3);
                    
                    // 重置渲染颜色为白色，避免影响后续绘制
                    SDL_SetRenderDrawColor(game_.getRenderer(), 255, 255, 255, 255);
                }
                
                SDL_Rect textRect = {
                    (game_.getWindowWidth() - textWidth) / 2,
                    400 + i * 60, // 菜单项之间的间距
                    textWidth,
                    textHeight
                };
                
                SDL_RenderCopy(game_.getRenderer(), textTexture, nullptr, &textRect);
                
                SDL_DestroyTexture(textTexture);
                SDL_FreeSurface(textSurface);
            }
        }
        
        TTF_CloseFont(menuFont);
    }
}

void SceneTitle::handleEvent(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN && !keyPressed_) {
        // 使用ASCII码作为键码的替代方案
        int key = event->key.keysym.sym;
        
        // 上下键和WSAD键都可以控制菜单选择
        if (key == SDLK_UP || key == 'w' || key == 'W') {
            selectedOption_ = (selectedOption_ - 1 + NUM_OPTIONS) % NUM_OPTIONS;
            keyPressed_ = true;
            keyCooldown_ = 0.2f;
        }
        else if (key == SDLK_DOWN || key == 's' || key == 'S') {
            selectedOption_ = (selectedOption_ + 1) % NUM_OPTIONS;
            keyPressed_ = true;
            keyCooldown_ = 0.2f;
        }
        else if (key == SDLK_RETURN || key == SDLK_SPACE || key == SDLK_KP_ENTER) {
            // 执行选中的菜单选项
            switch (selectedOption_) {
                case START_GAME:
                    // 在切换场景前重置渲染颜色为默认值
                    SDL_SetRenderDrawColor(game_.getRenderer(), 0, 0, 0, 255);
                    game_.changeScene(new SceneMain());
                    break;
                    
                case QUIT:
                    game_.quit();
                    break;
            }
        }
    }
}

void SceneTitle::clean() {
    // 在清理场景前重置渲染颜色为默认值
    SDL_SetRenderDrawColor(game_.getRenderer(), 0, 0, 0, 255);
}
