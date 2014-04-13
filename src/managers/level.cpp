#include <components/appearance/asteroid.h>
#include <components/appearance/ship.h>
#include <components/attrition.h>
#include <components/geometry.h>
#include <components/mass.h>
#include <components/momentum.h>
#include <components/position.h>
#include <systems/collision.h>
#include <systems/movement.h>
#include <systems/render.h>

#include "level.h"

const int AZTEROIDS_NUM = 20;
const float LINEAR_MOMENTUM_STEP = 1000;
const float ANGULAR_MOMENTUM_STEP = 500;

const float PI = 3.14159265359f;
const float DPI = 2 * PI;
const float HPI = 0.5f * PI;

void Level::configure() {
    system_manager->add<CollisionSystem>();
    system_manager->add<MovementSystem>(width, height);
    system_manager->add<RenderSystem>();
};

void Level::initialize() {

    // Create asteroids:
    for (int i = 0; i < AZTEROIDS_NUM; i++) {
        float mass = rand() % 20 + 20;
        entityx::Entity entity = entity_manager->create();
        entity.assign<Asteroid>(mass);
        entity.assign<Mass>(mass);
        entity.assign<Geometry>(mass);
        entity.assign<Momentum>(rand() % 1000 - 500, rand() % 1000 - 500,
                                rand() % 600 - 300);
        entity.assign<Position>(rand() % width, rand() % height,
                                rand() % 360,
                                rand() % 10, rand() % 10, rand() % 10);
    }

    // Creates user-controllable ship:
    user_ship = entity_manager->create();
    user_ship.assign<Mass>(30);
    user_ship.assign<Ship>();
    user_ship.assign<Geometry>(10);
    user_ship.assign<Momentum>(0, 0, 0);
    user_ship.assign<Attrition>(0.6f, 0.5f);
    user_ship.assign<Position>(width/2, height/2);
}

void Level::update(double dt) {
    system_manager->update<CollisionSystem>(dt);
    system_manager->update<MovementSystem>(dt);
    system_manager->update<RenderSystem>(dt);
}

void Level::send_up() {
    change_linear_momentum(LINEAR_MOMENTUM_STEP);
}

void Level::send_down() {
    change_linear_momentum(-LINEAR_MOMENTUM_STEP);
}

void Level::send_left() {
    change_angular_momemtum(ANGULAR_MOMENTUM_STEP);
}

void Level::send_right() {
    change_angular_momemtum(-ANGULAR_MOMENTUM_STEP);
}

void Level::change_angular_momemtum(float delta) {
    entityx::ptr<Momentum> momentum = user_ship.component<Momentum>();

    momentum->angular += delta;
}

void Level::change_linear_momentum(float delta) {
    entityx::ptr<Position> position = user_ship.component<Position>();
    entityx::ptr<Momentum> momentum = user_ship.component<Momentum>();
    entityx::ptr<Ship> ship = user_ship.component<Ship>();

    float angle = (position->rotation * DPI / 360.f) + HPI;
    momentum->x += cos(angle) * delta;
    momentum->y += sin(angle) * delta;

    ship->throttle();
}

