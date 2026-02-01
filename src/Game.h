#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include "scene.h"

class Game {
    public:
        static Game& getInstance(){
            static Game instance;
            return instance;
        };

        ~Game();
        void run();
        void init();
        void clean();        
        void changeScene(Scene* scene);

        void update();
        void render();
        void handleEvent(SDL_Event *event);

        SDL_Window* getWindow() { return window; }
        SDL_Renderer* getRenderer() { return renderer; }

        int getWindowWidth() { return windowWidth; }
        int getWindowHeight() { return windowHeight; }
    private:
        Game();
        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;
        
        bool isRunning = true;

        Scene* currentScene = nullptr;
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;

        int windowWidth = 720;
        int windowHeight = 960;

        int fps = 60;
        Uint32 frameTime;
        float deltaTime;
};

#endif 