#include "game.hpp"
#include <iostream>
#include "TextureManager.h"
#include "Map.h"
#include "ECS.h"
#include "Component.h"
#include "Vector2D.h"

using namespace std;

Map* gamemap;
Manager manager;

SDL_Renderer* Game::renderer = nullptr;

auto& player(manager.addEntity());
std::vector<Entity*> enemies;

Game::Game() : isRunning(false), window(nullptr), gameState(INTRO), introTexture(nullptr), stage1Texture(nullptr), stage2Texture(nullptr), winTexture(nullptr), loseTexture(nullptr), gameOverTime(0), introTime(0), stageTime(0), attackSound(nullptr), winSound(nullptr), loseSound(nullptr), themeSound(nullptr) {}
Game::~Game() {}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        cout << "Initialized SDL!" << endl;

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            cout << "SDL_mixer could not initialize! Error: " << Mix_GetError() << endl;
            isRunning = false;
            return;
        }
        cout << "Initialized SDL_mixer!" << endl;

        window = SDL_CreateWindow(title, xpos, ypos, 1280, 960, flags);

        if (window) { cout << "Window created!" << endl; }
        renderer = SDL_CreateRenderer(window, -1, 0);

        if (renderer) {
            cout << "Renderer created!" << endl;
            SDL_SetRenderDrawColor(renderer, 78, 173, 196, 255);
        }
        isRunning = true;
    } else {
        isRunning = false;
    }

    gamemap = new Map();

    introTexture = TextureManager::LoadTexture("assets/intro.png");
    stage1Texture = TextureManager::LoadTexture("assets/stage1.png");
    stage2Texture = TextureManager::LoadTexture("assets/stage2.png");
    winTexture = TextureManager::LoadTexture("assets/win.png");
    loseTexture = TextureManager::LoadTexture("assets/lose.png");
    if (!stage1Texture) {
        std::cout << "Failed to load stage1 texture!" << std::endl;
    }
    if (!stage2Texture) {
        std::cout << "Failed to load stage2 texture!" << std::endl;
    }
    if (!winTexture) {
        std::cout << "Failed to load win texture!" << std::endl;
    }
    if (!loseTexture) {
        std::cout << "Failed to load lose texture!" << std::endl;
    }

    attackSound = Mix_LoadWAV("assets/attack_sound.mp3");
    winSound = Mix_LoadWAV("assets/win_sound.mp3");
    loseSound = Mix_LoadWAV("assets/lose_sound.mp3");
    themeSound = Mix_LoadMUS("assets/theme_sound.mp3");

    if (themeSound && !Mix_PlayingMusic()) {
        Mix_PlayMusic(themeSound, -1);
    }

    introTime = SDL_GetTicks();

    player.addComponent<TransformComponent>(144.0f, 192.0f, 32, 32, 1.5);
    player.addComponent<SpriteComponent>("assets/player.png", "assets/player_walk.png", "assets/player_attack.png", true);
    player.addComponent<KeyboardController>();
    player.addComponent<HealthComponent>(100);

    float enemyPositions[5][2] = {
        {240, 240},
        {384, 672},
        {720, 240},
        {720, 720},
        {480, 480}
    };

    for (int i = 0; i < 5; i++) {
        auto& enemy = manager.addEntity();
        enemy.addComponent<TransformComponent>(enemyPositions[i][0], enemyPositions[i][1], 32, 32, 1.5);
        enemy.addComponent<SpriteComponent>("assets/enemy.png", "assets/enemy_walk.png", "assets/enemy_attack.png", true);
        enemy.addComponent<HealthComponent>(100);
        enemy.addComponent<EnemyAIComponent>(&player.getComponent<TransformComponent>());
        enemies.push_back(&enemy);
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
            break;
        }
    }
}

void Game::ShowGameOverScreen() {
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = nullptr;
    window = nullptr;

    SDL_DestroyTexture(introTexture);
    SDL_DestroyTexture(stage1Texture);
    SDL_DestroyTexture(stage2Texture);
    SDL_DestroyTexture(winTexture);
    SDL_DestroyTexture(loseTexture);
    introTexture = nullptr;
    stage1Texture = nullptr;
    stage2Texture = nullptr;
    winTexture = nullptr;
    loseTexture = nullptr;

    const char* title = (gameState == WIN) ? "You Win!" : "You Lose!";
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, 0);
    if (!window) {
        isRunning = false;
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        SDL_DestroyWindow(window);
        isRunning = false;
        return;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    stage1Texture = TextureManager::LoadTexture("assets/stage1.png");
    stage2Texture = TextureManager::LoadTexture("assets/stage2.png");
    winTexture = TextureManager::LoadTexture("assets/win.png");
    loseTexture = TextureManager::LoadTexture("assets/lose.png");
    if (!stage1Texture) {
        std::cout << "Failed to load stage1 texture!" << std::endl;
    }
    if (!stage2Texture) {
        std::cout << "Failed to load stage2 texture!" << std::endl;
    }
    if (!winTexture) {
        std::cout << "Failed to load win texture!" << std::endl;
    }
    if (!loseTexture) {
        std::cout << "Failed to load lose texture!" << std::endl;
    }

    if (gameState == WIN && winSound) {
        Mix_PlayChannel(-1, winSound, 0);
    } else if (gameState == LOSE && loseSound) {
        Mix_PlayChannel(-1, loseSound, 0);
    }
}

void Game::update() {
    if (gameState == INTRO) {
        if (SDL_GetTicks() - introTime >= introDuration) {
            gameState = STAGE1;
            stageTime = SDL_GetTicks();
        }
        return;
    }

    if (gameState == STAGE1) {
        if (SDL_GetTicks() - stageTime >= stageDuration) {
            gameState = PLAYING;
        }
        return;
    }

    if (gameState == STAGE2) {
        if (SDL_GetTicks() - stageTime >= stageDuration) {
            gameState = PLAYING_STAGE2;
            delete gamemap;
            gamemap = new Map(true);
            enemies.clear();
            for (auto& e : manager.getEntities()) {
                if (e->hasComponent<EnemyAIComponent>()) {
                    e->destroy();
                }
            }
            manager.refresh();
            auto& playerTransform = player.getComponent<TransformComponent>();
            auto& playerHealth = player.getComponent<HealthComponent>();
            playerTransform.position = Vector2D(144.0f, 192.0f);
            playerTransform.init();
            playerHealth.init();
            float nearbyEnemyPositions[9][2] = {
                {912, 432},
                {912, 480},
                {912, 528},
                {960, 432},
                {960, 528},
                {1008, 432},
                {1008, 480},
                {1008, 528},
                {960, 576}
            };
            for (int i = 0; i < 9; i++) {
                auto& enemy = manager.addEntity();
                enemy.addComponent<TransformComponent>(nearbyEnemyPositions[i][0], nearbyEnemyPositions[i][1], 32, 32, 1.5);
                enemy.addComponent<SpriteComponent>("assets/enemy.png", "assets/enemy_walk.png", "assets/enemy_attack.png", true);
                enemy.addComponent<HealthComponent>(100);
                enemy.addComponent<EnemyAIComponent>(&player.getComponent<TransformComponent>());
                enemies.push_back(&enemy);
            }
            auto& boss = manager.addEntity();
            boss.addComponent<TransformComponent>(960, 480, 32, 32, 2.0);
            boss.addComponent<SpriteComponent>("assets/boss.png", "assets/boss_walk.png", "assets/boss_attack.png", true);
            boss.addComponent<HealthComponent>(400);
            boss.addComponent<EnemyAIComponent>(&player.getComponent<TransformComponent>(), true);
            enemies.push_back(&boss);
        }
        return;
    }

    if (gameState != PLAYING && gameState != PLAYING_STAGE2) {
        if (SDL_GetTicks() - gameOverTime >= displayDuration) {
            isRunning = false;
        }
        return;
    }

    auto& playerTransform = player.getComponent<TransformComponent>();
    auto& playerHealth = player.getComponent<HealthComponent>();

    Vector2D oldPosition = playerTransform.position;

    manager.update();

    Vector2D newPosition = oldPosition + playerTransform.velocity * playerTransform.currentSpeed;

    int pw = playerTransform.width;
    int ph = playerTransform.height;

    Vector2D topLeft     = newPosition;
    Vector2D topRight    = newPosition + Vector2D(pw - 1, 0);
    Vector2D bottomLeft  = newPosition + Vector2D(0, ph - 1);
    Vector2D bottomRight = newPosition + Vector2D(pw - 1, ph - 1);

    int idTL = gamemap->getTileID(static_cast<int>(topLeft.x), static_cast<int>(topLeft.y));
    int idTR = gamemap->getTileID(static_cast<int>(topRight.x), static_cast<int>(topRight.y));
    int idBL = gamemap->getTileID(static_cast<int>(bottomLeft.x), static_cast<int>(bottomLeft.y));
    int idBR = gamemap->getTileID(static_cast<int>(bottomRight.x), static_cast<int>(bottomRight.y));

    if (idTL == 0 || idTR == 0 || idBL == 0 || idBR == 0) {
        playerTransform.position = oldPosition;
    } else {
        playerTransform.position = newPosition;

        if (idTL == 1 || idTR == 1 || idBL == 1 || idBR == 1) {
            playerTransform.setSpeed(playerTransform.baseSpeed / 2);
        } else {
            playerTransform.resetSpeed();
        }
    }

    if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_SPACE]) {
        for (auto* enemy : enemies) {
            if (!enemy->isActive()) continue;
            auto& enemyTransform = enemy->getComponent<TransformComponent>();
            float distance = (enemyTransform.position - playerTransform.position).magnitude();
            if (distance < 70.0f) {
                auto& enemySprite = enemy->getComponent<SpriteComponent>();
                auto& enemyHealth = enemy->getComponent<HealthComponent>();
                enemySprite.playAttack();
                enemyHealth.takeDamage(10);
                if (attackSound) {
                    if (Mix_PlayChannel(-1, attackSound, 0) == -1) {
                        std::cout << "Failed to play attack sound! Error: " << Mix_GetError() << std::endl;
                    }
                }
                if (enemyHealth.isDead()) {
                    enemy->destroy();
                    std::cout << "Enemy destroyed!" << std::endl;
                }
            }
        }
    }

    if (playerHealth.isDead()) {
        player.destroy();
        std::cout << "Player died! Game over!" << std::endl;
        gameState = LOSE;
        gameOverTime = SDL_GetTicks();
        ShowGameOverScreen();
    }

    int enemiesAlive = 0;
    for (auto& e : manager.getEntities()) {
        if (e->isActive() && e->hasComponent<EnemyAIComponent>()) {
            enemiesAlive++;
        }
    }
    std::cout << "Enemies alive: " << enemiesAlive << std::endl;
    if (enemiesAlive == 0) {
        if (gameState == PLAYING) {
            std::cout << "All enemies defeated! Proceed to Stage 2!" << std::endl;
            gameState = STAGE2;
            stageTime = SDL_GetTicks();
        } else if (gameState == PLAYING_STAGE2) {
            std::cout << "All enemies and boss defeated! You Win!" << std::endl;
            gameState = WIN;
            gameOverTime = SDL_GetTicks();
            ShowGameOverScreen();
        }
    }

    manager.refresh();
}

void Game::render() {
    SDL_RenderClear(renderer);

    if (gameState == INTRO && introTexture) {
        SDL_Rect dest = {0, 0, 1280, 960};
        SDL_Rect src = {0, 0, 1280, 960};
        TextureManager::Draw(introTexture, src, dest);
    } else if (gameState == STAGE1 && stage1Texture) {
        SDL_Rect dest = {0, 0, 1280, 960};
        SDL_Rect src = {0, 0, 1280, 960};
        TextureManager::Draw(stage1Texture, src, dest);
    } else if (gameState == STAGE2 && stage2Texture) {
        SDL_Rect dest = {0, 0, 1280, 960};
        SDL_Rect src = {0, 0, 1280, 960};
        TextureManager::Draw(stage2Texture, src, dest);
    } else if (gameState == WIN && winTexture) {
        SDL_Rect dest = {0, 0, 1280, 960};
        SDL_Rect src = {0, 0, 1280, 960};
        TextureManager::Draw(winTexture, src, dest);
    } else if (gameState == LOSE && loseTexture) {
        SDL_Rect dest = {0, 0, 1280, 960};
        SDL_Rect src = {0, 0, 1280, 960};
        TextureManager::Draw(loseTexture, src, dest);
    } else {
        gamemap->DrawMap();
        manager.draw();
        for (auto& e : manager.getEntities()) {
            if (e->isActive() && e->hasComponent<HealthComponent>()) {
                e->getComponent<HealthComponent>().draw();
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    Mix_FreeChunk(attackSound);
    Mix_FreeChunk(winSound);
    Mix_FreeChunk(loseSound);
    Mix_FreeMusic(themeSound);

    Mix_CloseAudio();

    SDL_DestroyTexture(introTexture);
    SDL_DestroyTexture(stage1Texture);
    SDL_DestroyTexture(stage2Texture);
    SDL_DestroyTexture(winTexture);
    SDL_DestroyTexture(loseTexture);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    cout << "Game cleaned!" << endl;
}
