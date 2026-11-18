#include "player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void Player::start_dash(const Vector2 &p_dir){
    dash_direction = p_dir.x;
    dash_timer = dash_duration;
    dash_cooldown_timer = dash_cooldown;
    can_dash = false;
}

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "p_speed"), &Player::set_speed);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");

    ClassDB::bind_method(D_METHOD("get_jump_velocity"), &Player::get_jump_velocity);
    ClassDB::bind_method(D_METHOD("set_jump_velocity", "p_jump_velocity"), &Player::set_jump_velocity);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "jump_velocity"), "set_jump_velocity", "get_jump_velocity");

    ClassDB::bind_method(D_METHOD("get_dash_cooldown"), &Player::get_dash_cooldown);
    ClassDB::bind_method(D_METHOD("set_dash_cooldown", "p_dash_cooldown"), &Player::set_dash_cooldown);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "dash_cooldown"), "set_dash_cooldown", "get_dash_cooldown");

    ClassDB::bind_method(D_METHOD("get_gravity"), &Player::get_gravity);
    ClassDB::bind_method(D_METHOD("set_gravity", "p_gravity"), &Player::set_gravity);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity"), "set_gravity", "get_gravity");
}

Player::Player() {}
Player::~Player() {}

void Player::_physics_process(double delta) {
    // Не выполняем логику в окне самого редактора сцен
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Vector2 velocity = get_velocity();

    Input *input = Input::get_singleton();

    Vector2 input_dir = input->get_vector("move_left", "move_right", "move_up", "move_down");
    if (input_dir.length_squared() > 0.0){
        facing_direction = input_dir;
    }

    // 2. Рывок (по умолчанию клавиша Shift — действие "dash")
    if (input->is_action_just_pressed("dash") && can_dash && dash_cooldown_timer <= 0) {
        start_dash(facing_direction);
    }

    if (dash_timer > 0){
        dash_timer -= delta;
        velocity.x = dash_direction * dash_speed;
        velocity.y = 0.0;

        if (dash_timer <= 0){
            velocity *= 0.2; // замедляем скорость
        }
    } else 
    { // если рывка нет, то обычное управление
        if (dash_cooldown_timer > 0){dash_cooldown_timer -= delta;}
        // Гравитация и восстановление рывка
        if (!is_on_floor()) {velocity.y += gravity * delta;} 
        if (is_on_floor()){can_dash = true;}

        // прыжок:
        if (input->is_action_just_pressed("ui_accept") && is_on_floor()) {
            velocity.y = jump_velocity;
        }

        // 3. Движение влево и вправо (по умолчанию стрелки — "ui_left" и "ui_right")
        double direction = input->get_axis("move_left", "move_right");
        if (direction != 0.0) {
            velocity.x = direction * speed;
        } else {
            velocity.x = Math::move_toward((double)velocity.x, 0.0, speed);
        }
    }

    set_velocity(velocity);
    move_and_slide();
}

void Player::set_speed(const double p_speed) { speed = p_speed; }
double Player::get_speed() const { return speed; }

void Player::set_jump_velocity(const double p_jump_velocity) { jump_velocity = p_jump_velocity; }
double Player::get_jump_velocity() const { return jump_velocity; }

void Player::set_dash_cooldown(const double p_dash_cooldown) { dash_cooldown = p_dash_cooldown; }
double Player::get_dash_cooldown() const { return dash_cooldown; }

void Player::set_gravity(const double p_gravity) { gravity = p_gravity;};
double Player::get_gravity() const { return gravity;};
