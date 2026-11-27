#ifndef HOOK_POINT_H
#define HOOK_POINT_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class HookPoint : public Node2D {
    GDCLASS(HookPoint, Node2D)

private:
    double hook_radius = 250.0; // Максимальная дальность зацепа за эту точку

protected:
    static void _bind_methods();

public:
    HookPoint();
    ~HookPoint();

    void _ready() override;

    void set_hook_radius(const double p_radius);
    double get_hook_radius() const;
};

} // namespace godot

#endif
