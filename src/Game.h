#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include "scene.h"
#include "object.h"

/**
 * @brief 游戏主类，采用单例模式
 */
class Game {
public:
    /**
     * @brief 获取游戏单例实例
     * @return 游戏实例引用
     */
    static Game& getInstance() {
        static Game instance;
        return instance;
    }

    /**
     * @brief 析构函数
     */
    ~Game();

    /**
     * @brief 运行游戏主循环
     */
    void run();
    
    /**
     * @brief 初始化游戏
     */
    void init();
    
    /**
     * @brief 清理游戏资源
     */
    void clean();
    
    /**
     * @brief 切换场景
     * @param scene 新场景指针
     */
    void changeScene(Scene* scene);

    /**
     * @brief 更新游戏状态
     */
    void update();
    
    /**
     * @brief 渲染游戏画面
     */
    void render();
    
    /**
     * @brief 处理输入事件
     * @param event SDL事件指针
     */
    void handleEvent(SDL_Event* event);

    /**
     * @brief 更新背景
     * @param deltaTime 时间增量
     */
    void updateBackGround(float deltaTime);
    
    /**
     * @brief 渲染背景
     */
    void renderBackGround();

    /**
     * @brief 获取窗口指针
     * @return SDL窗口指针
     */
    SDL_Window* getWindow() { return window_; }
    
    /**
     * @brief 获取渲染器指针
     * @return SDL渲染器指针
     */
    SDL_Renderer* getRenderer() { return renderer_; }

    /**
     * @brief 获取窗口宽度
     * @return 窗口宽度
     */
    int getWindowWidth() { return windowWidth_; }
    
    /**
     * @brief 获取窗口高度
     * @return 窗口高度
     */
    int getWindowHeight() { return windowHeight_; }    

    /**
     * @brief 退出游戏
     */
    void quit();

    /**
     * @brief 获取得分
     * @return 当前得分
     */
    int getScore() const { return score_; }
    
    /**
     * @brief 设置得分
     * @param score 新的得分值
     */
    void setScore(int score) { score_ = score; }

private:
    /**
     * @brief 私有构造函数，实现单例模式
     */
    Game();
    
    /**
     * @brief 禁用拷贝构造函数
     */
    Game(const Game&) = delete;
    
    /**
     * @brief 禁用赋值运算符
     */
    Game& operator=(const Game&) = delete;
    
    bool isRunning_;              // 游戏是否运行中
    Scene* currentScene_;         // 当前场景
    SDL_Window* window_;          // SDL窗口
    SDL_Renderer* renderer_;      // SDL渲染器

    int windowWidth_;             // 窗口宽度
    int windowHeight_;            // 窗口高度

    int fps_;                     // 目标帧率
    Uint32 frameTime_;            // 每帧时间（毫秒）
    float deltaTime_;             // 时间增量（秒）

    Background* nearStars_;       // 近景星星
    Background* farStars_;        // 远景星星

    int score_;                   // 得分
};

#endif  // GAME_H
