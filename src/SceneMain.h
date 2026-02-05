#ifndef SCENEMAIN_H
#define SCENEMAIN_H

#include <SDL.h>
#include <SDL_mixer.h>
#include "Scene.h"
#include "object.h"
#include "Game.h"

#include <list>
#include <random>
#include <map>

class Game;

/**
 * @brief 主游戏场景类
 */
class SceneMain : public Scene {
public:
    /**
     * @brief 构造函数
     */
    SceneMain();
    
    /**
     * @brief 析构函数
     */
    ~SceneMain();
    
    /**
     * @brief 更新场景
     * @param deltaTime 时间增量
     */
    void update(float deltaTime) override;
    
    /**
     * @brief 渲染场景
     */
    void render() override;
    
    /**
     * @brief 处理输入事件
     * @param event SDL事件指针
     */
    void handleEvent(SDL_Event* event) override;
    
    /**
     * @brief 初始化场景
     */
    void init() override;
    
    /**
     * @brief 清理场景资源
     */
    void clean() override;
    
    // 键盘控制
    /**
     * @brief 处理键盘输入
     * @param deltaTime 时间增量
     */
    void keyboardControl(float deltaTime);
    
    // 玩家控制
    /**
     * @brief 更新玩家状态
     */
    void updatePlayer();
    
    // 玩家子弹控制
    /**
     * @brief 创建玩家子弹
     */
    void createPlayerBullets();
    
    /**
     * @brief 更新玩家子弹
     * @param deltaTime 时间增量
     */
    void updatePlayerBullets(float deltaTime);
    
    /**
     * @brief 渲染玩家子弹
     */
    void renderPlayerBullets();
    
    // 敌人控制
    /**
     * @brief 生成敌人
     */
    void spawnEnemy();
    
    /**
     * @brief 更新敌人
     * @param deltaTime 时间增量
     */
    void updateEnemies(float deltaTime);
    
    /**
     * @brief 渲染敌人
     */
    void renderEnemies();
    
    // 敌人子弹控制
    /**
     * @brief 创建敌人子弹
     * @param enemy 敌人指针
     */
    void createEnemyBullets(Enemy* enemy);
    
    /**
     * @brief 获取敌人子弹方向
     * @param enemy 敌人指针
     * @return 子弹方向
     */
    SDL_FPoint getEnemyBulletDirection(Enemy* enemy);
    
    /**
     * @brief 更新敌人子弹
     * @param deltaTime 时间增量
     */
    void updateEnemyBullets(float deltaTime);
    
    /**
     * @brief 渲染敌人子弹
     */
    void renderEnemyBullets();

    // 爆炸效果
    /**
     * @brief 敌人爆炸
     * @param enemy 敌人指针
     */
    void enemyExplode(Enemy* enemy);
    
    /**
     * @brief 更新爆炸效果
     * @param deltaTime 时间增量
     */
    void updateExplosions(float deltaTime);
    
    /**
     * @brief 渲染爆炸效果
     */
    void renderExplosions();

    // 道具控制
    /**
     * @brief 敌人死亡掉落道具
     * @param enemy 敌人指针
     */
    void dropItem(Enemy* enemy);
    
    /**
     * @brief 玩家获取道具
     * @param item 道具指针
     */
    void playerGetItem(Item* item);
    
    /**
     * @brief 更新道具
     * @param deltaTime 时间增量
     */
    void updateItems(float deltaTime);
    
    /**
     * @brief 渲染道具
     */
    void renderItems();

    // 技能控制
    /**
     * @brief 初始化技能
     */
    void initSkill();
    
    /**
     * @brief 激活技能
     * @param skillType 技能类型
     */
    void activateSkill(SkillType skillType);
    
    /**
     * @brief 更新技能
     * @param deltaTime 时间增量
     */
    void updateSkill(float deltaTime);
    
    /**
     * @brief 渲染技能特效
     */
    void renderSkill();
    
    /**
     * @brief 技能CD包效果
     */
    void skillCDPackEffect();
    
private:   
    Game& game_;                           // 游戏实例引用
    bool isDeath_;                          // 玩家是否死亡

    // 背景音乐
    Mix_Music* bgm_;                       // 背景音乐

    // 随机数生成器
    std::mt19937 gen_;                     // 随机数生成器
    std::uniform_real_distribution<float> dis_; // 均匀分布

    // 技能管理器
    SkillManager skillManager_;            // 技能管理器

    // 帧率监控器
    Uint32 frameCount_;                    // 帧计数
    Uint32 lastFpsUpdateTime_;            // 上次更新FPS时间
    float currentFPS_;                     // 当前FPS

    // 音效资源
    std::map<std::string, Mix_Chunk*> soundEffectMap_; // 音效映射表

    // 玩家相关
    Player player_;                        // 玩家飞船
    PlayerBullet playerBulletTemplate_;   // 玩家子弹模板
    std::list<PlayerBullet*> playerBullets_; // 玩家子弹列表
    
    // 敌人相关
    Enemy enemyTemplate_;                 // 敌人模板
    std::list<Enemy*> enemies_;            // 敌人列表
    EnemyBullet enemyBulletTemplate_;     // 敌人子弹模板
    std::list<EnemyBullet*> enemyBullets_; // 敌人子弹列表

    // 爆炸效果
    Explosion explosionTemplate_;         // 爆炸模板
    std::list<Explosion*> explosions_;     // 爆炸列表

    // 道具
    Item itemHealthPackTemplate_;         // 血包模板
    Item itemShieldPackTemplate_;         // 盾包模板
    Item itemSkillCDPackTemplate_;        // 技能CD包模板
    std::list<Item*> items_;              // 道具列表

    // 技能特效
    float skillPulseTime_;                // 技能脉冲动画时间
    float invincibleEffectTime_;          // 无敌技能帧动画时间
    
    float shieldEffectFrameTime_;         // 盾反技能帧动画时间
    int shieldEffectCurrentFrame_;        // 盾反技能当前帧
    SDL_Texture* shieldEffectTexture_;    // 盾反技能特效纹理

    /**
     * @brief 绘制圆形的辅助函数
     * @param renderer 渲染器指针
     * @param centerX 圆心X坐标
     * @param centerY 圆心Y坐标
     * @param radius 半径
     */
    void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
};

#endif
