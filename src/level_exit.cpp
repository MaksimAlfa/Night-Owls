#include "level_exit.h"
#include "player.h" // Подключаем Player, чтобы отличать его от коробок
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

using namespace godot;

void LevelExit::_bind_methods() {
    // Регистрируем обработчик сигнала (обязательно, иначе сигнал не сможет его вызвать!)
    ClassDB::bind_method(D_METHOD("_on_body_entered", "body"), &LevelExit::_on_body_entered);

    // Регистрируем свойство выбора файла сцены в Инспекторе
    ClassDB::bind_method(D_METHOD("get_next_scene_path"), &LevelExit::get_next_scene_path);
    ClassDB::bind_method(D_METHOD("set_next_scene_path", "p_path"), &LevelExit::set_next_scene_path);
    
    // PROPERTY_HINT_FILE с фильтром "*.tscn" добавит в Инспектор удобную кнопку выбора сцены!
    ADD_PROPERTY(
        PropertyInfo(Variant::STRING, "next_scene_path", PROPERTY_HINT_FILE, "*.tscn"),
        "set_next_scene_path",
        "get_next_scene_path"
    );
}

LevelExit::LevelExit() {}
LevelExit::~LevelExit() {}

void LevelExit::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Разрешаем зоне видеть 2-й слой (на котором находится Player)
    set_collision_layer(0);
    set_collision_mask(2);

    // Подключаем сигнал "body_entered" к нашей функции "_on_body_entered"
    connect("body_entered", Callable(this, "_on_body_entered"));
}

void LevelExit::_on_body_entered(Node2D *body) {
    // Проверяем, что в дверь зашел именно Player (а не случайно залетевшая коробка)
    Player *player = Object::cast_to<Player>(body);
    if (player != nullptr && !next_scene_path.is_empty()) {
        // Отложенно меняем сцену (call_deferred безопаснее во время расчетов физики)
        get_tree()->call_deferred("change_scene_to_file", next_scene_path);
    }
}

void LevelExit::set_next_scene_path(const String &p_path) { next_scene_path = p_path; }
String LevelExit::get_next_scene_path() const { return next_scene_path; }
