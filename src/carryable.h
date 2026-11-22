#ifndef CARRYABLE_H
#define CARRYABLE_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/shape2d.hpp>
#include <godot_cpp/variant/vector2.hpp>

namespace godot {

class Carryable : public CharacterBody2D {
    GDCLASS(Carryable, CharacterBody2D)

private:
    Vector2 carry_size = Vector2(64.0, 64.0);

    bool is_carried = false;
    double gravity = 1200.0;

protected:
    static void _bind_methods();

public:

    void _ready() override;
    Carryable();
    ~Carryable();

    void _physics_process(double delta) override;

    void pick_up();
    void drop(const Vector2 &p_velocity);

    bool get_is_carried() const;
    Ref<Shape2D> get_shape() const;

    Vector2 get_carry_size() const;
};

} // namespace godot

#endif
