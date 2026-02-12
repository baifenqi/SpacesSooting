#include <string>
#include "SceneMain.h"
#include "SceneEnd.h"
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "object.h"

#include <random>

SceneMain::SceneMain(): game_(Game::getInstance())
{
    // 初始化成员变量
    isDeath_ = false;
    skillPulseTime_ = 0.0f;
    invincibleEffectTime_ = 0.0f;
    shieldEffectFrameTime_ = 0.0f;
    shieldEffectCurrentFrame_ = 0;
    shieldEffectTexture_ = nullptr;

    // 初始化玩家生命值
    player_.hp_ = 3;
    player_.maxHp_ = 7;
    player_.shield_ = 0;
    player_.maxShield_ = 5;
    
    // 初始化帧率监控器
    frameCount_ = 0;
    lastFpsUpdateTime_ = 0;
    currentFPS_ = 0.0f;

    changeSceneDelayed_ = false;
    deathTimer_ = 0.0f;
    timeEnd_ = 0.0f;
}

SceneMain::~SceneMain()
{
}

/******************主控制台*************/
void SceneMain::init()
{
    // 读取并播放背景音乐
    bgm_ = Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");
    if(bgm_ == nullptr){
        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
    }
    Mix_PlayMusic(bgm_, -1);

    // 读取音效资源
    soundEffectMap_["player_shoot"] = Mix_LoadWAV("assets/sound/laser_shoot4.wav");
    soundEffectMap_["enemy_shoot"] = Mix_LoadWAV("assets/sound/xs_laser.wav");
    soundEffectMap_["player_explode"] = Mix_LoadWAV("assets/sound/explosion1.wav");
    soundEffectMap_["enemy_explode"] = Mix_LoadWAV("assets/sound/explosion3.wav");
    soundEffectMap_["hit"] = Mix_LoadWAV("assets/sound/eff11.wav");
    // 补充缺失的player_hit音效，避免崩溃
    soundEffectMap_["player_hit"] = Mix_LoadWAV("assets/sound/eff11.wav");
    soundEffectMap_["get_item"] = Mix_LoadWAV("assets/sound/eff5.wav");

    //载入字体
    scoreFont_ = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 24);

    // 创建随机数生成器
    std::random_device rd;
    gen_ = std::mt19937(rd());
    dis_ = std::uniform_real_distribution<float>(0.0f, 1.0f);

    // 初始化难度设置
    gameTime_ = 0.0f;
    normalSpawnInterval_ = 1.0f;
    currentSpawnInterval_ = normalSpawnInterval_;
    difficultyMultiplier_ = 1.0f;
    rammingEnemyRatio_ = 0.3f;
    maxEnemies_ = 10;

    // 初始化得分
    score_ = 0;
    scoreColor_ = {255, 255, 255, 255};

    /****************初始化玩家************/
    // 飞船纹理
    player_.texture_ = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/SpaceShip.png");
    if(player_.texture_ == nullptr){ 
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(player_.texture_, nullptr, nullptr, &player_.width_, &player_.height_);

    // 调整玩家飞船大小
    player_.width_ /= 4;
    player_.height_ /= 4;

    // 调整玩家飞船位置
    player_.position_.x = static_cast<float>(game_.getWindowWidth()) / 2 - static_cast<float>(player_.width_) / 2;
    player_.position_.y = static_cast<float>(game_.getWindowHeight()) - static_cast<float>(player_.height_);


    /**************初始化玩家子弹模板*********/
    // 子弹纹理
    playerBulletTemplate_.texture_ = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/laser-1.png");
    if(playerBulletTemplate_.texture_ == nullptr){ 
        printf("Failed to load bullet texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(playerBulletTemplate_.texture_, nullptr, nullptr, &playerBulletTemplate_.width_, &playerBulletTemplate_.height_);

    // 调整子弹大小
    playerBulletTemplate_.width_ /= 4;
    playerBulletTemplate_.height_ /= 4;

    // 设置子弹初始速度
    playerBulletTemplate_.speed_ = 600;

    /****************初始化普通敌人飞船********/    
    // 敌人飞船纹理
    normalEnemyTemplate_.texture_ = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/insect-1.png");
    if(normalEnemyTemplate_.texture_ == nullptr){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(normalEnemyTemplate_.texture_, nullptr, nullptr, &normalEnemyTemplate_.width_, &normalEnemyTemplate_.height_);

    // 调整敌人飞船大小
    normalEnemyTemplate_.width_ /= 4;
    normalEnemyTemplate_.height_ /= 4; 

    /********初始化撞击敌人飞船********/
    // 敌人飞船纹理
    rammingEnemyTemplate_.texture_ = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/insect-2.png");
    if(rammingEnemyTemplate_.texture_ == nullptr){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(rammingEnemyTemplate_.texture_, nullptr, nullptr, &rammingEnemyTemplate_.width_, &rammingEnemyTemplate_.height_);

    // 调整敌人飞船大小
    rammingEnemyTemplate_.width_ /= 4;
    rammingEnemyTemplate_.height_ /= 4;
    
    /****************初始化敌人飞船子弹********/
    // 子弹纹理
    enemyBulletTemplate_.texture_ = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/bullet-1.png");
    if(enemyBulletTemplate_.texture_ == nullptr){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(enemyBulletTemplate_.texture_, nullptr, nullptr, &enemyBulletTemplate_.width_, &enemyBulletTemplate_.height_);

    // 调整敌人飞船子弹大小
    enemyBulletTemplate_.width_ /= 4;
    enemyBulletTemplate_.height_ /= 4;

    /****************初始化爆炸效果********/
    // 爆炸纹理
    explosionTemplate_.texture_ = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/effect/explosion.png");
    if(explosionTemplate_.texture_ == nullptr){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(explosionTemplate_.texture_, nullptr, nullptr, &explosionTemplate_.width_, &explosionTemplate_.height_);
    
    // 计算爆炸帧数
    explosionTemplate_.totalFrame_ = explosionTemplate_.width_ / explosionTemplate_.height_;
    // 调整爆炸效果大小     
    explosionTemplate_.width_ = explosionTemplate_.height_;

    /**********初始化游戏道具********/
    // 血包纹理
    itemHealthPackTemplate_.texture_ = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/bonus_life.png");
    if(itemHealthPackTemplate_.texture_ == nullptr){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(itemHealthPackTemplate_.texture_, nullptr, nullptr, &itemHealthPackTemplate_.width_, &itemHealthPackTemplate_.height_);
    
    // 调整血包大小
    itemHealthPackTemplate_.width_ /= 4;
    itemHealthPackTemplate_.height_ /= 4;

    // 盾包纹理
    itemShieldPackTemplate_.texture_ = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/bonus_shield.png");
    if(itemShieldPackTemplate_.texture_ == nullptr){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(itemShieldPackTemplate_.texture_, nullptr, nullptr, &itemShieldPackTemplate_.width_, &itemShieldPackTemplate_.height_);
    
    // 调整盾包大小
    itemShieldPackTemplate_.width_ /= 4;
    itemShieldPackTemplate_.height_ /= 4;

    // CD包纹理
    itemSkillCDPackTemplate_.texture_ = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/bonus_time.png");
    if(itemSkillCDPackTemplate_.texture_ == nullptr){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(itemSkillCDPackTemplate_.texture_, nullptr, nullptr, &itemSkillCDPackTemplate_.width_, &itemSkillCDPackTemplate_.height_);
    
    // 调整CD包大小
    itemSkillCDPackTemplate_.width_ /= 4;
    itemSkillCDPackTemplate_.height_ /= 4;
    
    /**********初始化玩家技能********/
    initSkill();

    // 加载盾反技能特效纹理
    shieldEffectTexture_ = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/effect/OrangeAura1.png");
    if(shieldEffectTexture_ == nullptr){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    
    // 初始化帧率监控器
    lastFpsUpdateTime_ = SDL_GetTicks();

    //加载状态UI图标
    heartTexture_ = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/Health UI Black.png");
    shieldTexture_ = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/Shield UI Black.png");

    //加载技能图标
    skillIconTexture_[0] = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/shieldReflect.png");
    skillIconTexture_[1] = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/invincible.png");
    skillIconTexture_[2] = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/ballisticUp.png");
    skillIconTexture_[3] = IMG_LoadTexture(Game::getInstance().getRenderer(), "assets/image/speedUp.png");
}

void SceneMain::clean()
{
    // 清理动态分配的游戏对象列表
    for(auto& playerBullet : playerBullets_){
        delete playerBullet; 
    }
    playerBullets_.clear();
    
    for(auto& enemy : enemies_){
        delete enemy; 
    }
    enemies_.clear();
    
    for(auto& enemyBullet : enemyBullets_){
        delete enemyBullet; 
    }
    enemyBullets_.clear();
    
    for(auto& explosion : explosions_){
        delete explosion; 
    }
    explosions_.clear();
    
    for(auto& item : items_){
        if(item != nullptr){
            delete item;
        }
    }
    items_.clear();

    // 清理技能对象
    for(auto& skill : skillManager_.skills_){
        if(skill != nullptr){
            delete skill;
        }
    }
    skillManager_.skills_.clear();

    // 清理音频资源
    if(bgm_ != nullptr){
        Mix_FreeMusic(bgm_);        
        bgm_ = nullptr;
    }

    for (auto& soundEffect : soundEffectMap_) {
        if(soundEffect.second != nullptr){
            Mix_FreeChunk(soundEffect.second); 
            soundEffect.second = nullptr;
        }            
    }
    soundEffectMap_.clear();

    // 清理字体资源
    if(scoreFont_ != nullptr){
        TTF_CloseFont(scoreFont_);
        scoreFont_ = nullptr;
    }

    // 清理纹理资源
    // 注意：不清理 scoreTexture_，因为它由 renderScore() 局部管理
    
    if(shieldEffectTexture_ != nullptr){
        SDL_DestroyTexture(shieldEffectTexture_);
        shieldEffectTexture_ = nullptr;
    }    

    if(heartTexture_ != nullptr){
        SDL_DestroyTexture(heartTexture_);
        heartTexture_ = nullptr;
    }

    if(shieldTexture_ != nullptr){
        SDL_DestroyTexture(shieldTexture_);
        shieldTexture_ = nullptr;
    }

    for(auto& skillIconTexture : skillIconTexture_){
        if(skillIconTexture != nullptr){
            SDL_DestroyTexture(skillIconTexture);
            skillIconTexture = nullptr;
        }
    };
}

void SceneMain::update(float deltaTime)
{
    // 核心修复：死亡后统一处理，停止所有逻辑，2秒后跳转
    if (isDeath_) {
        deathTimer_ += deltaTime;
        if (deathTimer_ >= 2.0f) {
            game_.changeScene(new SceneEnd());
        }
        return;
    }

    keyboardControl(deltaTime);    
    updatePlayerBullets(deltaTime);
    updatePlayer(deltaTime);
    spawnEnemy(deltaTime);
    updateEnemies(deltaTime);
    updateEnemyBullets(deltaTime);
    updateExplosions(deltaTime);
    updateItems(deltaTime);
    updateSkill(deltaTime);

    // 更新帧率
    frameCount_++;
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastFpsUpdateTime_ >= 1000) {
        currentFPS_ = frameCount_ * 1000.0f / (currentTime - lastFpsUpdateTime_);
        frameCount_ = 0;
        lastFpsUpdateTime_ = currentTime;

        // 输出帧率到控制台
        printf("FPS: %.2f\n", currentFPS_);
    }

    gameTime_ += deltaTime;

    difficultyMultiplier_ = 1.0f + (gameTime_ / 30.0f) * 0.1f; // 每过30秒，难度增加0.1倍
    currentSpawnInterval_ = normalSpawnInterval_ / difficultyMultiplier_;

    rammingEnemyRatio_ = 0.3f + (difficultyMultiplier_ - 1.0f) * 0.1f; // 每过30秒，撞击敌人比例增加0.1倍
    if (rammingEnemyRatio_ > 0.7f) {
        rammingEnemyRatio_ = 0.7f; // 限制撞击敌人比例的最大值为0.7
    }

    maxEnemies_ = static_cast<int>(10 + (difficultyMultiplier_ - 1.0f) * 5); // 每过30秒，最大敌人数量增加5个
    if (maxEnemies_ > 20) {
        maxEnemies_ = 20; // 限制最大敌人数量的最大值为20
    }
}

void SceneMain::render()
{
    // 渲染玩家子弹
    renderPlayerBullets();
    
    // 渲染玩家飞船
    if(!isDeath_){
        SDL_Rect playerRect = { 
            static_cast<int>(player_.position_.x),
            static_cast<int>(player_.position_.y),
            player_.width_,
            player_.height_
        };                            
        SDL_RenderCopy(game_.getRenderer(), player_.texture_, nullptr, &playerRect);    
    }
    
    // 渲染敌人飞船子弹
    renderEnemyBullets();
    
    // 渲染敌人飞船
    renderEnemies();

    // 渲染游戏道具
    renderItems();

    // 渲染爆炸效果
    renderExplosions();

    // 渲染技能
    renderSkill();
    
    // 渲染玩家状态图标
    renderPlayerStatus();

    //渲染得分UI
    renderScore();

    // 渲染技能图标
    renderSkillIcons();
}

void SceneMain::handleEvent(SDL_Event* event)
{
    (void)event; // 声明未使用    
}

/*********玩家飞船键盘控制*********************************************/
void SceneMain::keyboardControl(float deltaTime)
{
    if(isDeath_){
        return; // 如果玩家死亡，则不执行以下代码
    }

    auto keyboardState = SDL_GetKeyboardState(nullptr);  
    
    /******wsad控制玩家飞船上下左右移动*************************************/

    // 控制玩家飞船向上移动
    if (keyboardState[SDL_SCANCODE_W]){
        player_.position_.y -= deltaTime * player_.speed_;
    }
    
    // 控制玩家飞船向下移动
    if (keyboardState[SDL_SCANCODE_S]){
        player_.position_.y += deltaTime * player_.speed_;
    }
    
    // 控制玩家飞船向左移动
    if (keyboardState[SDL_SCANCODE_A]){
        player_.position_.x -= deltaTime * player_.speed_;
    }
    
    // 控制玩家飞船向右移动
    if (keyboardState[SDL_SCANCODE_D]){
        player_.position_.x += deltaTime * player_.speed_;
    }
    
    /*******限制玩家飞船移动范围*******************************************/

    // 限制玩家飞船向上移动范围
    if (player_.position_.y < 0){
        player_.position_.y = 0;
    }

    // 限制玩家飞船向下移动范围
    if (player_.position_.y > game_.getWindowHeight() - player_.height_){
        player_.position_.y = static_cast<float>(game_.getWindowHeight()) - static_cast<float>(player_.height_);
    }

    // 限制玩家飞船向左移动范围
    if (player_.position_.x < 0){
        player_.position_.x = 0;
    }

    // 限制玩家飞船向右移动范围
    if (player_.position_.x > game_.getWindowWidth() - player_.width_){
        player_.position_.x = static_cast<float>(game_.getWindowWidth()) - static_cast<float>(player_.width_);
    }

    /*******控制子弹发射***********************************************/
    float fireRateMultiplier = 1.0f;
    if(skillManager_.bulletSpeedUp_ && skillManager_.bulletSpeedUp_->isUsing_){
        fireRateMultiplier = skillManager_.bulletSpeedUp_->bulletSpeedUp_;
    }
    Uint32 adjustedCoolDown = static_cast<Uint32>(player_.coolDown_ / fireRateMultiplier);
    
    if(keyboardState[SDL_SCANCODE_SPACE]){
        auto currentTime = SDL_GetTicks();
        if (currentTime - player_.lastShotTime_ > adjustedCoolDown){
            createPlayerBullets();
            player_.lastShotTime_ = currentTime;
        }
    }   
    
    if (keyboardState[SDL_SCANCODE_J]){
        auto currentTime = SDL_GetTicks();
        if (currentTime - player_.lastShotTime_ > adjustedCoolDown){
            createPlayerBullets();
            player_.lastShotTime_ = currentTime;
        }
    }

    /*******控制技能释放***********************************************/
    if(keyboardState[SDL_SCANCODE_Q]){
        activateSkill(SkillType::ShieldReflect);
    }
    
    if(keyboardState[SDL_SCANCODE_E]){
        activateSkill(SkillType::Invincible);
    }
    
    if(keyboardState[SDL_SCANCODE_R]){
        activateSkill(SkillType::BulletBallisticUp);
    }

    if(keyboardState[SDL_SCANCODE_F]){
        activateSkill(SkillType::BulletSpeedUp);
    }
}

/*********玩家飞船状态更新*********************************************/
void SceneMain::updatePlayer(float deltaTime)
{
    // 移除死亡计时逻辑，避免重复
    if(collisionCooldown_ > 0){
        collisionCooldown_ -= deltaTime;
        if(collisionCooldown_ < 0){
            collisionCooldown_ = 0;
        }
        return; // 在冷却期间不进行碰撞检测
    }
    
    if(player_.hp_ <= 0){
        isDeath_ = true;
        player_.hp_ = 0;
        player_.shield_ = 0;
        auto currentTime = SDL_GetTicks();
        auto explosion = new Explosion();
        explosion->position_.x = player_.position_.x + player_.width_ / 2 - explosion->width_ / 2;
        explosion->position_.y = player_.position_.y + player_.height_ / 2 - explosion->height_ / 2;
        explosion->startTime_ = currentTime;
        explosions_.push_back(explosion);
        Mix_PlayChannel(-1, soundEffectMap_["player_explode"], 0);

        game_.setScore(score_);
        return;
    }
    
    for(auto enemy : enemies_){
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position_.x),
            static_cast<int>(enemy->position_.y),
            enemy->width_,
            enemy->height_
        };
        
        SDL_Rect playerRect = {
            static_cast<int>(player_.position_.x),
            static_cast<int>(player_.position_.y),
            player_.width_,
            player_.height_
        };
        
        if(SDL_HasIntersection(&enemyRect, &playerRect)){

            //设置碰撞冷却时间，防止玩家在短时间内多次碰撞
            collisionCooldown_ = 0.5f;
            
            //检查无敌技能
            if(skillManager_.invincible_ &&
                 skillManager_.invincible_->isUsing_ &&
                 skillManager_.invincible_->invincible_
                 ){
                enemy->hp_ = 0;
            }
            //检查盾反技能
            else if(skillManager_.shieldReflect_ && 
                skillManager_.shieldReflect_->isUsing_ &&
                skillManager_.shieldReflect_->reflectBullets_){
                    int damage = (enemy->type_ == EnemyType::Ramming) ?1 : 0;
                    accumulateedDamage_ += damage;
                    if(accumulateedDamage_ >= 1.0f){
                        player_.hp_ -= static_cast<int>(accumulateedDamage_);
                        accumulateedDamage_ -= static_cast<int>(accumulateedDamage_);
                    }
                    enemy->hp_ = 0;
                    hasShieldCollision_ = true;
                }   
            else if(player_.shield_ > 0){
                player_.shield_ -= 1;
                enemy->hp_ = 0;
            }
            else{
                int damage = (enemy->type_ == EnemyType::Ramming) ?1 : 1;
                player_.hp_ -= damage;
                enemy->hp_ = 0;                
            }
            Mix_PlayChannel(-1, soundEffectMap_["player_hit"], 0);
            break;
        }
    }
}

/*********创建玩家子弹*********************************************/
void SceneMain::createPlayerBullets()
{
    // 获取弹道up技能
    int extraRows = 0;
    if (skillManager_.bulletBallisticUp_ && skillManager_.bulletBallisticUp_->isUsing_){
        extraRows = static_cast<int>(skillManager_.bulletBallisticUp_->bulletBallisticUp_);        
    }
    
    // 创建玩家子弹
    PlayerBullet* playerBullet = new PlayerBullet();
    // 设置玩家子弹属性
    *playerBullet = playerBulletTemplate_;
    // 设置默认方向向量（向上）
    playerBullet->direction_.x = 0;
    playerBullet->direction_.y = -1;
    // 调整玩家子弹位置
    playerBullet->position_.x = player_.position_.x + player_.width_ / 2 - playerBullet->width_ / 2;
    playerBullet->position_.y = player_.position_.y;
    // 加入子弹容器
    playerBullets_.push_back(playerBullet);
    // 加入射击音效
    Mix_PlayChannel(0, soundEffectMap_["player_shoot"], 0);

    // 如果有弹道up技能，创建额外子弹
    if(extraRows > 0){
        float offsetX = playerBullet->width_ * 0.5f;        
        std::vector<SDL_FPoint> bulletPositions;
        bulletPositions.reserve(extraRows * 2 + 1);

        // 添加中心子弹位置
        bulletPositions.push_back({playerBullet->position_.x, playerBullet->position_.y});

        // 添加左侧子弹位置
        for(int i = 0; i <= extraRows; ++i){
            bulletPositions.push_back({playerBullet->position_.x - offsetX * (i + 1), playerBullet->position_.y});
        }
        
        // 添加右侧子弹位置
        for(int i = 0; i <= extraRows; ++i){
            bulletPositions.push_back({playerBullet->position_.x + offsetX * (i + 1), playerBullet->position_.y});
        }

        // 创建所有子弹
        for(const auto& pos : bulletPositions){
            PlayerBullet* extraBullet = new PlayerBullet();
            *extraBullet = playerBulletTemplate_;
            // 设置默认方向向量（向上）
            extraBullet->direction_.x = 0;
            extraBullet->direction_.y = -1;
            extraBullet->position_ = pos;
            playerBullets_.push_back(extraBullet);
        }
    }
}

void SceneMain::updatePlayerBullets(float deltaTime)
{   
    int margin = 32; // 子弹超出屏幕范围删除
    for (auto it = playerBullets_.begin(); it != playerBullets_.end();){

        auto playerBullet = *it;
        playerBullet->position_.x += deltaTime * playerBullet->speed_ * playerBullet->direction_.x;
        playerBullet->position_.y += deltaTime * playerBullet->speed_ * playerBullet->direction_.y;

        
        // 子弹超出屏幕范围删除
        if (playerBullet->position_.y + margin < 0){
            delete playerBullet;
            it = playerBullets_.erase(it);            
        } else {
            bool isHit = false;
            if(!enemies_.empty()){
                for(auto enemy : enemies_){

                    if(enemy->hp_ <= 0) continue;
                    
                    // 添加碰撞冷却检查
                    if(enemy->hitCooldown_ > 0){
                        continue;
                    }
                    
                    SDL_Rect enemyRect = {
                        static_cast<int>(enemy->position_.x),
                        static_cast<int>(enemy->position_.y),
                        enemy->width_,
                        enemy->height_
                    };
                    
                    SDL_Rect playerBulletRect = {
                        static_cast<int>(playerBullet->position_.x),
                        static_cast<int>(playerBullet->position_.y),
                        playerBullet->width_,
                        playerBullet->height_
                    };
                    
                    if(SDL_HasIntersection(&enemyRect, &playerBulletRect)){
                        enemy->hp_ -= playerBullet->damage_;
                        enemy->hitCooldown_ = static_cast<int>(0.1f);
                        delete playerBullet;
                        it = playerBullets_.erase(it);
                        isHit = true;
                        Mix_PlayChannel(-1, soundEffectMap_["hit"], 0);
                        break;
                    }                                        
                }
            }
            if(!isHit){
                ++it;
            }           
        }
    }
}

void SceneMain::renderPlayerBullets()
{
   for(auto playerBullet : playerBullets_){
        SDL_Rect playerBulletRect = { 
            static_cast<int>(playerBullet->position_.x),
            static_cast<int>(playerBullet->position_.y),
            playerBullet->width_,
            playerBullet->height_
        };                            
        SDL_RenderCopy(game_.getRenderer(), playerBullet->texture_, nullptr, &playerBulletRect);
   }
}

/*********创建敌人飞船*********************************************/
void SceneMain::spawnEnemy(float deltaTime)
{
    static float spawnTimer = 0.0f;
    spawnTimer += deltaTime;

    if(spawnTimer < currentSpawnInterval_) {
        return;
    }
    spawnTimer = 0.0f;

    if(enemies_.size() >= maxEnemies_){
        return; 
    }

    float spawnChance = dis_(gen_);
    
    // 根据动态比例生成敌人
    if(spawnChance < rammingEnemyRatio_) {
        RammingEnemy* enemy = new RammingEnemy(rammingEnemyTemplate_);
        enemy->position_.x = dis_(gen_) * (game_.getWindowWidth() - enemy->width_);
        enemy->position_.y = static_cast<float>(-enemy->height_);
        enemies_.push_back(enemy);
    }
    else {
        NormalEnemy* enemy = new NormalEnemy(normalEnemyTemplate_);
        enemy->position_.x = dis_(gen_) * (game_.getWindowWidth() - enemy->width_);
        enemy->position_.y = static_cast<float>(-enemy->height_);
        enemies_.push_back(enemy);
    }
}

void SceneMain::updateEnemies(float deltaTime)
{
    auto currentTime = SDL_GetTicks();
    for (auto it = enemies_.begin(); it != enemies_.end();){
        auto enemy = *it;

        enemy->update(deltaTime);

        if(enemy->hitCooldown_ > 0){
            enemy->hitCooldown_ -= static_cast<int>(deltaTime);
            if(enemy->hitCooldown_ < 0){
                enemy->hitCooldown_ = 0;
            }
        }
        
        if (enemy->position_.y > game_.getWindowHeight()){
            delete enemy;
            it = enemies_.erase(it);
        } else {
            if(enemy->type_ == EnemyType::Normal && 
               currentTime - enemy->lastShotTime_ > enemy->coolDown_ && 
               isDeath_ == false){
                createEnemyBullets(enemy);
                enemy->lastShotTime_ = currentTime;                
            }
            
            if(enemy->hp_ <= 0){
                enemyExplode(enemy);
                it = enemies_.erase(it);
            } else {
                ++it;
            }
        }
    }    
}

void SceneMain::renderEnemies()
{
    for(auto enemy : enemies_){
        SDL_Rect enemyRect = { 
            static_cast<int>(enemy->position_.x),
            static_cast<int>(enemy->position_.y),
            enemy->width_,
            enemy->height_
        };                            
        SDL_RenderCopy(game_.getRenderer(), enemy->texture_, nullptr, &enemyRect);
    }
}

/*********创建敌人子弹*********************************************/
void SceneMain::createEnemyBullets(Enemy* enemy)
{
    auto enemyBullet = new EnemyBullet();
    *enemyBullet = enemyBulletTemplate_;
    enemyBullet->position_.x = enemy->position_.x + enemy->width_ / 2 - enemyBullet->width_ / 2;
    enemyBullet->position_.y = enemy->position_.y + enemy->height_ / 2 - enemyBullet->height_ / 2;
    enemyBullet->direction_ = getEnemyBulletDirection(enemy);
    enemyBullets_.push_back(enemyBullet);
    Mix_PlayChannel(-1, soundEffectMap_["enemy_shoot"], 0);
}

SDL_FPoint SceneMain::getEnemyBulletDirection(Enemy* enemy)
{
    auto x = player_.position_.x + player_.width_ / 2 - enemy->position_.x - enemy->width_ / 2;
    auto y = player_.position_.y + player_.height_ / 2 - enemy->position_.y - enemy->height_ / 2;
    auto length = sqrt(x * x + y * y);
    x /= length;
    y /= length;
    return SDL_FPoint{x, y};
}

void SceneMain::updateEnemyBullets(float deltaTime)
{
    auto margin = 32;
    for (auto it = enemyBullets_.begin(); it != enemyBullets_.end();){
        auto enemyBullet = *it;
        enemyBullet->position_.x += deltaTime * enemyBullet->speed_ * enemyBullet->direction_.x;
        enemyBullet->position_.y += deltaTime * enemyBullet->speed_ * enemyBullet->direction_.y;
        
        if (enemyBullet->position_.y > game_.getWindowHeight() + margin ||
            enemyBullet->position_.y < -margin ||
            enemyBullet->position_.x > game_.getWindowHeight() + margin ||
            enemyBullet->position_.x < -margin){                
                delete enemyBullet;
                it = enemyBullets_.erase(it);
        } else {
            
            //如果玩家死亡，则不检测碰撞
            if(isDeath_){
                ++it;
                continue;                                                            
            }
            SDL_Rect enemyBulletRect = {
                static_cast<int>(enemyBullet->position_.x),
                static_cast<int>(enemyBullet->position_.y),
                enemyBullet->width_,
                enemyBullet->height_
            }; 
                            
            SDL_Rect playerRect = {
                static_cast<int>(player_.position_.x),
                static_cast<int>(player_.position_.y),
                player_.width_,
                player_.height_
            };
            
            if(SDL_HasIntersection(&enemyBulletRect, &playerRect) && isDeath_ == false){
                // 检查无敌技能
                if(skillManager_.invincible_ && 
                    skillManager_.invincible_->isUsing_ && 
                    skillManager_.invincible_->invincible_){
                        delete enemyBullet;
                        it = enemyBullets_.erase(it);                                      
                }
                // 检查盾反技能
                else if(skillManager_.shieldReflect_ && 
                    skillManager_.shieldReflect_->isUsing_ && 
                    skillManager_.shieldReflect_->reflectBullets_){
                        // 创建反弹子弹
                        auto playerBullet = new PlayerBullet();
                        playerBullet->texture_ = enemyBullet->texture_;
                        playerBullet->position_ = enemyBullet->position_;
                        playerBullet->width_ = enemyBullet->width_;
                        playerBullet->height_ = enemyBullet->height_;
                        playerBullet->speed_ = enemyBullet->speed_;
                        playerBullet->damage_ = enemyBullet->damage_;
                        
                        // 设置反弹方向：向上反弹，而不是简单反转
                        playerBullet->direction_.x = 0;
                        playerBullet->direction_.y = -1; // 向上
                        
                        playerBullets_.push_back(playerBullet);

                        delete enemyBullet;
                        it = enemyBullets_.erase(it);

                        // 盾反技能减少玩家受到的伤害
                        accumulateedDamage_ += enemyBullet->damage_ * skillManager_.shieldReflect_->damageReflection_;
                        if(accumulateedDamage_ >= 1.0f){
                            player_.hp_ -= static_cast<int>(accumulateedDamage_);
                            accumulateedDamage_ -= static_cast<int>(accumulateedDamage_);
                        }
                }
                else if(player_.shield_ > 0){
                    player_.shield_ -= enemyBullet->damage_;
                    delete enemyBullet;
                    it = enemyBullets_.erase(it);
                    Mix_PlayChannel(-1, soundEffectMap_["hit"], 0);  
                }                
                else{
                    player_.hp_ -= enemyBullet->damage_;
                    delete enemyBullet;
                    it = enemyBullets_.erase(it);
                    Mix_PlayChannel(-1, soundEffectMap_["hit"], 0);
                }                                              
            }
            else {
                ++it;
            }
        } 
    }
}

void SceneMain::renderEnemyBullets()
{
    for(auto enemyBullet : enemyBullets_){
        SDL_Rect enemyBulletRect = { 
            static_cast<int>(enemyBullet->position_.x),
            static_cast<int>(enemyBullet->position_.y),
            enemyBullet->width_,
            enemyBullet->height_
        };                            
        double angle = atan2f(enemyBullet->direction_.y, enemyBullet->direction_.x) * 180.0 / M_PI - 90.0f;
        SDL_RenderCopyEx(game_.getRenderer(), enemyBullet->texture_, nullptr, &enemyBulletRect, angle, nullptr, SDL_FLIP_NONE);
   }
}

/*********敌人爆炸*********************************************/
void SceneMain::enemyExplode(Enemy* enemy)
{
    auto currentTime = SDL_GetTicks();
    auto explosion = new Explosion(explosionTemplate_);
    explosion->position_.x = enemy->position_.x + enemy->width_ / 2 - explosion->width_ / 2;
    explosion->position_.y = enemy->position_.y + enemy->height_ / 2 - explosion->height_ / 2;
    explosion->startTime_ = currentTime;
    explosions_.push_back(explosion); 
    Mix_PlayChannel(-1, soundEffectMap_["enemy_explode"], 0);
    
    if(enemy ->type_ == EnemyType::Normal){
        score_ += 10;
    }
    else if(enemy ->type_ == EnemyType::Ramming){
        score_ += 30;
    }

    dropItem(enemy);  
    delete enemy;
}

void SceneMain::updateExplosions(float deltaTime)
{
    (void)deltaTime;//声明但不使用
    auto currentTime = SDL_GetTicks();
    for (auto it = explosions_.begin(); it != explosions_.end();){
        auto explosion = *it;
        explosion->currentFrame_ = (currentTime - explosion->startTime_) * explosion->fps_ / 1000; 
        if (explosion->currentFrame_ >= explosion->totalFrame_){
            delete explosion;
            it = explosions_.erase(it);
        } else {
            ++it;
        }
    }
}

void SceneMain::renderExplosions()
{
    for(auto explosion : explosions_){
        SDL_Rect src = {
            explosion->currentFrame_ * explosion->width_,
            0,
            explosion->width_,
            explosion->height_
        };
        SDL_Rect dst = {
            static_cast<int>(explosion->position_.x),
            static_cast<int>(explosion->position_.y),
            explosion->width_,
            explosion->height_
        };
        SDL_RenderCopy(game_.getRenderer(), explosion->texture_, &src, &dst);
    }
}

/*********道具掉落*********************************************/
void SceneMain::dropItem(Enemy* enemy)
{
    float dropChance = dis_(gen_); 
    Item* item;
    
    if(dropChance < 0.2f){
        // 20%概率血包
        item = new HealthPack();
        *item = itemHealthPackTemplate_;
        item->type_ = ItemType::HealthPack;
    }
    else if(dropChance < 0.35f){
        // 15%概率护盾
        item = new ShieldPack();
        *item = itemShieldPackTemplate_;
        item->type_ = ItemType::ShieldPack;
    }
    else if(dropChance < 0.5f){
        // 15%概率cd包
        item = new SkillCDPack();
        *item = itemSkillCDPackTemplate_;
        item->type_ = ItemType::SkillCDPack;
    }
    else{
        // 50%概率无道具
        return;
    }

    item->position_.x = enemy->position_.x + enemy->width_ / 2 - item->width_ / 2;
    item->position_.y = enemy->position_.y + enemy->height_ / 2 - item->height_ / 2;
    double angle = dis_(gen_) * 2 * M_PI;
    item->direction_.x = static_cast<float>(cos(angle));
    item->direction_.y = static_cast<float>(sin(angle));
    items_.push_back(item);
}

void SceneMain::playerGetItem(Item* item)
{
    // 血包获取
    if(item->type_ == ItemType::HealthPack){
        player_.hp_ += 1;        
        if(player_.hp_ > player_.maxHp_){
            player_.hp_ = player_.maxHp_;
        }
    }
    // 护盾获取
    else if(item->type_ == ItemType::ShieldPack){
        player_.shield_ += 1;
        if(player_.shield_ > player_.maxShield_){
            player_.shield_ = player_.maxShield_;
        }
    }
    // cd包减少cd时间
    else if(item->type_ == ItemType::SkillCDPack){
        skillCDPackEffect();
    }    
    Mix_PlayChannel(-1, soundEffectMap_["get_item"], 0);
}

void SceneMain::updateItems(float deltaTime)
{
    for(auto it = items_.begin(); it != items_.end();){
        auto item = *it;

        item->position_.x += deltaTime * item->speed_ * item->direction_.x;
        item->position_.y += deltaTime * item->speed_ * item->direction_.y;

        bool outOfScreen =  item->position_.x < 0 ||
                            item->position_.x > game_.getWindowWidth() - item->width_ ||
                            item->position_.y < 0 || 
                            item->position_.y > game_.getWindowHeight() - item->height_;
        bool needBounce = false;

        // 碰撞边缘三次后，超出屏幕删除
        if(item->position_.x < 0 && item->bounceCount_ > 0){
            item->direction_.x = -item->direction_.x; 
            item->bounceCount_--;
            needBounce = true;           
        }
        if(item->position_.x > game_.getWindowWidth() - item->width_ && item->bounceCount_ > 0){
            item->direction_.x = -item->direction_.x;
            item->bounceCount_--;
            needBounce = true;
        }
        if(item->position_.y < 0 && item->bounceCount_ > 0){
            item->direction_.y = -item->direction_.y;
            item->bounceCount_--;
            needBounce = true;
        }
        if(item->position_.y > game_.getWindowHeight() - item->height_ && item->bounceCount_ > 0){
            item->direction_.y = -item->direction_.y;
            item->bounceCount_--;
            needBounce = true;
        }

        if(outOfScreen && !needBounce){
            delete item;
            it = items_.erase(it);
        }
        // 碰撞检测,拾取道具
        else {
            SDL_Rect itemRect = {
                static_cast<int>(item->position_.x),
                static_cast<int>(item->position_.y),
                item->width_,
                item->height_        
            };
            SDL_Rect playerRect = {
                static_cast<int>(player_.position_.x),
                static_cast<int>(player_.position_.y),
                player_.width_,
                player_.height_
            };
            
            if(SDL_HasIntersection(&itemRect, &playerRect) && isDeath_ == false){
                playerGetItem(item);
                delete item;
                it = items_.erase(it);
            }
            else {
                ++it;
            }
        }
    };
}

void SceneMain::renderItems()
{ 
    for(auto item : items_){
        SDL_Rect itemRect = {
            static_cast<int>(item->position_.x),
            static_cast<int>(item->position_.y),
            item->width_,
            item->height_
        };
        SDL_RenderCopy(game_.getRenderer(), item->texture_, nullptr, &itemRect);
    }
}

void SceneMain::skillCDPackEffect()
{
    for(auto skill : skillManager_.skills_){
        if (skill->currentCooldownTime_ > 0.0f){
            skill->currentCooldownTime_ *= 0.7f;
            if(skill->currentCooldownTime_ < 0.0f){
                skill->currentCooldownTime_ = 0.0f;
            }
        }
    }
}

void SceneMain::initSkill()
{
    // 创建盾反技能
    skillManager_.shieldReflect_ = new ShieldReflect();
    skillManager_.shieldReflect_->type_ = SkillType::ShieldReflect;
    skillManager_.shieldReflect_->coolDownTime_ = 7.0f;
    skillManager_.shieldReflect_->durationTime_ = 4.0f;
    skillManager_.shieldReflect_->currentCooldownTime_ = 0.0f;
    skillManager_.shieldReflect_->currentDurationTime_ = 0.0f;
    skillManager_.shieldReflect_->isUsing_ = false;
    skillManager_.shieldReflect_->damageReflection_ = 0.5f;
    skillManager_.shieldReflect_->reflectBullets_ = false;
    skillManager_.skills_.push_back(skillManager_.shieldReflect_);

    // 创建无敌技能
    skillManager_.invincible_ = new Invincible();
    skillManager_.invincible_->type_ = SkillType::Invincible;
    skillManager_.invincible_->coolDownTime_ = 10.0f;
    skillManager_.invincible_->durationTime_ = 3.0f;
    skillManager_.invincible_->currentCooldownTime_ = 0.0f;
    skillManager_.invincible_->currentDurationTime_ = 0.0f;
    skillManager_.invincible_->isUsing_ = false;
    skillManager_.invincible_->invincible_ = false;
    skillManager_.skills_.push_back(skillManager_.invincible_);

    // 创建射速up技能
    skillManager_.bulletSpeedUp_ = new BulletSpeedUp();
    skillManager_.bulletSpeedUp_->type_ = SkillType::BulletSpeedUp;
    skillManager_.bulletSpeedUp_->coolDownTime_ = 10.0f;
    skillManager_.bulletSpeedUp_->durationTime_ = 20.0f;
    skillManager_.bulletSpeedUp_->currentCooldownTime_ = 0.0f;
    skillManager_.bulletSpeedUp_->currentDurationTime_ = 0.0f;
    skillManager_.bulletSpeedUp_->isUsing_ = false;
    skillManager_.bulletSpeedUp_->bulletSpeedUp_ = 2.0f;
    skillManager_.skills_.push_back(skillManager_.bulletSpeedUp_);

    // 创建弹道up技能
    skillManager_.bulletBallisticUp_ = new BulletBallisticUp();
    skillManager_.bulletBallisticUp_->type_ = SkillType::BulletBallisticUp;
    skillManager_.bulletBallisticUp_->coolDownTime_ = 10.0f;
    skillManager_.bulletBallisticUp_->durationTime_ = 20.0f;
    skillManager_.bulletBallisticUp_->currentCooldownTime_ = 0.0f;
    skillManager_.bulletBallisticUp_->currentDurationTime_ = 0.0f;
    skillManager_.bulletBallisticUp_->isUsing_ = false;
    skillManager_.bulletBallisticUp_->bulletBallisticUp_ = 1;
    skillManager_.skills_.push_back(skillManager_.bulletBallisticUp_);
}

void SceneMain::activateSkill(SkillType skillType)
{
    for (auto skill : skillManager_.skills_){
        if(skill->type_ == skillType && skill->currentCooldownTime_ <= 0 && !skill->isUsing_){
            skill->isUsing_ = true;
            skill->currentCooldownTime_ = skill->coolDownTime_;
            skill->currentDurationTime_ = skill->durationTime_;  
            
            // 根据技能类型激活技能
            switch (skillType)
            {
            case SkillType::ShieldReflect:
                skillManager_.shieldReflect_->reflectBullets_ = true;
                break;
            case SkillType::Invincible:
                skillManager_.invincible_->invincible_ = true;
                break;
            case SkillType::BulletSpeedUp:
                skillManager_.bulletSpeedUp_->bulletSpeedUp_ = 2.0f;
                break;
            case SkillType::BulletBallisticUp:
                skillManager_.bulletBallisticUp_->bulletBallisticUp_++;
                break;
            }            
        }
    }
}

void SceneMain::updateSkill(float deltaTime)
{
    // 更新盾反技能
    if(skillManager_.shieldReflect_->currentCooldownTime_ > 0){
        skillManager_.shieldReflect_->currentCooldownTime_ -= deltaTime;
        if(skillManager_.shieldReflect_->currentCooldownTime_ < 0){    
            skillManager_.shieldReflect_->currentCooldownTime_ = 0;
        }
    }

    if(skillManager_.shieldReflect_->currentDurationTime_ > 0){
        skillManager_.shieldReflect_->currentDurationTime_ -= deltaTime;
        if(skillManager_.shieldReflect_->currentDurationTime_ < 0){
            skillManager_.shieldReflect_->isUsing_ = false;
            skillManager_.shieldReflect_->currentDurationTime_ = 0;            
            skillManager_.shieldReflect_->reflectBullets_ = false;
        }
    }

    // 更新盾反技能帧动画
    if(skillManager_.shieldReflect_->isUsing_) {
        shieldEffectFrameTime_ += deltaTime;
        if(shieldEffectFrameTime_ >= 0.1f) { // 每0.1秒切换一帧
            shieldEffectFrameTime_ -= 0.1f;
            shieldEffectCurrentFrame_ = (shieldEffectCurrentFrame_ + 1) % 4; // 假设有4帧
        }
    } else {
        shieldEffectCurrentFrame_ = 0; // 技能未激活时重置为第一帧
    }

    // 更新无敌技能特效时间
    if(skillManager_.invincible_->isUsing_) {
        invincibleEffectTime_ += deltaTime;
    }

    // 更新无敌技能
    if(skillManager_.invincible_->currentCooldownTime_ > 0){
        skillManager_.invincible_->currentCooldownTime_ -= deltaTime;
        if(skillManager_.invincible_->currentCooldownTime_ < 0){
            skillManager_.invincible_->currentCooldownTime_ = 0;
        }
    }

    if(skillManager_.invincible_->currentDurationTime_ > 0){
        skillManager_.invincible_->currentDurationTime_ -= deltaTime;
        if(skillManager_.invincible_->currentDurationTime_ < 0){
            skillManager_.invincible_->isUsing_ = false;
            skillManager_.invincible_->currentDurationTime_ = 0;
            skillManager_.invincible_->invincible_ = false;
        }
    }

    // 更新射速up技能
    if(skillManager_.bulletSpeedUp_->currentCooldownTime_ > 0){
        skillManager_.bulletSpeedUp_->currentCooldownTime_ -= deltaTime;
        if(skillManager_.bulletSpeedUp_->currentCooldownTime_ < 0){
            skillManager_.bulletSpeedUp_->currentCooldownTime_ = 0;
        }
    }

    if(skillManager_.bulletSpeedUp_->currentDurationTime_ > 0){
        skillManager_.bulletSpeedUp_->currentDurationTime_ -= deltaTime;
        if(skillManager_.bulletSpeedUp_->currentDurationTime_ < 0){
            skillManager_.bulletSpeedUp_->isUsing_ = false;
            skillManager_.bulletSpeedUp_->currentDurationTime_ = 0;
        }
    }

    // 更新弹道up技能
    if(skillManager_.bulletBallisticUp_->currentCooldownTime_ > 0){
        skillManager_.bulletBallisticUp_->currentCooldownTime_ -= deltaTime;
        if(skillManager_.bulletBallisticUp_->currentCooldownTime_ < 0){
            skillManager_.bulletBallisticUp_->currentCooldownTime_ = 0;
        }
    }

    if(skillManager_.bulletBallisticUp_->currentDurationTime_ > 0){
        skillManager_.bulletBallisticUp_->currentDurationTime_ -= deltaTime;
        if(skillManager_.bulletBallisticUp_->currentDurationTime_ < 0){
            skillManager_.bulletBallisticUp_->isUsing_ = false;
            skillManager_.bulletBallisticUp_->currentDurationTime_ = 0;
        }
    }

    skillPulseTime_ += deltaTime;
    if(skillPulseTime_ > 2.0f * M_PI){
        skillPulseTime_ -= static_cast<float>(2.0f * M_PI);        
    }
}

void SceneMain::renderSkill()
{
    // 只在技能激活时渲染效果
    if(!skillManager_.shieldReflect_->isUsing_ && !skillManager_.invincible_->isUsing_){
        return;       
    }
    
    // 保持当前混合模式
    SDL_BlendMode currentBlendMode;
    Uint8 currentR, currentG, currentB, currentA;
    SDL_GetRenderDrawBlendMode(game_.getRenderer(), &currentBlendMode);
    SDL_GetRenderDrawColor(game_.getRenderer(), &currentR, &currentG, &currentB, &currentA);

    // 启用混合模式以支持透明度
    SDL_SetRenderDrawBlendMode(game_.getRenderer(), SDL_BLENDMODE_BLEND);

    // 计算脉冲因子（0.8 ~ 1.2）
    float pulseFactor = 1.0f + 0.2f * static_cast<float>(sin(skillPulseTime_ * 3.0f));

    // 渲染盾反技能效果
    if(skillManager_.shieldReflect_->reflectBullets_ && 
        skillManager_.shieldReflect_->isUsing_ && 
        skillManager_.shieldReflect_->reflectBullets_){
            // 获取纹理尺寸
            int effectWidth, effectHeight;
            SDL_QueryTexture(shieldEffectTexture_, nullptr, nullptr, &effectWidth, &effectHeight);

            // 计算单帧宽度和高度
            int frameWidth = effectWidth / 4;
            int frameHeight = effectHeight;

            // 计算特效位置和大小
            int centerX = static_cast<int>(player_.position_.x + player_.width_ / 2);
            int centerY = static_cast<int>(player_.position_.y + player_.height_ / 2);
            float scaleMultiplier = 5.0f;
            int scaleWidth = static_cast<int>(frameWidth * pulseFactor * scaleMultiplier);
            int scaleHeight = static_cast<int>(frameHeight * pulseFactor * scaleMultiplier);

            // 计算纹理(当前帧)
            SDL_Rect srcRect = {
                shieldEffectCurrentFrame_ * frameWidth,
                0,
                frameWidth,
                frameHeight
            };

            // 计算目标矩形(渲染位置和大小)
            SDL_Rect dstRect = {
                centerX - scaleWidth / 2,
                centerY - scaleHeight / 2,
                scaleWidth,
                scaleHeight
            };

            // 渲染特效纹理
            SDL_RenderCopy(game_.getRenderer(), shieldEffectTexture_, &srcRect, &dstRect); 
            
            //碰撞反馈效果
            if(hasShieldCollision_){
                // 渲染碰撞反馈效果               
                SDL_Rect shieldRect = {
                    centerX - scaleWidth / 2,
                    centerY - scaleHeight / 2,
                    scaleWidth,
                    scaleHeight
                };
                SDL_SetRenderDrawColor(game_.getRenderer(), 255, 255, 255, 128);
                SDL_SetRenderDrawBlendMode(game_.getRenderer(), SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(game_.getRenderer(), &shieldRect);
                SDL_SetRenderDrawBlendMode(game_.getRenderer(), currentBlendMode);
                hasShieldCollision_ = false;
            }
            
    }

    // 渲染无敌技能效果
    if(skillManager_.invincible_->invincible_ && 
        skillManager_.invincible_->isUsing_ && 
        skillManager_.invincible_->invincible_){
            
            // 计算玩家中心位置
            int centerX = static_cast<int>(player_.position_.x + player_.width_ / 2);
            int centerY = static_cast<int>(player_.position_.y + player_.height_ / 2);
            
            // 使用不同的变量名避免声明隐藏
            float invinciblePulseFactor = 1.0f + 0.05f * static_cast<float>(sin(skillPulseTime_ * 3.0f));
            
            // 绘制外围光环（更亮的蓝紫色）
            int outerRadius = static_cast<int>((player_.width_ / 2) * 1.2f * invinciblePulseFactor);
            int innerRadius = static_cast<int>((player_.width_ / 2) * 0.9f * invinciblePulseFactor);
            
            // 使用多个同心圆创建渐变效果
            for(int radius = innerRadius; radius <= outerRadius; radius += 1) {
                // 计算透明度（外层更透明）
                float alphaRatio = 1.0f - static_cast<float>(radius - innerRadius) / (outerRadius - innerRadius);
                // 添加范围检查，确保alpha值在0-255范围内
                int alphaValue = static_cast<int>(200 * alphaRatio);
                Uint8 alpha = static_cast<Uint8>(std::min(255, std::max(0, alphaValue)));
                
                // 设置颜色（调整为更亮的蓝紫色）
                SDL_SetRenderDrawColor(game_.getRenderer(), 100, 149, 237, alpha);
                
                // 绘制圆形光晕
                drawCircle(game_.getRenderer(), centerX, centerY, radius);
            }
            
            // 绘制内部发光效果（更亮的青色圆形）
            for(int i = 0; i < 3; i++) {
                int glowRadius = static_cast<int>((player_.width_ / 2) * (0.8f - i * 0.1f) * invinciblePulseFactor);
                // 添加范围检查，确保alpha值在0-255范围内
                int alphaValue = 160 - i * 40;
                Uint8 alpha = static_cast<Uint8>(std::min(255, std::max(0, alphaValue)));
                
                SDL_SetRenderDrawColor(game_.getRenderer(), 135, 206, 250, alpha);
                drawCircle(game_.getRenderer(), centerX, centerY, glowRadius);
            }

            // 添加额外的光点效果，增强亮度感
            int numPoints = 8;
            for(int i = 0; i < numPoints; i++) {
                // 添加显式类型转换
                float angle = static_cast<float>((2.0f * M_PI / numPoints) * i + skillPulseTime_ * 2.0f);
                int pointRadius = static_cast<int>((player_.width_ / 2) * 1.1f * invinciblePulseFactor);
                int pointX = centerX + static_cast<int>(cos(angle) * pointRadius);
                int pointY = centerY + static_cast<int>(sin(angle) * pointRadius);
                
                // 绘制光点
                for(int r = 0; r < 3; r++) {
                    // 添加范围检查，确保alpha值在0-255范围内
                    int alphaValue = 220 - r * 50;
                    Uint8 alpha = static_cast<Uint8>(std::min(255, std::max(0, alphaValue)));
                    SDL_SetRenderDrawColor(game_.getRenderer(), 200, 230, 255, alpha);
                    drawCircle(game_.getRenderer(), pointX, pointY, 3 - r);
                }
            }
    }

    // 恢复之前的混合模式
    SDL_SetRenderDrawBlendMode(game_.getRenderer(), currentBlendMode);
    SDL_SetRenderDrawColor(game_.getRenderer(), currentR, currentG, currentB, currentA);
}

void SceneMain::renderPlayerStuas()
{
}

void SceneMain::renderScore()
{
    std::string scoreText = "Score: " + std::to_string(score_);

    SDL_Surface* scoreSurface = TTF_RenderText_Solid(scoreFont_, scoreText.c_str(), scoreColor_);
    if(scoreSurface == nullptr){
        SDL_Log("Failed to render text: %s", TTF_GetError());
        return;
    }
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(game_.getRenderer(), scoreSurface);
    if(scoreTexture == nullptr){
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        SDL_FreeSurface(scoreSurface);
        return;
    }

    int textWidth = scoreSurface->w;
    int textHeight = scoreSurface->h;
    int x = (game_.getWindowWidth() - textWidth) / 2;
    int y = 10;

    SDL_Rect textRect = {x, y, textWidth, textHeight};

    SDL_RenderCopy(game_.getRenderer(), scoreTexture, NULL, &textRect);

    SDL_DestroyTexture(scoreTexture);
    SDL_FreeSurface(scoreSurface);
}

void SceneMain::spawnNormalEnemy()
{
}

void SceneMain::spawnRammingEnemy()
{
}

void SceneMain::drawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius)
{
    // 使用Bresenham算法绘制圆形
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);

        x++;

        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else {
            d = d + 4 * x + 6;
        }       
    }
}

void SceneMain::renderPlayerStatus()
{
    // 确保生命值不为负
    int displayHp = std::max(0, player_.hp_);
    //渲染生命值
    for(int i = 0; i < displayHp; i++)
    {
        SDL_Rect destRect = {
            STAUS_X + i * STAUS_SPACING,
            STAUS_Y,
            32,
            32 
        };
        SDL_RenderCopy(game_.getRenderer(), heartTexture_, NULL, &destRect);
    }
    //确保护盾值不为负
    int displayShield = std::max(0, player_.shield_);

    //渲染护盾值
    for(int i = 0; i < displayShield; i++){
        SDL_Rect destRect = {
            STAUS_X + i * STAUS_SPACING,
            STAUS_Y + 32,
            32,
            32 
        };
        SDL_RenderCopy(game_.getRenderer(), shieldTexture_, NULL, &destRect);
    }

    // 如果玩家死亡，显示游戏结束提示
    if(isDeath_){
        // 渲染半透明黑色背景
        SDL_Rect overlayRect = {
            0,
            0,
            game_.getWindowWidth(),
            game_.getWindowHeight()
        };
        SDL_SetRenderDrawBlendMode(game_.getRenderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(game_.getRenderer(), 0, 0, 0, 180);
        SDL_RenderFillRect(game_.getRenderer(), &overlayRect);
        SDL_SetRenderDrawBlendMode(game_.getRenderer(), SDL_BLENDMODE_NONE);
        // 这里可以添加游戏结束的UI渲染代码
        // 例如：显示"Game Over"文本或图像
    }
}

void SceneMain::renderSkillIcons()
{
    int skillX = game_.getWindowWidth() - SKILL_X_OFFSET - 64;
    int skillY = SKILL_Y_START;
    int index = 0;

    for(auto it = skillManager_.skills_.begin(); it != skillManager_.skills_.end(); ++it)
    {
        renderSkillIcon(*it, skillX, skillY, index);
        skillY += SKILL_SPACING;
        index++;
    }
}

void SceneMain::renderSkillIcon(Skill *skill, int x, int y, int index)
{
    SDL_BlendMode currentBlendMode;
    Uint8 currentR, currentG, currentB, currentA;
    SDL_GetRenderDrawBlendMode(game_.getRenderer(), &currentBlendMode);
    SDL_GetRenderDrawColor(game_.getRenderer(), &currentR, &currentG, &currentB, &currentA);

    try{
    // 渲染技能图标
    SDL_Rect destRect = {
        x,
        y,
        64,
        64
    };
    SDL_RenderCopy(game_.getRenderer(), skillIconTexture_[index], NULL, &destRect);

    // 渲染技能冷却时间
    if(skill->currentCooldownTime_ > 0)
    {
        float cooldownRatio = skill->currentCooldownTime_ / skill->coolDownTime_;
        int maskHeight = static_cast<int>(64 * cooldownRatio);

        SDL_Rect maskRect = {
            x,
            y + 64 - maskHeight,
            64,
            maskHeight
        };
        SDL_SetRenderDrawColor(game_.getRenderer(), 0, 0, 0, 180);
        SDL_SetRenderDrawBlendMode(game_.getRenderer(), SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(game_.getRenderer(), &maskRect);
        SDL_SetRenderDrawBlendMode(game_.getRenderer(), SDL_BLENDMODE_NONE);
    }

    // 渲染技能激活状态
    if(skill->isUsing_){
        SDL_Rect activeRect = {
            x - 2,
            y - 2,
            68,
            68
        };
        SDL_SetRenderDrawColor(game_.getRenderer(), 255, 255, 0, 255);
        SDL_RenderDrawRect(game_.getRenderer(), &activeRect);        
    }
    }catch(...){
        // 确保即使发生异常也能恢复渲染器状态
    }

    SDL_SetRenderDrawBlendMode(game_.getRenderer(), currentBlendMode);
    SDL_SetRenderDrawColor(game_.getRenderer(), currentR, currentG, currentB, currentA);
}

void SceneMain::changeSceneDelayed(float deltaTime, float delay)
{
    timeEnd_ += deltaTime;
    if(timeEnd_ > delay){
        game_.changeScene(new SceneEnd());
    }
}