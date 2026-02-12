// src/SceneTitle.h
#ifndef SCENE_TITLE_H
#define SCENE_TITLE_H

#include "Scene.h"
#include "Game.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

class SceneTitle : public Scene {
public:
    SceneTitle();
    ~SceneTitle();
    
    void init() override;
    void update(float deltaTime) override;
    void render() override;
    void handleEvent(SDL_Event* event) override;
    void clean() override;

private:
    Game& game_;
    
    // 菜单选项
    enum MenuOption {
        START_GAME,
        QUIT,
        NUM_OPTIONS
    };
    
    int selectedOption_;
    bool keyPressed_;
    float keyCooldown_;
    
    // 背景动画
    float backgroundOffset_;
    float backgroundSpeed_;
    
    // 星星背景
    struct Star {
        float x, y;
        float size;
        float brightness;
        float twinkleSpeed;
    };
    std::vector<Star> stars_;
    
    // 标题动画
    float titlePulse_;
    float titlePulseSpeed_;
};

#endif // SCENE_TITLE_H
