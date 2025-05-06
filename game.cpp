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

Game::Game() : isRunning(false), window(nullptr), gameState(INTRO), introTexture(nullptr), winTexture(nullptr), loseTexture(nullptr), gameOverTime(0), introTime(0), attackSound(nullptr), winSound(nullptr), loseSound(nullptr), themeSound(nullptr) {}
Game::~Game() {}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        cout << "Initialized SDL!" << endl;

        // Khởi tạo SDL_mixer
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
    winTexture = TextureManager::LoadTexture("assets/win.png");
    loseTexture = TextureManager::LoadTexture("assets/lose.png");
    if (!winTexture) {
        std::cout << "Failed to load win texture!" << std::endl;
    }
    if (!loseTexture) {
        std::cout << "Failed to load lose texture!" << std::endl;
    }

    // Load các âm thanh
    attackSound = Mix_LoadWAV("assets/attack_sound.mp3");
    winSound = Mix_LoadWAV("assets/win_sound.mp3");
    loseSound = Mix_LoadWAV("assets/lose_sound.mp3");
    themeSound = Mix_LoadMUS("assets/theme_sound.mp3");

    if (!attackSound) {
        cout << "Failed to load attack sound! Error: " << Mix_GetError() << endl;
    } else {
        cout << "Attack sound loaded successfully" << endl;
    }
    if (!winSound) {
        cout << "Failed to load win sound! Error: " << Mix_GetError() << endl;
    }
    if (!loseSound) {
        cout << "Failed to load lose sound! Error: " << Mix_GetError() << endl;
    }
    if (!themeSound) {
        cout << "Failed to load theme sound! Error: " << Mix_GetError() << endl;
    }

    // Phát nhạc nền ngay từ đầu
    if (themeSound && !Mix_PlayingMusic()) {
        Mix_PlayMusic(themeSound, -1); // -1 để loop vô hạn
    }

    // Đặt thời gian bắt đầu cho màn hình giới thiệu
    introTime = SDL_GetTicks();

    // Thêm TransformComponent trước
    player.addComponent<TransformComponent>(144.0f, 192.0f, 32, 32, 1.5);

    // Thêm các component khác
    player.addComponent<SpriteComponent>("assets/player.png", "assets/player_walk.png", "assets/player_attack.png", true);
    player.addComponent<KeyboardController>();
    player.addComponent<HealthComponent>(100);

    // Vị trí spawn của enemy
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
    // Dừng nhạc nền khi chuyển sang màn hình Win/Lose
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }

    // Xóa cửa sổ và renderer cũ
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = nullptr;
    window = nullptr;

    // Xóa texture cũ vì chúng liên kết với renderer cũ
    SDL_DestroyTexture(introTexture);
    SDL_DestroyTexture(winTexture);
    SDL_DestroyTexture(loseTexture);
    introTexture = nullptr;
    winTexture = nullptr;
    loseTexture = nullptr;

    // Tạo cửa sổ mới với tiêu đề phù hợp
    const char* title = (gameState == WIN) ? "You Win!" : "You Lose!";
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, 0);
    if (!window) {
        std::cout << "Failed to create game over window: " << SDL_GetError() << std::endl;
        isRunning = false;
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        std::cout << "Failed to create game over renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        isRunning = false;
        return;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Load lại texture cho màn hình kết quả
    winTexture = TextureManager::LoadTexture("assets/win.png");
    loseTexture = TextureManager::LoadTexture("assets/lose.png");
    if (!winTexture) {
        std::cout << "Failed to load win texture!" << std::endl;
    }
    if (!loseTexture) {
        std::cout << "Failed to load lose texture!" << std::endl;
    }

    // Phát âm thanh Win hoặc Lose
    if (gameState == WIN && winSound) {
        Mix_PlayChannel(-1, winSound, 0);
    } else if (gameState == LOSE && loseSound) {
        Mix_PlayChannel(-1, loseSound, 0);
    }
}

void Game::update() {
    // Kiểm tra trạng thái INTRO
    if (gameState == INTRO) {
        if (SDL_GetTicks() - introTime >= introDuration) {
            gameState = PLAYING;
        }
        return;
    }

    // Nếu game đã kết thúc, kiểm tra thời gian để thoát
    if (gameState != PLAYING) {
        if (SDL_GetTicks() - gameOverTime >= displayDuration) {
            isRunning = false;
        }
        return;
    }

    auto& playerTransform = player.getComponent<TransformComponent>();
    auto& playerHealth = player.getComponent<HealthComponent>();

// Lưu vị trí cũ
Vector2D oldPosition = playerTransform.position;

// Cập nhật tất cả entity
manager.update();

// Tính vị trí mới
Vector2D newPosition = oldPosition + playerTransform.velocity * playerTransform.currentSpeed;

// Kích thước nhân vật
int pw = playerTransform.width;
int ph = playerTransform.height;

// 4 góc của nhân vật tại vị trí mới
Vector2D topLeft     = newPosition;
Vector2D topRight    = newPosition + Vector2D(pw - 1, 0);
Vector2D bottomLeft  = newPosition + Vector2D(0, ph - 1);
Vector2D bottomRight = newPosition + Vector2D(pw - 1, ph - 1);

// Lấy tileID tại 4 góc
int idTL = gamemap->getTileID(static_cast<int>(topLeft.x), static_cast<int>(topLeft.y));
int idTR = gamemap->getTileID(static_cast<int>(topRight.x), static_cast<int>(topRight.y));
int idBL = gamemap->getTileID(static_cast<int>(bottomLeft.x), static_cast<int>(bottomLeft.y));
int idBR = gamemap->getTileID(static_cast<int>(bottomRight.x), static_cast<int>(bottomRight.y));

// Nếu bất kỳ góc nào là water (tileID == 0), huỷ di chuyển
if (idTL == 0 || idTR == 0 || idBL == 0 || idBR == 0) {
    playerTransform.position = oldPosition;
} else {
    playerTransform.position = newPosition;

    // Nếu có ít nhất 1 tile là grass (tileID == 1), giảm speed
    if (idTL == 1 || idTR == 1 || idBL == 1 || idBR == 1) {
        playerTransform.setSpeed(playerTransform.baseSpeed / 2);
    } else {
        playerTransform.resetSpeed();
    }
}


    // Kiểm tra tấn công của player
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
                    } else {
                        std::cout << "Playing attack sound for player attack" << std::endl;
                    }
                }
                if (enemyHealth.isDead()) {
                    enemy->destroy();
                    std::cout << "Enemy destroyed!" << std::endl;
                }
            }
        }
    }

    // Kiểm tra máu của player
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
        std::cout << "All enemies defeated! You Win!" << std::endl;
        gameState = WIN;
        gameOverTime = SDL_GetTicks();
        ShowGameOverScreen();
    }

    manager.refresh();
}

void Game::render() {
    SDL_RenderClear(renderer);

    if (gameState == INTRO && introTexture) {
        SDL_Rect dest = {0, 0, 1280, 960};
        SDL_Rect src = {0, 0, 1280, 960};
        TextureManager::Draw(introTexture, src, dest);
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
    // Giải phóng âm thanh
    Mix_FreeChunk(attackSound);
    Mix_FreeChunk(winSound);
    Mix_FreeChunk(loseSound);
    Mix_FreeMusic(themeSound);

    // Đóng SDL_mixer
    Mix_CloseAudio();

    SDL_DestroyTexture(introTexture);
    SDL_DestroyTexture(winTexture);
    SDL_DestroyTexture(loseTexture);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    cout << "Game cleaned!" << endl;
}
