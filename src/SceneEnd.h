#ifndef SCENE_END_H
#define SCENE_END_H

#include "Scene.h"
#include "Game.h"
#include <SDL.h>
#include <SDL_ttf.h>  
#include <string>
#include <vector>

// 玩家得分记录结构体
struct ScoreRecord {
    std::string playerName;
    int score;
    std::string date; // 记录日期
};

class SceneEnd : public Scene {
public:
    SceneEnd();
    virtual ~SceneEnd();
    
    virtual void init() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void clean() override;
    virtual void handleEvent(SDL_Event* event) override;
    
private:
    Game& game_;
    
    // 场景切换相关
    float sceneSwitchDelay_;
    float sceneSwitchTimer_;
    bool sceneSwitchTriggered_;
    
    // 玩家名字输入相关
    std::string playerName_;
    bool isNameInputActive_;
    int cursorPosition_;
    float cursorBlinkTimer_;
    bool cursorVisible_;
    
    // 得分榜相关
    std::vector<ScoreRecord> highScores_;
    int maxHighScores_;
    bool isScoreboardVisible_;
    
    // 字体和纹理
    TTF_Font* titleFont_;
    TTF_Font* textFont_;
    TTF_Font* scoreFont_;
    
    // 方法
    void renderTitle();
    void renderPlayerNameInput();
    void renderScoreboard();
    void saveHighScores();
    void loadHighScores();
    void addScoreToLeaderboard(int score);
    void handleTextInput(SDL_Event* event);
    void handleBackspace();
    void switchToTitleScene();
};

#endif // SCENE_END_H
