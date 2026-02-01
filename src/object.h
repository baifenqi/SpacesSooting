#ifndef OBJECT_H
#define OBJECT_H
#include <SDL.h>

//玩家飞船类
struct Player
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint  position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 300;
    int hp = 3;

    Uint32 coolDown = 500;
    Uint32 lastShotTime = 0;
};

//玩家子弹类
struct PlayerBullet
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint  position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 400;
    int damage = 1;
};

//敌人飞船类
struct Enemy{
    SDL_Texture *texture = nullptr;
    SDL_FPoint  position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 150;
    int hp = 2;

    Uint32 coolDown = 3000;
    Uint32 lastShotTime = 0;
};

//敌人子弹类
struct EnemyBullet{
    SDL_Texture *texture = nullptr;
    SDL_FPoint  position = {0, 0};
    SDL_FPoint  direction = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 400;
    int damage = 1;
};


//爆炸纹理类
struct Explosion{
    SDL_Texture *texture = nullptr;
    SDL_FPoint  position = {0, 0};
    int width = 0;
    int height = 0;
    int currentFrame = 0;
    int totalFrame = 0;

    Uint32 startTime = 0;
    Uint32 FPS = 10;
};

enum class ItemType{
    HealthPack,
    ShieldPack,
    CdPack    
};

struct Item{
    SDL_Texture *texture = nullptr;
    SDL_FPoint  position = {0, 0};
    SDL_FPoint  direction = {0, 0};
    int width = 0;
    int height = 0;
    ItemType type = ItemType::HealthPack;    
};




#endif