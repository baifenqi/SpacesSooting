#ifndef SCENEMAIN_H
#define SCENEMAIN_H

#include <SDL.h>
#include "Scene.h"
#include "object.h"
#include "Game.h"

#include <list>
#include <random>

class Game;
class SceneMain : public Scene{
public:
    //主控制器
    SceneMain();
    ~SceneMain();
    void update(float deltaTime) override;
    void render() override;
    void handleEvent(SDL_Event* event) override;
    void init() override;
    void clean() override;
    //键盘控制
    void keyboardControl(float deltaTime);
    //玩家控制
    void updatePlayer();
    //玩家子弹控制
    void playerbulletControl();
    void updateplayerbullets(float deltaTime);
    void renderPlayerBullets();
    //敌人控制
    void spawnEnemy();
    void updateEnemies(float deltaTime);
    void renderEnemies();
    //敌人子弹控制
    void enemybulletControl(Enemy* enemy);
    SDL_FPoint getEnemyBulletDirecition(Enemy* enemy);
    void updateEnemyBullets(float deltaTime);
    void renderEnemyBullets();

    //敌机爆炸
    void enemyExplode(Enemy* enemy);
    void updateExplosions(float deltaTime);
    void renderExplosions();
private:   
    Game& game;
    bool isDeath = false;//玩家死亡?

    //随机数生成器
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;

    //玩家飞船
    Player player;
    //玩家子弹模板和容器
    PlayerBullet playerBulletTemplate;
    std::list<PlayerBullet*> playerBullets;
    
    //敌人飞船模板和容器
    Enemy enemyTemplate;
    std::list<Enemy*> enemies;
    //敌人子弹模板和容器
    EnemyBullet enemyBulletTemplate;
    std::list<EnemyBullet*> enemyBullets;

    //爆炸纹理模板和容器
    Explosion explosionTemplate;
    std::list<Explosion*> explosions;


};

#endif