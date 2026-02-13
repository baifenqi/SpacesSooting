#ifndef OBJECT_H
#define OBJECT_H

#include <SDL.h>
#include <list>
#include <memory>

// 玩家飞船类
struct Player {
    SDL_Texture* texture_ = nullptr;
    SDL_FPoint position_ = {0, 0};
    int width_ = 0;
    int height_ = 0;
    int speed_ = 300;
    int hp_ = 3;
    int maxHp_ = 10;
    int shield_ = 0;
    int maxShield_ = 100;
    
    Uint32 coolDown_ = 500;
    Uint32 lastShotTime_ = 0;
};

// 玩家子弹类
struct PlayerBullet {
    SDL_Texture* texture_ = nullptr;
    SDL_FPoint position_ = {0, 0};
    SDL_FPoint direction_ = {0, -1};
    int width_ = 0;
    int height_ = 0;
    int speed_ = 500;
    int damage_ = 1;
};

// 敌人飞船类
enum class EnemyType {
    Normal,
    Ramming    
};

class Enemy {
    public:
        Enemy() = default;
        virtual ~Enemy() = default;

        SDL_Texture* texture_ = nullptr;
        SDL_FPoint position_ = {0, 0};
        int hitCooldown_ = 0;
        int width_ = 0;
        int height_ = 0;
        int speed_ = 150;
        int hp_ = 2;
    
        Uint32 coolDown_ = 3000;
        Uint32 lastShotTime_ = 0;

        EnemyType type_ = EnemyType::Normal;

        virtual void update( [[maybe_unused]] float deltaTime) {};
        virtual void onCollosionWithPlayer(){}
};

// 普通敌人
struct NormalEnemy : public Enemy {
    public:
        NormalEnemy() {
            type_ = EnemyType::Normal;
            hp_ = 2;
            speed_ = 150;
            coolDown_ = 3000;
        }

        void update(float deltaTime) override {
            position_.y += speed_ * deltaTime;
        }

        void onCollosionWithPlayer() override {
            
        } 
};

// 撞击敌人
class RammingEnemy : public Enemy {
    public:
        RammingEnemy() {
            type_ = EnemyType::Ramming;
            hp_ = 3;
            speed_ = 300;
            coolDown_ = 0;
            damage_ = 2;
        }

        void update(float deltaTime) override {
            position_.y += speed_ * deltaTime;
        }

        void onCollosionWithPlayer() override {
        
    }

    int damage_ = 2;
};

// 敌人子弹类
struct EnemyBullet {
    SDL_Texture* texture_ = nullptr;
    SDL_FPoint position_ = {0, 0};
    SDL_FPoint direction_ = {0, 0};
    int width_ = 0;
    int height_ = 0;
    int speed_ = 300;
    int damage_ = 1;
};

// 爆炸纹理类
struct Explosion {
    SDL_Texture* texture_ = nullptr;
    SDL_FPoint position_ = {0, 0};
    int width_ = 0;
    int height_ = 0;
    int currentFrame_ = 0;
    int totalFrame_ = 0;
    
    Uint32 startTime_ = 0;
    Uint32 fps_ = 10;
};

// 道具类型枚举
enum class ItemType {
    HealthPack,
    ShieldPack,
    SkillCDPack
};

// 道具基类
struct Item {
    SDL_Texture* texture_ = nullptr;
    SDL_FPoint position_ = {0, 0};
    SDL_FPoint direction_ = {0, 0};
    int width_ = 0;
    int height_ = 0;
    int speed_ = 200;
    int bounceCount_ = 3;
    ItemType type_ = ItemType::HealthPack;
};

// 血包类
struct HealthPack : public Item {
    HealthPack() {
        type_ = ItemType::HealthPack;
    }
};

// 盾包类
struct ShieldPack : public Item {
    ShieldPack() {
        type_ = ItemType::ShieldPack;
    }
};

// 技能包类
struct SkillCDPack : public Item {
    SkillCDPack() {
        type_ = ItemType::SkillCDPack;
    }
};

// 技能类型枚举
enum class SkillType {
    ShieldReflect,
    Invincible,
    BulletSpeedUp,
    BulletBallisticUp
};

// 技能基类
struct Skill {
    SkillType type_;
    float coolDownTime_;
    float durationTime_;
    float currentCooldownTime_;
    float currentDurationTime_;
    bool isUsing_;
};

// 盾反技能
struct ShieldReflect : public Skill {
    float damageReflection_;
    bool reflectBullets_;
};

// 无敌技能
struct Invincible : public Skill {
    bool invincible_;
};

// 子弹速度提升技能
struct BulletSpeedUp : public Skill {
    float bulletSpeedUp_;
};

// 子弹弹道提升技能
struct BulletBallisticUp : public Skill {
    float bulletBallisticUp_;
};

// 技能管理器
struct SkillManager {
    std::list<Skill*> skills_;
    ShieldReflect* shieldReflect_;
    Invincible* invincible_;
    BulletSpeedUp* bulletSpeedUp_;
    BulletBallisticUp* bulletBallisticUp_;
};

// 背景图类
struct Background {
    SDL_Texture* texture_ = nullptr;
    SDL_FPoint position_ = {0, 0};
    float offset_ = 0.0f;
    int width_ = 0;
    int height_ = 0;
    int speed_ = 30;
};

#endif // OBJECT_H
