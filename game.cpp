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

Game::Game() : isRunning(false), window(nullptr), gameState(INTRO), introTexture(nullptr), introTexture2(nullptr), stage1Texture(nullptr), stage2Texture(nullptr), winTexture(nullptr), loseTexture(nullptr), gameOverTime(0), introTime(0), stageTime(0), introStartTime(0), attackSound(nullptr), winSound(nullptr), loseSound(nullptr), themeSound(nullptr), playButtonTexture(nullptr), quitButtonTexture(nullptr), soundButtonTexture(nullptr), playHoverTexture(nullptr), quitHoverTexture(nullptr), isMusicOn(true), introStarted(false), hpTexture(nullptr), swordTexture(nullptr), hasSword(false) {}
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
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }
        isRunning = true;
    } else {
        isRunning = false;
    }

    gamemap = new Map();

    introTexture = TextureManager::LoadTexture("assets/background.png");
    introTexture2 = TextureManager::LoadTexture("assets/intro.png");
    stage1Texture = TextureManager::LoadTexture("assets/stage1.png");
    stage2Texture = TextureManager::LoadTexture("assets/stage2.png");
    winTexture = TextureManager::LoadTexture("assets/win.png");
    loseTexture = TextureManager::LoadTexture("assets/lose.png");
    playButtonTexture = TextureManager::LoadTexture("assets/play_button.png");
    quitButtonTexture = TextureManager::LoadTexture("assets/quit_button.png");
    soundButtonTexture = TextureManager::LoadTexture("assets/sound_button.png");
    playHoverTexture = TextureManager::LoadTexture("assets/play_hover.png");
    quitHoverTexture = TextureManager::LoadTexture("assets/quit_hover.png");
    hpTexture = TextureManager::LoadTexture("assets/hp.png");
    swordTexture = TextureManager::LoadTexture("assets/sword.png");

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
        enemy.addComponent<EnemyAIComponent>(&player.getComponent<TransformComponent>(), gamemap);
        enemies.push_back(&enemy);
    }

    items.emplace_back(swordTexture, 700.0f, 640.0f, 32, 32, Item::SWORD);
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
            break;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            int x = event.button.x;
            int y = event.button.y;

            SDL_Rect playRect = {440, 400, 400, 210};
            if (x >= playRect.x && x <= playRect.x + playRect.w && y >= playRect.y && y <= playRect.y + playRect.h) {
                introStarted = true;
                introStartTime = SDL_GetTicks();
            }

            SDL_Rect quitRect = {440, 630, 400, 210};
            if (x >= quitRect.x && x <= quitRect.x + quitRect.w && y >= quitRect.y && y <= quitRect.y + quitRect.h) {
                isRunning = false;
            }

            SDL_Rect soundRect = {1080, 560, 150, 150};
            if (x >= soundRect.x && x <= soundRect.x + soundRect.w && y >= soundRect.y && y <= soundRect.y + soundRect.h) {
                isMusicOn = !isMusicOn;
                if (isMusicOn && themeSound && !Mix_PlayingMusic()) {
                    Mix_PlayMusic(themeSound, -1);
                } else if (!isMusicOn && Mix_PlayingMusic()) {
                    Mix_HaltMusic();
                }
            }
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
    SDL_DestroyTexture(introTexture2);
    SDL_DestroyTexture(stage1Texture);
    SDL_DestroyTexture(stage2Texture);
    SDL_DestroyTexture(winTexture);
    SDL_DestroyTexture(loseTexture);
    SDL_DestroyTexture(hpTexture);
    SDL_DestroyTexture(swordTexture);
    SDL_DestroyTexture(playButtonTexture);
    SDL_DestroyTexture(quitButtonTexture);
    SDL_DestroyTexture(soundButtonTexture);
    SDL_DestroyTexture(playHoverTexture);
    SDL_DestroyTexture(quitHoverTexture);
    introTexture = nullptr;
    introTexture2 = nullptr;
    stage1Texture = nullptr;
    stage2Texture = nullptr;
    winTexture = nullptr;
    loseTexture = nullptr;
    hpTexture = nullptr;
    swordTexture = nullptr;
    playButtonTexture = nullptr;
    quitButtonTexture = nullptr;
    soundButtonTexture = nullptr;
    playHoverTexture = nullptr;
    quitHoverTexture = nullptr;

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

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    stage1Texture = TextureManager::LoadTexture("assets/stage1.png");
    stage2Texture = TextureManager::LoadTexture("assets/stage2.png");
    winTexture = TextureManager::LoadTexture("assets/win.png");
    loseTexture = TextureManager::LoadTexture("assets/lose.png");

    if (gameState == WIN && winSound) {
        Mix_PlayChannel(-1, winSound, 0);
    } else if (gameState == LOSE && loseSound) {
        Mix_PlayChannel(-1, loseSound, 0);
    }
}

void Game::update() {
    if (gameState == INTRO) {
        if (introStarted) {
            if (SDL_GetTicks() - introStartTime < introDuration) {
            } else {
                gameState = STAGE1;
                stageTime = SDL_GetTicks();
                introStarted = false;
            }
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
                {48, 48},
                {96, 768},
                {576, 384},
                {1152, 48},
                {1152, 816},
                {912, 480},
                {912, 432},
                {912, 528},
                {864, 480}
            };
            for (int i = 0; i < 9; i++) {
                auto& enemy = manager.addEntity();
                enemy.addComponent<TransformComponent>(nearbyEnemyPositions[i][0], nearbyEnemyPositions[i][1], 32, 32, 1.5);
                enemy.addComponent<SpriteComponent>("assets/enemy.png", "assets/enemy_walk.png", "assets/enemy_attack.png", true);
                enemy.addComponent<HealthComponent>(100);
                enemy.addComponent<EnemyAIComponent>(&player.getComponent<TransformComponent>(), gamemap);
                enemies.push_back(&enemy);
            }
            auto& boss = manager.addEntity();
            boss.addComponent<TransformComponent>(960, 480, 32, 32, 2.5);
            boss.addComponent<SpriteComponent>("assets/boss.png", "assets/boss_walk.png", "assets/boss_attack.png", true);
            boss.addComponent<HealthComponent>(300);
            boss.addComponent<EnemyAIComponent>(&player.getComponent<TransformComponent>(), gamemap, true);
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
    auto& playerSprite = player.getComponent<SpriteComponent>();

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

    for (auto& item : items) {
        if (!item.active) continue;

        SDL_Rect itemRect = {static_cast<int>(item.position.x), static_cast<int>(item.position.y), item.width, item.height};
        SDL_Rect playerRect = {static_cast<int>(playerTransform.position.x), static_cast<int>(playerTransform.position.y), pw, ph};

        if (SDL_HasIntersection(&playerRect, &itemRect)) {
            if (item.type == Item::HP) {
                playerHealth.takeDamage(-10);
                std::cout << "Picked up HP! Health: " << playerHealth.getHealth() << std::endl;
                item.active = false;
            } else if (item.type == Item::SWORD && !hasSword) {
                hasSword = true;
                std::cout << "Picked up Sword! Damage doubled." << std::endl;
                playerSprite.updateTextures("assets/sword_idle.png", "assets/sword_walk.png", "assets/sword_attack.png");
                item.active = false;
            }
        }
    }

    items.erase(std::remove_if(items.begin(), items.end(), [](const Item& item) { return !item.active; }), items.end());

    if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_SPACE]) {
        for (auto* enemy : enemies) {
            if (!enemy->isActive()) continue;
            auto& enemyTransform = enemy->getComponent<TransformComponent>();
            float distance = (enemyTransform.position - playerTransform.position).magnitude();
            if (distance < 70.0f) {
                auto& enemySprite = enemy->getComponent<SpriteComponent>();
                auto& enemyHealth = enemy->getComponent<HealthComponent>();
                enemySprite.playAttack();
                int damage = hasSword ? 20 : 10;
                enemyHealth.takeDamage(damage);
                if (attackSound) {
                    if (Mix_PlayChannel(-1, attackSound, 0) == -1) {
                        std::cout << "Failed to play attack sound! Error: " << Mix_GetError() << std::endl;
                    }
                }
                if (enemyHealth.isDead()) {
                    items.emplace_back(hpTexture, enemyTransform.position.x, enemyTransform.position.y, 32, 32, Item::HP);
                    enemy->destroy();
                    std::cout << "Enemy destroyed! Dropped HP." << std::endl;
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (gameState == INTRO) {
        if (!introStarted && introTexture) {
            SDL_Rect dest = {0, 0, 1280, 960};
            SDL_Rect src = {0, 0, 1280, 960};
            TextureManager::Draw(introTexture, src, dest);

            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            SDL_Rect playSrc = {0, 0, 400, 210};
            SDL_Rect playDest = {440, 400, 400, 210};

            if (mouseX >= playDest.x && mouseX <= playDest.x + playDest.w && mouseY >= playDest.y && mouseY <= playDest.y + playDest.h) {
                TextureManager::Draw(playHoverTexture, playSrc, playDest);
            } else {
                TextureManager::Draw(playButtonTexture, playSrc, playDest);
            }

            SDL_Rect quitSrc = {0, 0, 400, 210};
            SDL_Rect quitDest = {440, 630, 400, 210};

            if (mouseX >= quitDest.x && mouseX <= quitDest.x + quitDest.w && mouseY >= quitDest.y && mouseY <= quitDest.y + quitDest.h) {
                TextureManager::Draw(quitHoverTexture, quitSrc, quitDest);
            } else {
                TextureManager::Draw(quitButtonTexture, quitSrc, quitDest);
            }

            SDL_Rect soundSrc = {0, 0, 150, 150};
            SDL_Rect soundDest = {1080, 560, 150, 150};
            TextureManager::Draw(soundButtonTexture, soundSrc, soundDest);
        } else if (introStarted && introTexture2) {
            SDL_Rect dest = {0, 0, 1280, 960};
            SDL_Rect src = {0, 0, 1280, 960};
            TextureManager::Draw(introTexture2, src, dest);
        }
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
        for (const auto& item : items) {
            if (item.active) {
                SDL_Rect src = {0, 0, item.width, item.height};
                SDL_Rect dest = {static_cast<int>(item.position.x), static_cast<int>(item.position.y), item.width, item.height};
                TextureManager::Draw(item.texture, src, dest);
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
    SDL_DestroyTexture(introTexture2);
    SDL_DestroyTexture(stage1Texture);
    SDL_DestroyTexture(stage2Texture);
    SDL_DestroyTexture(winTexture);
    SDL_DestroyTexture(loseTexture);
    SDL_DestroyTexture(playButtonTexture);
    SDL_DestroyTexture(quitButtonTexture);
    SDL_DestroyTexture(soundButtonTexture);
    SDL_DestroyTexture(playHoverTexture);
    SDL_DestroyTexture(quitHoverTexture);
    SDL_DestroyTexture(hpTexture);
    SDL_DestroyTexture(swordTexture);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    cout << "Game cleaned!" << endl;
}
