#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include<bits/stdc++.h>

class Entity;

class Component {
public:
    Entity* entity;
    virtual void init() {}
    virtual void update() {}
    virtual void draw() {}
    virtual ~Component() {}
};

class Entity {
private:
    bool active = true;
    std::vector<std::unique_ptr<Component>> components;

public:
    void update() {
        for (auto& c : components) {
            c->update();
        }
    }

    void draw() {
        for (auto& c : components) c->draw();
    }

    bool isActive() const { return active; }
    void destroy() { active = false; }

    template<typename T, typename... TArgs>
    T& addComponent(TArgs&&... args) {
        T* c(new T(std::forward<TArgs>(args)...));
        c->entity = this;
        std::unique_ptr<Component> uPtr{ c };
        components.emplace_back(std::move(uPtr));
        c->init();
        return *c;
    }

    template<typename T>
    T& getComponent() {
        for (auto& c : components) {
            T* t = dynamic_cast<T*>(c.get());
            if (t) return *t;
        }
        throw std::runtime_error("Component not found!");
    }

    template<typename T>
    bool hasComponent() const {
        for (auto& c : components) {
            if (dynamic_cast<T*>(c.get())) return true;
        }
        return false;
    }
};

class Manager {
private:
    std::vector<std::unique_ptr<Entity>> entities;

public:
    void update() {
        for (auto& e : entities) {
            if (e->isActive()) {
                e->update();
            } else {
                std::cout << "Inactive entity" << std::endl;
            }
        }
    }

    void draw() {
        for (auto& e : entities) {
            if (e->isActive()) e->draw();
        }
    }

    void refresh() {
        entities.erase(std::remove_if(entities.begin(), entities.end(),
            [](const std::unique_ptr<Entity>& e) { return !e->isActive(); }), entities.end());
    }

    Entity& addEntity() {
        Entity* e = new Entity();
        std::unique_ptr<Entity> uPtr{ e };
        entities.emplace_back(std::move(uPtr));
        return *e;
    }

    std::vector<std::unique_ptr<Entity>>& getEntities() {
        return entities;
    }
};
