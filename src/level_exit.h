#ifndef LEVEL_EXIT_H
#define LEVEL_EXIT_H

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class LevelExit : public Area2D {
    GDCLASS(LevelExit, Area2D)

private:
    String next_scene_path = ""; // Путь к следующей сцене (например, "res://levels/level_2.tscn")

protected:
    static void _bind_methods();

public:
    LevelExit();
    ~LevelExit();

    void _ready() override;

    // Обработчик сигнала входа тела в зону
    void _on_body_entered(Node2D *body);

    // Геттер и сеттер для Инспектора
    void set_next_scene_path(const String &p_path);
    String get_next_scene_path() const;
};

} // namespace godot

#endif
