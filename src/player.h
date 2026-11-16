#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/input.hpp>

namespace godot {

class Player : public CharacterBody2D {
    GDCLASS(Player, CharacterBody2D)

private:
    double speed = 300.0;
    double jump_velocity = -450.0;
    double gravity = 980.0;

protected:
    static void _bind_methods();

public:
    Player();
    ~Player();

    void _physics_process(double delta) override;

    void set_speed(const double p_speed);
    double get_speed() const;

    void set_jump_velocity(const double p_jump_velocity);
    double get_jump_velocity() const;
};

} // namespace godot

#endif
