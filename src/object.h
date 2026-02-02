#ifndef OBJECT_H
#define OBJECT_H
#include <SDL.h>
#include <list>

//玩家飞船类
struct Player
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint  position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 300;
    int hp = 3;
    int maxHp = 10;
    int shield = 0;
    int maxShield = 100;
    

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
    int speed = 500;
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
    int speed = 300;
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

//道具类
enum class ItemType{
    HealthPack,
    ShieldPack,
    SkillCDPack    
};

struct Item{
    SDL_Texture *texture = nullptr;
    SDL_FPoint  position = {0, 0};
    SDL_FPoint  direction = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 200;
    int bounceCount = 3;
    ItemType type = ItemType::HealthPack;    
};
    //血包类
    struct HealthPack : public Item{
        HealthPack() {
            type = ItemType::HealthPack;
        }
    };

    //盾包类
    struct ShieldPack : public Item{
        ShieldPack() {
            type = ItemType::ShieldPack;
        }
    };

    //技能包类
    struct SkillCDPack : public Item{
        SkillCDPack() {
            type = ItemType::SkillCDPack;
        }
    };


//技能类
enum class SkillType{
    ShieldReflect,
    Invincible,
    BulletSpeedUp,
    BulletBallisticUp

};

struct Skill{
   SkillType type;
   float cooldDownTime;
   float durationTime;
   float currentCooldownTime;
   float currentDurationTime;
   bool isUsing;  
};
   //盾反技能
   struct ShieldReflect : public Skill{
       float damageReflection;
       bool reflectBulletts;
   };

   //无敌技能
   struct Invincible : public Skill{
       bool invincible;
   };

   //子弹速度提升技能
   struct BulletSpeedUp : public Skill{
       float bulletSpeedUp;
   };

   //子弹弹道提升技能
   struct BulletBallisticUp : public Skill{
       float bulletBallisticUp;
   };

//技能管理
struct SkillManager{
    std::list<Skill*> skills;
    ShieldReflect* shieldReflect;
    Invincible* invincible;
    BulletSpeedUp* bulletSpeedUp;
    BulletBallisticUp* bulletBallisticUp;
};


#endif