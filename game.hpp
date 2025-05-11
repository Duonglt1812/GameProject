#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include "Vector2D.h"

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();
    void ShowGameOverScreen();

    bool running() { return isRunning; }

    static SDL_Renderer* renderer;

private:
    bool isRunning;
    SDL_Window* window;
    enum GameState { INTRO, STAGE1, PLAYING, STAGE2, PLAYING_STAGE2, WIN, LOSE };
    GameState gameState;
    SDL_Texture* introTexture;
    SDL_Texture* introTexture2;
    SDL_Texture* stage1Texture;
    SDL_Texture* stage2Texture;
    SDL_Texture* winTexture;
    SDL_Texture* loseTexture;
    Uint32 gameOverTime;
    Uint32 introTime;
    Uint32 stageTime;
    Uint32 introStartTime;
    const Uint32 introDuration = 3000;
    const Uint32 stageDuration = 2000;
    const Uint32 displayDuration = 3000;
    Mix_Chunk* attackSound;
    Mix_Chunk* winSound;
    Mix_Chunk* loseSound;
    Mix_Music* themeSound;
    SDL_Texture* playButtonTexture;
    SDL_Texture* quitButtonTexture;
    SDL_Texture* soundButtonTexture;
    SDL_Texture* playHoverTexture;
    SDL_Texture* quitHoverTexture;
    bool isMusicOn;
    bool introStarted;

    struct Item {
        SDL_Texture* texture;
        Vector2D position;
        int width;
        int height;
        bool active;
        enum ItemType { HP, SWORD } type;
        Item(SDL_Texture* tex, float x, float y, int w, int h, ItemType t) : texture(tex), position(x, y), width(w), height(h), active(true), type(t) {}
    };
    std::vector<Item> items;
    SDL_Texture* hpTexture;
    SDL_Texture* swordTexture;
    bool hasSword;
};
