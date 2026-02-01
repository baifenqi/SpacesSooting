#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "Game.h"

int main(int , char**) {
    Game& game = Game::getInstance();//获取单例实例
    game.init();//初始化
    game.run();//运行
    
    return 0;
}         