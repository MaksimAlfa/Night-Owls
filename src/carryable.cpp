#include "carryable.h"
#include "godot_cpp/classes/rectangle_shape2d.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void Carryable::_bind_methods() {
    ClassDB::bind_method(D_METHOD("pick_up"), &Carryable::pick_up);
    ClassDB::bind_method(D_METHOD("drop", "p_velocity"), &Carryable::drop);
    ClassDB::bind_method(D_METHOD("get_is_carried"), &Carryable::get_is_carried);
    ClassDB::bind_method(D_METHOD("get_shape"), &Carryable::get_shape);
    ClassDB::bind_method(D_METHOD("get_carry_size"), &Carryable::get_carry_size);
}

Carryable::Carryable() {}
Carryable::~Carryable() {}

void Carryable::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    add_to_group("boxes");

    // Коробка находится на 3-м слое (бит 4), но сталкивается только с 1-м слоем (землей/стенами)
    set_collision_layer(4); // 3-й слой (2^2 = 4)
    set_collision_mask(5);  // 1-й слой (земля)
    set_safe_margin(1.0);

    CollisionShape2D *col = get_node<CollisionShape2D>("CollisionShape2D");
    if (col != nullptr) {

        Ref<RectangleShape2D> rect = col->get_shape();
        if (rect.is_valid()){
            carry_size = rect->get_size();
        }
    }
}


void Carryable::_physics_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Если коробку несут в руках — её физика спит, двигает её Player
    if (is_carried) {
        return;
    }

    Vector2 velocity = get_velocity();

    if (!is_on_floor()) {
        velocity.y += gravity * delta;
    } else {
        if (velocity.y >= 0.0){
            velocity.y = 0.0;
        }
        velocity.x = Math::move_toward((double)velocity.x, 0.0, 600.0 * delta);
    }

    set_velocity(velocity);
    move_and_slide();
}

void Carryable::pick_up() {
    is_carried = true;
    set_velocity(Vector2(0.0, 0.0));
    // Отключаем коллизии, пока несем над головой
    set_collision_layer(0);
    set_collision_mask(0);
}

void Carryable::drop(const Vector2 &p_velocity) {
    is_carried = false;
    // Возвращаем коллизии и задаем стартовую скорость
    set_collision_layer(4);
    set_collision_mask(5);
    set_velocity(p_velocity);
}

bool Carryable::get_is_carried() const {
    return is_carried;
}

Ref<Shape2D> Carryable::get_shape() const {
    CollisionShape2D *col = get_node<CollisionShape2D>("CollisionShape2D");
    if (col != nullptr) {
        return col->get_shape();
    }
    return Ref<Shape2D>();
}

Vector2 Carryable::get_carry_size() const{
    return carry_size;
}
