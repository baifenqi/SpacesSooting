#include "SceneMain.h"
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"

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

    //渲染爆炸效果
    renderExplosions();
    

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
    if(keyboardState[SDL_SCANCODE_SPACE]){
        auto currentTime = SDL_GetTicks();
        if (currentTime - player.lastShotTime > player.coolDown){
            playerbulletControl();
            player.lastShotTime = currentTime;
        }

    }   
    if (keyboardState[SDL_SCANCODE_J]){
        auto currentTime = SDL_GetTicks();
        if (currentTime - player.lastShotTime > player.coolDown){
            playerbulletControl();
            player.lastShotTime = currentTime;
        }
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
    //创建玩家子弹
    PlayerBullet *playerbullet = new PlayerBullet();
    //设置玩家子弹属性
    *playerbullet = playerBulletTemplate;
    //调整玩家子弹位置
    playerbullet->position.x = player.position.x + player.width/2 - playerbullet->width/2;
    playerbullet->position.y = player.position.y;
    //加入子弹容器
    playerBullets.push_back(playerbullet);
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
                player.hp -= enemybullet->damage;
                delete enemybullet;
                it = enemyBullets.erase(it);    
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

void SceneMain::enemyExplode(Enemy *enemy)
{
    auto currentTime = SDL_GetTicks();
    auto explosion = new Explosion(explosionTemplate);
    explosion->position.x = enemy->position.x + enemy->width/2 - explosion->width/2;
    explosion->position.y = enemy->position.y + enemy->height/2 - explosion->height/2;
    explosion->startTime = currentTime;
    explosions.push_back(explosion);
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
