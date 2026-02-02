#include "SceneMain.h"
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "object.h"

#include <random>

SceneMain::SceneMain():game(Game::getInstance())
{
}

SceneMain::~SceneMain()
{
}

/******************主控制台*************/
void SceneMain::init()
{    
    /****************初始化玩家************/
    //飞船图层
    player.texture = IMG_LoadTexture(Game::getInstance().getRenderer(),"assets/image/SpaceShip.png");
    if(player.texture == NULL){ 
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.height);

    //调整玩家飞船大小
    player.width /= 4;
    player.height /= 4;

    //调整玩家飞船位置
    player.position.x =static_cast<float>(game.getWindowWidth())/2- static_cast<float>(player.width)/2;
    player.position.y =static_cast<float>(game.getWindowHeight()) - static_cast<float>(player.height);


    /**************初始化玩家子弹模板*********/
    //子弹图层
    playerBulletTemplate.texture = IMG_LoadTexture(Game::getInstance().getRenderer(),"assets/image/laser-1.png");
    if(playerBulletTemplate.texture == NULL){ 
        printf("Failed to load bullet texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(playerBulletTemplate.texture, NULL, NULL, &playerBulletTemplate.width, &playerBulletTemplate.height);

    //调整子弹大小
    playerBulletTemplate.width /= 4;
    playerBulletTemplate.height /= 4;

    //显示设置子弹初始位置
    playerBulletTemplate.speed = 600;

    /****************初始化敌人飞船********/    
    //敌人飞船图层
    enemyTemplate.texture = IMG_LoadTexture(Game::getInstance().getRenderer(),"assets/image/insect-1.png");
    if(enemyTemplate.texture == NULL){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(enemyTemplate.texture, NULL, NULL, &enemyTemplate.width, &enemyTemplate.height);

    //调整敌人飞船大小
    enemyTemplate.width /= 4;
    enemyTemplate.height /= 4; 
    
    //调整敌人飞船位置
        //创建随机数——敌人飞船出现位置
        std::random_device rd;
        gen = std::mt19937(rd());
        dis = std::uniform_real_distribution<float>(0.0f, 1.0f);

    /****************初始化敌人飞船子弹********/
    //子弹图层
    enemyBulletTemplate.texture = IMG_LoadTexture(Game::getInstance().getRenderer(),"assets/image/bullet-1.png");
    if(enemyBulletTemplate.texture == NULL){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(enemyBulletTemplate.texture, NULL, NULL, &enemyBulletTemplate.width, &enemyBulletTemplate.height);

    //调整敌人飞船子弹大小
    enemyBulletTemplate.width /= 4;
    enemyBulletTemplate.height /= 4;

    /****************初始化爆炸效果********/
    //爆炸图层
    explosionTemplate.texture = IMG_LoadTexture(Game::getInstance().getRenderer(),"assets/effect/explosion.png");
    if(explosionTemplate.texture == NULL){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(explosionTemplate.texture, NULL, NULL, &explosionTemplate.width, &explosionTemplate.height);
    //计算爆炸帧数
    explosionTemplate.totalFrame =explosionTemplate.width / explosionTemplate.height;
    //调整爆炸效果大小     
    explosionTemplate.width = explosionTemplate.height;

    /**********初始化游戏道具********/
    //血包图层
    itemHealthPackTemplate.texture = IMG_LoadTexture(Game::getInstance().getRenderer(),"assets/image/bonus_life.png");
    if(itemHealthPackTemplate.texture == NULL){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(itemHealthPackTemplate.texture, NULL, NULL, &itemHealthPackTemplate.width, &itemHealthPackTemplate.height);
    //调整血包大小
    itemHealthPackTemplate.width /= 4;
    itemHealthPackTemplate.height /= 4;

    //盾包图层
    itemShieldPackTemplate.texture = IMG_LoadTexture(Game::getInstance().getRenderer(),"assets/image/bonus_shield.png");
    if(itemShieldPackTemplate.texture == NULL){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(itemShieldPackTemplate.texture, NULL, NULL, &itemShieldPackTemplate.width, &itemShieldPackTemplate.height);
    //调整盾包大小
    itemShieldPackTemplate.width /= 4;
    itemShieldPackTemplate.height /= 4;

    //cd包图层
    itemSkillCDPackTemplate.texture = IMG_LoadTexture(Game::getInstance().getRenderer(),"assets/image/bonus_time.png");
    if(itemSkillCDPackTemplate.texture == NULL){
        printf("Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
    }
    SDL_QueryTexture(itemSkillCDPackTemplate.texture, NULL, NULL, &itemSkillCDPackTemplate.width, &itemSkillCDPackTemplate.height);
    //调整cd包大小
    itemSkillCDPackTemplate.width /= 4;
    itemSkillCDPackTemplate.height /= 4;
    
    /**********初始化玩家技能********/
    initSkill();

}

void SceneMain::update(float deltaTime)
{
    keyboardControl(deltaTime);
    updateplayerbullets(deltaTime);
    updatePlayer();
    spawnEnemy();
    updateEnemies(deltaTime);
    updateEnemyBullets(deltaTime);
    updateExplosions(deltaTime);
    updateItems(deltaTime);
    updateSkill(deltaTime);
}

void SceneMain::render()
{
    //渲染玩家子弹
    renderPlayerBullets();
    //渲染玩家飞船
    if(!isDeath){
        SDL_Rect PlayerRect = { static_cast<int>(player.position.x),
                            static_cast<int>(player.position.y),
                            player.width,
                            player.height};                            
    SDL_RenderCopy(game.getRenderer(), player.texture, NULL,&PlayerRect);    
    }
    
    //渲染敌人飞船子弹
    renderEnemyBullets();
    //渲染敌人飞船
    renderEnemies();

    //渲染游戏道具
    renderItems();

    //渲染爆炸效果
    renderExplosions();

    //渲染技能
    renderSkill();    
}

void SceneMain::handleEvent(SDL_Event *event)
{
    (void)event;//声明未使用    
}


void SceneMain::clean()
{
    /******清理玩家飞船*****/
    if(player.texture != NULL){
        SDL_DestroyTexture(player.texture);  
    }
    /*****清理玩家子弹*****/
    //清理子弹容器
    for(auto &playerbullet : playerBullets){
        delete playerbullet; // 只需要 delete 对象
    }
    playerBullets.clear();
    
    //清理子弹模板
    if(playerBulletTemplate.texture != NULL){
        SDL_DestroyTexture(playerBulletTemplate.texture);
    }

    /******清理敌人飞船*****/
    //清理敌人飞船容器
    for(auto &enemy : enemies){
        delete enemy; // 只需要 delete 对象 
    }
    enemies.clear();

    //清理敌人飞船模板
    if(enemyTemplate.texture != NULL){
        SDL_DestroyTexture(enemyTemplate.texture);
    }
    /******清理敌人飞船子弹*****/
    //清理敌人飞船子弹容器
    for(auto &enemyBullet : enemyBullets){
        delete enemyBullet; // 只需要 delete 对象
    }
    enemyBullets.clear();
    //清理敌人飞船子弹模板
    if(enemyBulletTemplate.texture != NULL){
        SDL_DestroyTexture(enemyBulletTemplate.texture);
    }

    /******清理爆炸效果*****/
    //清理爆炸效果容器
    for(auto &explosion : explosions){
        delete explosion; 
    }
    explosions.clear();

    //清理爆炸效果模板
    if(explosionTemplate.texture != NULL){
        SDL_DestroyTexture(explosionTemplate.texture);
    }

    /******清理游戏道具*****/
    //清理游戏道具容器
    for(auto &item : items){
        if(item != NULL){
        delete item;
        }
    }
    items.clear();
    //清理游戏道具模板
    if(itemHealthPackTemplate.texture != NULL){
        SDL_DestroyTexture(itemHealthPackTemplate.texture);
    }
    if(itemShieldPackTemplate.texture != NULL){
        SDL_DestroyTexture(itemShieldPackTemplate.texture);
    }
    if(itemSkillCDPackTemplate.texture != NULL){
        SDL_DestroyTexture(itemSkillCDPackTemplate.texture);
    }
    

    //清理技能
    for(auto &skill : skillManager.skills){
        if(skill != NULL){
            delete skill;
        }
    }
    skillManager.skills.clear();
}

/*********玩家飞船键盘控制*********************************************/
void SceneMain::keyboardControl(float deltaTime)
{
    if(isDeath){
        return;//如果玩家死亡，则不执行以下代码
    }

    auto keyboardState = SDL_GetKeyboardState(NULL);  
/******wsad控制玩家飞船上下左右移动*************************************/

    //控制玩家飞船向上移动
    if (keyboardState[SDL_SCANCODE_W]){
        player.position.y -= deltaTime *player.speed;
    }
    //控制玩家飞船向下移动
    if (keyboardState[SDL_SCANCODE_S]){
        player.position.y += deltaTime *player.speed;
    }
    //控制玩家飞船向左移动
    if (keyboardState[SDL_SCANCODE_A]){
        player.position.x -= deltaTime *player.speed;
    }
    //控制玩家飞船向右移动
    if (keyboardState[SDL_SCANCODE_D]){
        player.position.x += deltaTime *player.speed;
    }
/*******限制玩家飞船移动范围*******************************************/
    

     //限制玩家飞船向上移动范围
    if (player.position.y < 0){
        player.position.y = 0;
    }

    //限制玩家飞船向下移动范围
    if (player.position.y > game.getWindowHeight() - player.height){
        player.position.y = static_cast<float>(game.getWindowHeight()) - static_cast<float>(player.height);
    }

    //限制玩家飞船向左移动范围
    if (player.position.x < 0){
        player.position.x = 0;
    }

    //限制玩家飞船向右移动范围
    if (player.position.x > game.getWindowWidth() - player.width){
        player.position.x = static_cast<float>(game.getWindowWidth()) - static_cast<float>(player.width);
    }



    /*******控制子弹发射***********************************************/
    float fireRateMultiplier = 1.0f;
    if(skillManager.bulletSpeedUp && skillManager.bulletSpeedUp->isUsing){
        fireRateMultiplier = skillManager.bulletSpeedUp->bulletSpeedUp;
    }
    Uint32 adjustedCoolDown = static_cast<Uint32>(player.coolDown / fireRateMultiplier);
    
    if(keyboardState[SDL_SCANCODE_SPACE]){
        auto currentTime = SDL_GetTicks();
        if (currentTime - player.lastShotTime > adjustedCoolDown){
            playerbulletControl();
            player.lastShotTime = currentTime;
        }

    }   
    if (keyboardState[SDL_SCANCODE_J]){
        auto currentTime = SDL_GetTicks();
        if (currentTime - player.lastShotTime > adjustedCoolDown){
            playerbulletControl();
            player.lastShotTime = currentTime;
        }
    }

    /*******控制技能释放***********************************************/
    if(keyboardState[SDL_SCANCODE_1]){
        activateSkill(SkillType::ShieldReflect);
    }
    if(keyboardState[SDL_SCANCODE_2]){
        activateSkill(SkillType::Invincible);
    }
    if(keyboardState[SDL_SCANCODE_3]){
        activateSkill(SkillType::BulletBallisticUp);
    }

    if(keyboardState[SDL_SCANCODE_4]){
        activateSkill(SkillType::BulletSpeedUp);
    }

        
   
}
/*********玩家飞船状态更新*********************************************/
void SceneMain::updatePlayer()
{
    
    if(isDeath){
        return;//如果玩家死亡，则不执行以下代码
    }
    if(player.hp <= 0){

        isDeath = true;
        auto currentTime = SDL_GetTicks();
        auto explosion = new Explosion();
        explosion->position.x = player.position.x + player.width/2 - explosion->width/2;
        explosion->position.y = player.position.y + player.height/2 - explosion->height/2;
        explosion->startTime = currentTime;
        explosions.push_back(explosion);
        return;
    }
    for(auto enemy : enemies){
        SDL_Rect enmyRect = {
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height
        };
        SDL_Rect playerRect = {
            static_cast<int>(player.position.x),
            static_cast<int>(player.position.y),
            player.width,
            player.height
        };
        if(SDL_HasIntersection(&enmyRect,&playerRect)){
            player.hp -= 1;
            enemy->hp = 0;
            break;
        }
    }
}

/*********创建玩家子弹*********************************************/
void SceneMain::playerbulletControl()
{
    //获取弹道up技能
    int extraRows = 0;
    if (skillManager.bulletBallisticUp && skillManager.bulletBallisticUp->isUsing){
        extraRows = static_cast<int>(skillManager.bulletBallisticUp->bulletBallisticUp);        
    }
    //创建玩家子弹
    PlayerBullet *playerbullet = new PlayerBullet();
    //设置玩家子弹属性
    *playerbullet = playerBulletTemplate;
    //调整玩家子弹位置
    playerbullet->position.x = player.position.x + player.width/2 - playerbullet->width/2;
    playerbullet->position.y = player.position.y;
    //加入子弹容器
    playerBullets.push_back(playerbullet);

    //如果有弹道up技能，创建额外子弹
    if(extraRows > 0){
        float offsetX = playerbullet->width * 0.5f;        
        for(int i = 0; i <= extraRows; i++){
            PlayerBullet *extraBullet = new PlayerBullet();
            *extraBullet = playerBulletTemplate;
            extraBullet->position.x = player.position.x + offsetX * (i + 1);
            extraBullet->position.y = player.position.y;
            playerBullets.push_back(extraBullet);

            PlayerBullet *extraBullet2 = new PlayerBullet();
            *extraBullet2 = playerBulletTemplate;
            extraBullet2->position.x = player.position.x - offsetX * (i + 1);
            extraBullet2->position.y = player.position.y;
            playerBullets.push_back(extraBullet2);
        }        
    }
}

void SceneMain::updateplayerbullets(float deltaTime)
{   
    int margin = 32; //子弹超出屏幕范围删除
    for (auto it = playerBullets.begin(); it != playerBullets.end();){

        auto playerbullet = *it;
        playerbullet->position.y -= deltaTime * playerbullet->speed;
        //子弹超出屏幕范围删除
        if (playerbullet->position.y + margin < 0){
            delete playerbullet;
            it = playerBullets.erase(it);            
        }else {
            bool isHit = false;
            for(auto enemy : enemies){
                
                SDL_Rect enmyRect = {
                    static_cast<int>(enemy->position.x),
                    static_cast<int>(enemy->position.y),
                    enemy->width,
                    enemy->height
                };
                SDL_Rect playerBulletRect = {
                    static_cast<int>(playerbullet->position.x),
                    static_cast<int>(playerbullet->position.y),
                    playerbullet->width,
                    playerbullet->height
                };
                if(SDL_HasIntersection(&enmyRect,&playerBulletRect)){
                    enemy->hp -= playerbullet->damage;
                    delete playerbullet;
                    it = playerBullets.erase(it);
                    isHit = true;
                    break;
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
   for(auto playerbullet : playerBullets){
        SDL_Rect PlayerBulletRect = { 
                            static_cast<int>(playerbullet->position.x),
                            static_cast<int>(playerbullet->position.y),
                            playerbullet->width,
                            playerbullet->height};                            
        SDL_RenderCopy(game.getRenderer(), playerbullet->texture, NULL,&PlayerBulletRect);
   }
}

/*********创建敌人飞船*********************************************/
void SceneMain::spawnEnemy()
{
    
    if(dis(gen) > 1 / 60.0f){
        return;    
    }
    Enemy *enemy = new Enemy(enemyTemplate);
    enemy ->position.x =dis(gen) * (game.getWindowWidth() - enemy->width);
    enemy ->position.y = static_cast<float>(-enemy->height);
    enemies.push_back(enemy);
}

void SceneMain::updateEnemies(float deltaTime)
{
    auto currentTime = SDL_GetTicks();
    for (auto it = enemies.begin(); it != enemies.end();){
        auto enemy = *it;
        enemy->position.y += deltaTime * enemy->speed;
        if (enemy->position.y > game.getWindowHeight()){
            delete enemy;
            it = enemies.erase(it);
        }else {
            if(currentTime - enemy->lastShotTime > enemy->coolDown && isDeath == false){
                enemybulletControl(enemy);
                enemy->lastShotTime = currentTime;                
            }
            if(enemy->hp <= 0){
                enemyExplode(enemy);
                it = enemies.erase(it);
            }
            else {
                ++it;
            }
            
        }
    }    
}

void SceneMain::renderEnemies()
{
    for(auto enemy : enemies){
        SDL_Rect enemyRect = { 
                            static_cast<int>(enemy->position.x),
                            static_cast<int>(enemy->position.y),
                            enemy->width,
                            enemy->height};                            
        SDL_RenderCopy(game.getRenderer(), enemy->texture, NULL,&enemyRect);
    }
}

/*********创建敌人子弹*********************************************/
void SceneMain::enemybulletControl(Enemy *enemy)
{
    auto enemybullet = new EnemyBullet();
    *enemybullet = enemyBulletTemplate;
    enemybullet->position.x = enemy->position.x + enemy->width/2 - enemybullet->width/2;
    enemybullet->position.y = enemy->position.y + enemy->height/2 - enemybullet->height/2;
    enemybullet->direction = getEnemyBulletDirecition(enemy);
    enemyBullets.push_back(enemybullet);
}
SDL_FPoint SceneMain::getEnemyBulletDirecition(Enemy *enemy)
{
    auto x = player.position.x + player.width/2 - enemy->position.x - enemy->width/2;
    auto y = player.position.y + player.height/2 - enemy->position.y - enemy->height/2;
    auto length = sqrt(x * x + y * y);
    x /= length;
    y /= length;
    return SDL_FPoint{x,y};
}

void SceneMain::updateEnemyBullets(float deltaTime)
{
    auto margin = 32;
    for (auto it = enemyBullets.begin(); it != enemyBullets.end();){
        auto enemybullet = *it;
        enemybullet->position.x += deltaTime * enemybullet->speed * enemybullet->direction.x;
        enemybullet->position.y += deltaTime * enemybullet->speed * enemybullet->direction.y;
        if (enemybullet->position.y > game.getWindowHeight() + margin ||
            enemybullet->position.y < -margin ||
            enemybullet->position.x > game.getWindowHeight() + margin ||
            enemybullet->position.x < -margin){                
                delete enemybullet;
                it = enemyBullets.erase(it);
        }
        else {            
            SDL_Rect enemyBulletRect = {
                static_cast<int>(enemybullet->position.x),
                static_cast<int>(enemybullet->position.y),
                enemybullet->width,
                enemybullet->height
            }; 
                            
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height
            };
            if(SDL_HasIntersection(&enemyBulletRect,&playerRect) && isDeath == false){
                //检查无敌技能
                if(skillManager.invincible && 
                    skillManager.invincible->isUsing && 
                    skillManager.invincible->invincible){
                        delete enemybullet;
                        it = enemyBullets.erase(it);                                      
                }
                //检查盾反技能
                else if(skillManager.shieldReflect && 
                    skillManager.shieldReflect->isUsing && 
                    skillManager.shieldReflect->reflectBulletts){
                        enemybullet->direction.x = -enemybullet->direction.x;
                        enemybullet->direction.y = -enemybullet->direction.y;

                        auto playerBullet = new PlayerBullet();
                        playerBullet->texture = enemybullet->texture;
                        playerBullet->position = enemybullet->position;
                        playerBullet->width = enemybullet->width;
                        playerBullet->height = enemybullet->height;
                        playerBullet->speed = enemybullet->speed;
                        playerBullet->damage = enemybullet->damage;
                        playerBullets.push_back(playerBullet);

                        delete enemybullet;
                        it = enemyBullets.erase(it);

                        player.hp -= static_cast<int>(enemybullet->damage * skillManager.shieldReflect->damageReflection);
                }
                else if(player.shield > 0){
                    player.shield -= enemybullet->damage;
                    delete enemybullet;
                    it = enemyBullets.erase(it);    
                }
                else{
                    player.hp -= enemybullet->damage;
                    delete enemybullet;
                    it = enemyBullets.erase(it);

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
    for(auto enemybullet : enemyBullets){
        SDL_Rect enemyBulletRect = { 
                            static_cast<int>(enemybullet->position.x),
                            static_cast<int>(enemybullet->position.y),
                            enemybullet->width,
                            enemybullet->height};                            
        double angle = atan2f(enemybullet->direction.y, enemybullet->direction.x) * 180.0f / M_PI - 90.0f;
        SDL_RenderCopyEx(game.getRenderer(), enemybullet->texture, NULL,&enemyBulletRect,angle,NULL,SDL_FLIP_NONE);
   }
}

/*********敌人爆炸*********************************************/
void SceneMain::enemyExplode(Enemy *enemy)
{
    auto currentTime = SDL_GetTicks();
    auto explosion = new Explosion(explosionTemplate);
    explosion->position.x = enemy->position.x + enemy->width/2 - explosion->width/2;
    explosion->position.y = enemy->position.y + enemy->height/2 - explosion->height/2;
    explosion->startTime = currentTime;
    explosions.push_back(explosion);   
    dropItem(enemy);  
    delete enemy;
}

void SceneMain::updateExplosions(float)
{
    auto currentTime = SDL_GetTicks();
    for (auto it = explosions.begin(); it != explosions.end();){
        auto explosion = *it;
        explosion->currentFrame = (currentTime - explosion->startTime) * explosion->FPS / 1000; 
        if (explosion->currentFrame >= explosion->totalFrame){
            delete explosion;
            it = explosions.erase(it);
        }
        else {
            ++it;
        }
    }
}

void SceneMain::renderExplosions()
{
    for(auto explosion : explosions){
        SDL_Rect src  = {
            explosion->currentFrame * explosion->width,
            0,
            explosion->width,
            explosion->height
        };
        SDL_Rect dst = {
            static_cast<int>(explosion->position.x),
            static_cast<int>(explosion->position.y),
            explosion->width,
            explosion->height
        };
        SDL_RenderCopy(game.getRenderer(), explosion->texture, &src, &dst);
    }
    
}
/*********道具掉落*********************************************/
void SceneMain::dropItem(Enemy *enemy)
{
    float dropChance = dis(gen); 
    Item *item;
    if(dropChance < 0.2f){
        //20%概率血包
        item = new HealthPack();
        *item = itemHealthPackTemplate;
        item->type = ItemType::HealthPack;
    }
    else if(dropChance < 0.35f){
        //15%概率护盾
        item = new ShieldPack();
        *item = itemShieldPackTemplate;
        item->type = ItemType::ShieldPack;
    }
    else if(dropChance < 0.5f){
        //15%概率cd包
        item = new SkillCDPack();
        *item = itemSkillCDPackTemplate;
        item->type = ItemType::SkillCDPack;
    }
    else{
        //50%概率无道具
        return;
    }

    item->position.x = enemy->position.x + enemy->width/2 - item->width/2;
    item->position.y = enemy->position.y + enemy->height/2 - item->height/2;
    double angle = dis(gen) * 2 * M_PI;
    item->direction.x = static_cast<float>(cos(angle));
    item->direction.y = static_cast<float>(sin(angle));
    items.push_back(item);
}

void SceneMain::playerGetItem(Item *item)
{
    //血包获取
    if(item -> type == ItemType::HealthPack){
        player.hp += 1;        
        if(player.hp > player.maxHp){
            player.hp = player.maxHp;
        }
    }
    //护盾获取
    else if(item -> type == ItemType::ShieldPack){
        player.shield += 1;
        if(player.shield > player.maxShield){
            player.shield = player.maxShield;
        }
    }
    //cd包减少cd时间
    else if(item -> type == ItemType::SkillCDPack){
        skillCDPackEffect();
    }


}

void SceneMain::updateItems(float deltaTime)
{
        
    for(auto it = items.begin(); it != items.end();){
        auto item = *it;

        item->position.x += deltaTime * item->speed * item->direction.x;
        item->position.y += deltaTime * item->speed * item->direction.y;

        bool outOfScreen =  item->position.x < 0 ||
                            item->position.x > game.getWindowWidth() - item->width ||
                            item->position.y < 0 || 
                            item->position.y > game.getWindowHeight() - item->height;
        bool needBounce = false;

        //碰撞边缘三次后，超出屏幕删除
        if(item->position.x < 0 && item->bounceCount > 0){
            item->direction.x = -item->direction.x; 
            item->bounceCount--;
            needBounce = true;           
        }
        if(item->position.x > game.getWindowWidth() - item->width && item->bounceCount > 0){
            item->direction.x = -item->direction.x;
            item->bounceCount--;
            needBounce = true;
        }
        if(item->position.y < 0 && item->bounceCount > 0){
            item->direction.y = -item->direction.y;
            item->bounceCount--;
            needBounce = true;
        }
        if(item->position.y > game.getWindowHeight() - item->height && item->bounceCount > 0){
            item->direction.y = -item->direction.y;
            item->bounceCount--;
            needBounce = true;
        }

        if(outOfScreen && !needBounce){
                delete item;
                it = items.erase(it);
        }
        //碰撞检测,拾取道具
        else {
            SDL_Rect itemRect = {
                static_cast<int>(item->position.x),
                static_cast<int>(item->position.y),
                item->width,
                item->height        
            };
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height
            };
            if(SDL_HasIntersection(&itemRect,&playerRect)){
                playerGetItem(item);
                delete item;
                it = items.erase(it);
            }
            else {
                ++it;
            }
                
        }
    };
    
}

void SceneMain::renderItems()
{ 
    for(auto item : items){
        SDL_Rect itemRect = {
            static_cast<int>(item->position.x),
            static_cast<int>(item->position.y),
            item->width,
            item->height
        };
        SDL_RenderCopy(game.getRenderer(), item->texture, nullptr, &itemRect);
    }
}

void SceneMain::skillCDPackEffect()
{
    for(auto skill : skillManager.skills){
        if (skill->currentCooldownTime > 0.0f){
            skill->currentCooldownTime *= 0.7f;
            if(skill->currentCooldownTime < 0.0f){
                skill->currentCooldownTime = 0.0f;
            }
        }
    }
}

void SceneMain::initSkill()
{
    //创建盾反技能
    skillManager.shieldReflect = new ShieldReflect();
    skillManager.shieldReflect->type = SkillType::ShieldReflect;
    skillManager.shieldReflect->cooldDownTime = 7.0f;
    skillManager.shieldReflect->durationTime= 4.0f;
    skillManager.shieldReflect->currentCooldownTime = 0.0f;
    skillManager.shieldReflect->currentDurationTime = 0.0f;
    skillManager.shieldReflect->isUsing = false;
    skillManager.shieldReflect->damageReflection = 0.5f;
    skillManager.shieldReflect->reflectBulletts = false;
    skillManager.skills.push_back(skillManager.shieldReflect);

    //创建无敌技能
    skillManager.invincible = new Invincible();
    skillManager.invincible->type = SkillType::Invincible;
    skillManager.invincible->cooldDownTime = 10.0f;
    skillManager.invincible->durationTime = 3.0f;
    skillManager.invincible->currentCooldownTime = 0.0f;
    skillManager.invincible->currentDurationTime = 0.0f;
    skillManager.invincible->isUsing = false;
    skillManager.invincible->invincible = false;
    skillManager.skills.push_back(skillManager.invincible);

    //创建射速up技能
    skillManager.bulletSpeedUp = new BulletSpeedUp();
    skillManager.bulletSpeedUp->type = SkillType::BulletSpeedUp;
    skillManager.bulletSpeedUp->cooldDownTime = 10.0f;
    skillManager.bulletSpeedUp->durationTime = 20.0f;
    skillManager.bulletSpeedUp->currentCooldownTime = 0.0f;
    skillManager.bulletSpeedUp->currentDurationTime = 0.0f;
    skillManager.bulletSpeedUp->isUsing = false;
    skillManager.bulletSpeedUp->bulletSpeedUp = 2.0f;
    skillManager.skills.push_back(skillManager.bulletSpeedUp);

    //创建弹道up技能
    skillManager.bulletBallisticUp = new BulletBallisticUp();
    skillManager.bulletBallisticUp->type = SkillType::BulletBallisticUp;
    skillManager.bulletBallisticUp->cooldDownTime = 10.0f;
    skillManager.bulletBallisticUp->durationTime = 20.0f;
    skillManager.bulletBallisticUp->currentCooldownTime = 0.0f;
    skillManager.bulletBallisticUp->currentDurationTime = 0.0f;
    skillManager.bulletBallisticUp->isUsing = false;
    skillManager.bulletBallisticUp->bulletBallisticUp = 1;
    skillManager.skills.push_back(skillManager.bulletBallisticUp);

}

void SceneMain::activateSkill(SkillType skillType)
{
    for (auto skill : skillManager.skills){
        if(skill->type == skillType && skill->currentCooldownTime <= 0 && !skill->isUsing){
            skill->isUsing = true;
            skill->currentCooldownTime = skill->cooldDownTime;
            skill->currentDurationTime = skill->durationTime;  
            
            //根据技能类型激活技能
            switch (skillType)
            {
            case SkillType::ShieldReflect:
                skillManager.shieldReflect->reflectBulletts = true;
                break;
            case SkillType::Invincible:
                skillManager.invincible->invincible = true;
                break;
            case SkillType::BulletSpeedUp:
                skillManager.bulletSpeedUp->bulletSpeedUp = 2.0f;
                break;
            case SkillType::BulletBallisticUp:
                skillManager.bulletBallisticUp->bulletBallisticUp++;
                break;
            }            
        }
    }
}

void SceneMain::updateSkill(float deltaTime)
{
    for (auto skill : skillManager.skills){
        if(skill->currentCooldownTime >0){
            skill->currentCooldownTime -= deltaTime;
            if(skill->currentCooldownTime < 0){
                skill->currentCooldownTime = 0;                
            }
        }

        //更新效果持续时间
        if(skill->isUsing){
            skill->currentCooldownTime -= deltaTime;
            
            if(skill->currentDurationTime <= 0){
                skill->isUsing = false;
                skill->currentDurationTime = 0;
                //根据技能类型取消技能效果
                switch (skill->type)
                {
                case SkillType::ShieldReflect:
                    skillManager.shieldReflect->reflectBulletts = false;
                    break;
                case SkillType::Invincible:
                    skillManager.invincible->invincible = false;
                    break;
                case SkillType::BulletSpeedUp:
                    skillManager.bulletSpeedUp->bulletSpeedUp = 1.0f;
                    break;
                case SkillType::BulletBallisticUp:
                    skillManager.bulletBallisticUp->bulletBallisticUp = 0;
                    break;
                }
            }
        }
        
    }
}

void SceneMain::renderSkill()
{
    //渲染盾反技能效果
    if(skillManager.shieldReflect->reflectBulletts && 
        skillManager.shieldReflect->isUsing && 
        skillManager.shieldReflect->reflectBulletts){
            //在玩家周围绘制光晕效果
            SDL_Rect shieldRect = {
                static_cast<int>(player.position.x - 10),
                static_cast<int>(player.position.y - 10),
                player.width + 20,
                player.height + 20
            };
            SDL_SetRenderDrawColor(game.getRenderer(), 0, 191, 255, 128);
            SDL_RenderFillRect(game.getRenderer(), &shieldRect);
        
    }

    //渲染无敌技能效果
    if(skillManager.invincible->invincible && 
        skillManager.invincible->isUsing && 
        skillManager.invincible->invincible){
            //在玩家周围绘制发光效果
            SDL_Rect glowRect = {
                static_cast<int>(player.position.x - 5),
                static_cast<int>(player.position.y - 5),
                player.width + 10,
                player.height + 10
            };
            SDL_SetRenderDrawColor(game.getRenderer(), 255, 255, 0, 128);
            SDL_RenderFillRect(game.getRenderer(), &glowRect);
        
    }
}
