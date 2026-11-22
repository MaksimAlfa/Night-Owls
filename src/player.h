#ifndef PLAYER_H
#define PLAYER_H

#include "godot_cpp/variant/vector2.hpp"
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/rectangle_shape2d.hpp>
#include <godot_cpp/classes/marker2d.hpp>

#include "carryable.h"

namespace godot {

class Player : public CharacterBody2D {
    GDCLASS(Player, CharacterBody2D)

private:
    Vector2 player_size = Vector2(64.0, 64.0);

    double speed = 500.0;
    double jump_velocity = -550.0;
    double gravity = 1200.0;

    double dash_speed = speed * 2;
    double dash_timer = 0.0;
    double dash_duration = 0.15; //длительность рывка

    double dash_cooldown = 0.1;
    double dash_cooldown_timer = 0.0;
    bool can_dash = true;

    Vector2 facing_direction = Vector2(1.0, 0.0); // Куда смотрит игрок (изначально вправо)
    double dash_direction = 1.0; // Куда смотрит игрок во время старта рывка

    void start_dash(const Vector2 &dir);

    Area2D *body_check_area = nullptr;

    double drop_through_timer = 0.0;

    Area2D *grab_area = nullptr;
    Marker2D *hold_point = nullptr;

    Carryable *held_box = nullptr;

    void try_pick_up_box();
    void drop_box();

protected:
    static void _bind_methods();

public:
    Player();
    ~Player();

    void _ready() override;
    void _physics_process(double delta) override;

    void set_speed(const double p_speed);
    double get_speed() const;

    void set_jump_velocity(const double p_jump_velocity);
    double get_jump_velocity() const;

    void set_dash_cooldown(const double p_dash_cooldown);
    double get_dash_cooldown() const;

    void set_gravity(const double p_gravity);
    double get_gravity() const;
};

} // namespace godot

#endif
