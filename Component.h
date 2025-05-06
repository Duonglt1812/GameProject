#pragma once
#include "ECS.h"
#include "Vector2D.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include "TextureManager.h"
#include <iostream>

class TransformComponent;
class SpriteComponent;
class HealthComponent;

class TransformComponent : public Component {
public:
    Vector2D position;
    Vector2D velocity;
    int height = 32;
    int width = 32;
    float scale = 1.0f;
    float baseSpeed = 3.0f;
    float currentSpeed = baseSpeed;

    TransformComponent() {
        position.x = 0.0f;
        position.y = 0.0f;
    }

    TransformComponent(float x, float y) {
        position.x = x;
        position.y = y;
    }

    TransformComponent(float x, float y, int h, int w, float sc) {
        position.x = x;
        position.y = y;
        height = h;
        width = w;
        scale = sc;
    }

    void init() override {
        velocity.x = 0;
        velocity.y = 0;
    }

    void update() override {
        position += velocity * currentSpeed;
        if (velocity.x != 0 || velocity.y != 0) {
        }
    }

    void setSpeed(float speed) {
        if (currentSpeed != speed) {
            currentSpeed = speed;
        }
    }

    void resetSpeed() {
        if (currentSpeed != baseSpeed) {
            currentSpeed = baseSpeed;
        }
    }
};

class SpriteComponent : public Component {
private:
    TransformComponent* transform;
    SDL_Texture* idleTexture;
    SDL_Texture* walkTexture;
    SDL_Texture* attackTexture;
    SDL_Rect srcRect, destRect;
    bool animated = false;
    int walkFrames = 6;
    int attackFrames = 6;
    int speed = 100;
    int animIndex = 0;
    bool isAttacking = false;
    float attackTimer = 0.0f;
    bool isMoving = false;
    bool flipHorizontal = false;

public:
    SpriteComponent() = default;
    SpriteComponent(const char* idlePath, const char* walkPath, const char* attackPath, bool isAnimated = false) {
        idleTexture = TextureManager::LoadTexture(idlePath);
        walkTexture = TextureManager::LoadTexture(walkPath);
        attackTexture = TextureManager::LoadTexture(attackPath);
        animated = isAnimated;
    }

    ~SpriteComponent() {
        SDL_DestroyTexture(idleTexture);
        SDL_DestroyTexture(walkTexture);
        SDL_DestroyTexture(attackTexture);
    }

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
        if (!transform) {
            std::cout << "SpriteComponent: TransformComponent not found!" << std::endl;
        }
        srcRect.x = srcRect.y = 0;
        srcRect.w = transform ? transform->width : 32;
        srcRect.h = transform ? transform->height : 32;
    }

    void update() override {
        if (!transform) return;
        isMoving = (transform->velocity.x != 0 || transform->velocity.y != 0);

        if (transform->velocity.x < 0) {
            flipHorizontal = true;
        } else if (transform->velocity.x > 0) {
            flipHorizontal = false;
        }

        if (animated) {
            if (isAttacking) {
                srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % attackFrames);
                srcRect.y = 0;
                attackTimer -= 1.0f / 60.0f;
                if (attackTimer <= 0) isAttacking = false;
            } else if (isMoving) {
                srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % walkFrames);
                srcRect.y = 0;
            } else {
                srcRect.x = 0;
                srcRect.y = 0;
            }
        }

        destRect.x = static_cast<int>(transform->position.x);
        destRect.y = static_cast<int>(transform->position.y);
        destRect.w = transform->width * transform->scale;
        destRect.h = transform->height * transform->scale;
    }

    void draw() override {
        if (!transform) return;
        if (isAttacking && attackTexture) {
            TextureManager::DrawFlipped(attackTexture, srcRect, destRect, flipHorizontal);
        } else if (isMoving && walkTexture) {
            TextureManager::DrawFlipped(walkTexture, srcRect, destRect, flipHorizontal);
        } else if (idleTexture) {
            TextureManager::DrawFlipped(idleTexture, srcRect, destRect, flipHorizontal);
        } else {
            std::cout << "SpriteComponent: Cannot draw texture" << std::endl;
        }
    }

    void playAttack() {
        if (attackFrames > 0) {
            isAttacking = true;
            attackTimer = 0.5f;
        }
    }
};

class KeyboardController : public Component {
private:
    TransformComponent* transform;

public:
    KeyboardController() {}

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
    }

    void update() override {

        auto state = SDL_GetKeyboardState(nullptr);

        transform->velocity.x = 0;
        transform->velocity.y = 0;

        if (state[SDL_SCANCODE_W]) {
            transform->velocity.y = -1;
        }
        if (state[SDL_SCANCODE_S]) {
            transform->velocity.y = 1;
        }
        if (state[SDL_SCANCODE_A]) {
            transform->velocity.x = -1;
        }
        if (state[SDL_SCANCODE_D]) {
            transform->velocity.x = 1;
        }

        if (state[SDL_SCANCODE_SPACE]) {
            auto& sprite = entity->getComponent<SpriteComponent>();
            sprite.playAttack();
        }

        float magnitude = transform->velocity.magnitude();
        if (magnitude > 0) {
            transform->velocity = transform->velocity / magnitude;
        }
    }
};

class HealthComponent : public Component {
private:
    TransformComponent* transform;
    int health;
    int maxHealth;

public:
    HealthComponent(int hp) : health(hp), maxHealth(hp) {}

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
    }

    void draw() override {
        if (!transform) return;

        SDL_Rect healthBar;
        healthBar.x = transform->position.x;
        healthBar.y = transform->position.y - 10;
        healthBar.w = transform->width * (health / static_cast<float>(maxHealth));
        healthBar.h = 5;

        SDL_SetRenderDrawColor(Game::renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(Game::renderer, &healthBar);
    }

    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    bool isDead() const {
        return health <= 0;
    }

    int getHealth() const {
        return health;
    }
};

class EnemyAIComponent : public Component {
private:
    TransformComponent* transform;
    TransformComponent* playerTransform;
    SpriteComponent* sprite;
    float attackRange = 70.0f;
    float chaseRange = 200.0f;
    float attackCooldown = 1.0f;
    float attackTimer = 0.0f;
    Mix_Chunk* attackSound;

public:
    EnemyAIComponent(TransformComponent* playerTrans) : playerTransform(playerTrans) {
        attackSound = Mix_LoadWAV("assets/attack_sound.mp3");
    }

    ~EnemyAIComponent() {
        Mix_FreeChunk(attackSound);
    }

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
        sprite = &entity->getComponent<SpriteComponent>();
    }

    void update() override {
        if (!playerTransform) return;

        Vector2D direction = playerTransform->position - transform->position;
        float distance = direction.magnitude();

        if (attackTimer > 0) {
            attackTimer -= 1.0f / 60.0f;
        }

        if (distance <= chaseRange) {
            if (distance < attackRange && attackTimer <= 0) {
                sprite->playAttack();
                auto& playerHealth = playerTransform->entity->getComponent<HealthComponent>();
                playerHealth.takeDamage(10);
                if (attackSound) {
                    if (Mix_PlayChannel(-1, attackSound, 0) == -1) {
                        std::cout << "Failed to play attack sound! Error: " << Mix_GetError() << std::endl;
                    }
                }
                attackTimer = attackCooldown;
            }

            if (distance > attackRange) {
                if (distance > 1.0f) {
                    direction = direction / distance;
                }
                transform->velocity = direction;
            } else {
                transform->velocity.x = 0;
                transform->velocity.y = 0;
            }
        } else {
            transform->velocity.x = 0;
            transform->velocity.y = 0;
        }
    }
};
