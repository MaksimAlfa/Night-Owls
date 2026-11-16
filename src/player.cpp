#include "player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "p_speed"), &Player::set_speed);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");

    ClassDB::bind_method(D_METHOD("get_jump_velocity"), &Player::get_jump_velocity);
    ClassDB::bind_method(D_METHOD("set_jump_velocity", "p_jump_velocity"), &Player::set_jump_velocity);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "jump_velocity"), "set_jump_velocity", "get_jump_velocity");
}

Player::Player() {}
Player::~Player() {}

void Player::_physics_process(double delta) {
    // Не выполняем логику в окне самого редактора сцен
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Vector2 velocity = get_velocity();

    // 1. Применяем гравитацию, если персонаж в воздухе
    if (!is_on_floor()) {
        velocity.y += gravity * delta;
    }

    Input *input = Input::get_singleton();

    // 2. Прыжок (по умолчанию клавиша Пробел / Enter — действие "ui_accept")
    if (input->is_action_just_pressed("ui_accept") && is_on_floor()) {
        velocity.y = jump_velocity;
    }

    // 3. Движение влево и вправо (по умолчанию стрелки — "ui_left" и "ui_right")
    double direction = input->get_axis("ui_left", "ui_right");
    if (direction != 0.0) {
        velocity.x = direction * speed;
    } else {
        velocity.x = Math::move_toward((double)velocity.x, 0.0, speed);
    }

    set_velocity(velocity);
    move_and_slide();
}

void Player::set_speed(const double p_speed) { speed = p_speed; }
double Player::get_speed() const { return speed; }

void Player::set_jump_velocity(const double p_jump_velocity) { jump_velocity = p_jump_velocity; }
double Player::get_jump_velocity() const { return jump_velocity; }
