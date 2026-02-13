#ifndef SCENE_H
#define SCENE_H

#include <SDL.h>

/**
 * @brief 场景基类，定义了所有场景必须实现的接口
 */
class Scene {
public:
    Scene() = default;
    virtual ~Scene() = default;
    
    /**
     * @brief 初始化场景
     */
    virtual void init() = 0;
    
    /**
     * @brief 更新场景状态
     * @param deltaTime 时间增量（秒）
     */
    virtual void update(float deltaTime) = 0;
    
    /**
     * @brief 渲染场景
     */
    virtual void render() = 0;
    
    /**
     * @brief 清理场景资源
     */
    virtual void clean() = 0;
    
    /**
     * @brief 处理输入事件
     * @param event SDL事件指针
     */
    virtual void handleEvent(SDL_Event* event) = 0;
};

#endif // SCENE_H
