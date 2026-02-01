#include "game.h"
#include "SceneMain.h"
#include <SDL.h>
#include <SDL_image.h>
Game::Game()
{

}

Game::~Game()
{
    clean();
}

void Game::run()
{
    while(isRunning){

        auto framStartTime = SDL_GetTicks();

        SDL_Event event;
        handleEvent(&event);
        update();
        render();

        auto frameEndTime = SDL_GetTicks();
        auto diff = frameEndTime - framStartTime;

        //计算帧率
        if(diff < frameTime){
            SDL_Delay(frameTime - diff);
            deltaTime = frameTime/1000.0f;
        }
        else{
            deltaTime = diff/1000.0f;
        }
        

    }
}

void Game::init()
{
    frameTime = 1000 / fps;
    //SDL 初始化
   if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"SDL_Init Error: %s\n",SDL_GetError());
        isRunning = false;
   }
   
   //创建窗口
   window = SDL_CreateWindow("SDL",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,windowWidth,windowHeight,0);
   if(window == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"SDL_CreateWindow Error: %s\n",SDL_GetError());
        isRunning = false;
    }

    //创建渲染器
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(renderer == nullptr){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"SDL_CreateRenderer Error: %s\n",SDL_GetError());
        isRunning = false;
    }
       
    //SDL_Image 初始化
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"IMG_Init Error: %s\n",SDL_GetError());
        isRunning = false;
    }


    currentScene = new SceneMain();//创建场景
    currentScene->init();//初始化场景
}

void Game::clean()
{
    //SDL_Image 清理
    IMG_Quit();
    //SDL 清理
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void Game::changeScene(Scene *scene)
{
    //清理当前场景
    if(currentScene != nullptr){
        currentScene->clean();
        delete currentScene;
    }
    //创建新场景并初始化
    currentScene = scene;
    currentScene->init();
}

void Game::handleEvent(SDL_Event *event)
{
    while (SDL_PollEvent(event))
        {
            if(event->type == SDL_QUIT){ 
                isRunning = false;
            }
            currentScene->handleEvent(event);
        } 
}

void Game::update()
{
    currentScene->update(deltaTime);
}

void Game::render()
{
     //清理
    SDL_RenderClear(renderer);
    //渲染
    currentScene->render();
    //更新
    SDL_RenderPresent(renderer);
}


