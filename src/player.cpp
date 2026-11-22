#include "player.h"
#include "carryable.h"
#include "godot_cpp/classes/collision_shape2d.hpp"
#include "godot_cpp/classes/rectangle_shape2d.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/world2d.hpp>
#include <godot_cpp/classes/physics_direct_space_state2d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters2d.hpp>


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

void Player::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Игрок находится на 2-м слое, а сталкивается с 1-м (пол) и 3-м (коробки) слоями:
    set_collision_layer(2);
    set_collision_mask(5); // 1 (пол) + 4 (коробки) = 5

    CollisionShape2D *player_col = get_node<CollisionShape2D>("CollisionShape2D");

    if (player_col != nullptr) {
    // Приводим общую форму Shape2D к прямоугольнику RectangleShape2D
    Ref<RectangleShape2D> rect = player_col->get_shape();
    if (rect.is_valid()) {
        player_size = rect->get_size(); // Получаем точные ширину (x) и высоту (y)!
    }
    }

    body_check_area = memnew(Area2D);
    body_check_area->set_position(Vector2(0.0, 0.0));
    body_check_area->set_collision_layer(0);
    body_check_area->set_collision_mask(4); //4 слой - коробки, смотрим только их
    add_child(body_check_area);

    CollisionShape2D *body_col = memnew(CollisionShape2D);
    body_col->set_position(Vector2(0.0, 10.0));

    Ref<RectangleShape2D> body_rect;
    body_rect.instantiate();

    body_rect->set_size(Vector2(player_size.x, player_size.y + 20));
    body_col->set_shape(body_rect);
    body_check_area->add_child(body_col);


    // 1. Создаем зону подбора GrabArea и крепим её к Player
    grab_area = memnew(Area2D);
    grab_area->set_collision_layer(0); // Сама зона не является препятствием
    grab_area->set_collision_mask(4);  // Ищет объекты строго на 3-м слое (коробки)
    grab_area->set_position(Vector2(player_size.x/2, 0.0)); // Сдвигаем перед игроком
    add_child(grab_area);

    // 2. Создаем прямоугольный коллайдер (например, ширина 60, высота 80 — достанет от ног до головы)
    CollisionShape2D *grab_col = memnew(CollisionShape2D);
    Ref<RectangleShape2D> rect;
    rect.instantiate();
    rect->set_size(Vector2(player_size.x+10, player_size.y+10));
    grab_col->set_shape(rect);
    grab_area->add_child(grab_col);


    // 3. Создаем точку удержания коробки над головой
    hold_point = memnew(Marker2D);
    hold_point->set_position(Vector2(0.0, -player_size.y/2));
    add_child(hold_point);

}

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

        // 1. Поворачиваем зону подбора GrabArea в сторону взгляда по горизонтали
    if (grab_area != nullptr && facing_direction.x != 0.0) {
        double side = (facing_direction.x > 0.0) ? 1.0 : -1.0;
        grab_area->set_position(Vector2(35.0 * side, 0.0));
    }

    // 2. Обработка нажатия кнопки взаимодействия ("interact")
    if (input->is_action_just_pressed("interact")) {
        if (held_box == nullptr) {
            try_pick_up_box();
        } else {
            drop_box();
        }
    }

    // 3. Если держим коробку — тянем её за точкой HoldPoint над головой
    if (held_box != nullptr && hold_point != nullptr) {
        held_box->set_global_position(hold_point->get_global_position());
    }

    if (drop_through_timer > 0.0) {
    drop_through_timer -= delta;
    }

    if (input->is_action_just_pressed("move_down") && is_on_floor()) {
       drop_through_timer = 0.15; // На 0.15 секунды отключаем опору на коробки
    }

        bool can_stand_on_box = false;

    if (velocity.y >= 0.0 && drop_through_timer <= 0.0 && body_check_area != nullptr) {
        double player_bottom = get_global_position().y + (player_size.y / 2.0);
        TypedArray<Node2D> overlapping = body_check_area->get_overlapping_bodies();
        PhysicsDirectSpaceState2D *space_state = get_world_2d()->get_direct_space_state();

        for (int i = 0; i < overlapping.size(); i++) {
            Carryable *box = Object::cast_to<Carryable>(overlapping[i]);
            if (box != nullptr && !box->get_is_carried()) {
                double box_top = box->get_global_position().y - (box->get_carry_size().y / 2.0);
                double tolerance = 8.0 + (velocity.y * delta);

                if (player_bottom <= box_top + tolerance) {
                    // Проверяем, хватит ли места от крышки коробки вверх до потолка для всего роста игрока!
                    Vector2 check_start = Vector2(get_global_position().x, box_top - 2.0);
                    Vector2 check_end = Vector2(get_global_position().x, box_top - player_size.y);

                    Ref<PhysicsRayQueryParameters2D> ceiling_query = PhysicsRayQueryParameters2D::create(
                        check_start,
                        check_end,
                        1 // Проверяем только 1-й слой (стены и потолок)
                    );

                    Dictionary ceiling_hit = space_state->intersect_ray(ceiling_query);
                    
                    // Разрешаем встать на коробку ТОЛЬКО если луч не уперся в низкий потолок!
                    if (ceiling_hit.is_empty()) {
                        can_stand_on_box = true;
                        break;
                    }
                }
            }
        }
    }

    set_collision_mask_value(3, can_stand_on_box);


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

void Player::try_pick_up_box() {
    if (grab_area == nullptr) {
        return;
    }

    // Получаем все физические тела, попавшие в круг GrabArea
    TypedArray<Node2D> bodies = grab_area->get_overlapping_bodies();
    for (int i = 0; i < bodies.size(); i++) {
        Carryable *box = Object::cast_to<Carryable>(bodies[i]);
        if (box != nullptr && !box->get_is_carried()) {
            held_box = box;
            held_box->pick_up();

            Vector2 box_size = held_box->get_carry_size();
            double hold_y = -player_size.y/2.0 - box_size.y/2.0 - 5;

            if (hold_point != nullptr){
                hold_point->set_position(Vector2(0.0, hold_y));
            }
            break;
        }
    }
}

void Player::drop_box() {
    if (held_box == nullptr) {
        return;
    }

    double side = (facing_direction.x >= 0.0) ? 1.0 : -1.0;
    Vector2 box_size = held_box->get_carry_size();
    double desired_offset_x = ((player_size.x / 2.0) + (box_size.x / 2.0) + 5.0) * side;

    Vector2 start_pos = get_global_position();
    Vector2 target_pos = start_pos + Vector2(desired_offset_x, 0.0);

    // Пускаем луч от центра игрока до края будущей коробки (проверяем только 1-й слой — стены/пол)
    PhysicsDirectSpaceState2D *space_state = get_world_2d()->get_direct_space_state();
    Ref<PhysicsRayQueryParameters2D> query = PhysicsRayQueryParameters2D::create(
        start_pos,
        target_pos + Vector2((box_size.x / 2.0) * side, 0.0),
        1 // Маска 1 — проверяем только стены
    );

    Dictionary result = space_state->intersect_ray(query);
    if (!result.is_empty()) {
        // Если перед нами стена — берем координату удара и отступаем от стены на половину ширины коробки!
        Vector2 hit_pos = result["position"];
        target_pos.x = hit_pos.x - ((box_size.x / 2.0 + 1.0) * side);
    }

    held_box->set_global_position(target_pos);
    held_box->drop(Vector2(0.0, 0.0));
    held_box = nullptr;
}
