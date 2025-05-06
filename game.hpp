#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include "ECS.h"

class Game {
private:
    enum GameState {
        INTRO,
        PLAYING,
        WIN,
        LOSE
    };

    GameState gameState;
    bool isRunning;
    SDL_Window* window;
    SDL_Texture* introTexture;
    SDL_Texture* winTexture;
    SDL_Texture* loseTexture;
    Uint32 gameOverTime;
    Uint32 introTime;
    const Uint32 introDuration = 3000;
    const Uint32 displayDuration = 3000;

    // Biến cho âm thanh
    Mix_Chunk* attackSound;
    Mix_Chunk* winSound;
    Mix_Chunk* loseSound;
    Mix_Music* themeSound;

    void ShowGameOverScreen();

public:
    static SDL_Renderer* renderer;
    Game();
    ~Game();

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();

    bool running() { return isRunning; }
};

