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
#include <godot_cpp/classes/line2d.hpp>
#include <godot_cpp/classes/camera2d.hpp>

#include "carryable.h"
#include "hook_point.h"

namespace godot {

class Player : public CharacterBody2D {
    GDCLASS(Player, CharacterBody2D)

private:
    Vector2 player_size = Vector2(64.0, 64.0);

    double air_acceleration = 1000.0; // Ускорение подруливания в воздухе (пикс/сек²)
    double air_drag = 200.0;         // Легкое сопротивление воздуха, если кнопки не нажаты

    double speed = 500.0;
    double jump_velocity = -550.0;
    double gravity = 1200.0;

    double dash_speed = speed * 4;
    double dash_timer = 0.0;
    double dash_duration = 0.15; //длительность рывка

    double dash_cooldown = 0.1;
    double dash_cooldown_timer = 0.0;
    bool can_dash = true;

    Vector2 facing_direction = Vector2(1.0, 0.0); // Куда смотрит игрок (изначально вправо)
    double dash_direction = 1.0; // Куда смотрит игрок во время старта рывка

    void start_dash(const Vector2 &dir);

    double drop_through_timer = 0.0;

    Area2D *grab_area = nullptr;
    Marker2D *hold_point = nullptr;

    Carryable *held_box = nullptr;

    void try_pick_up_box();
    void drop_box();

    HookPoint *active_hook = nullptr;
    Line2D *rope_line = nullptr;
    double rope_length = 0.0;
    double swing_force = 600.0; // Сила раскачивания влево-вправо на веревке

    void try_attach_hook();
    void release_hook();

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
