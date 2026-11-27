#include "hook_point.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void HookPoint::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_hook_radius"), &HookPoint::get_hook_radius);
    ClassDB::bind_method(D_METHOD("set_hook_radius", "p_radius"), &HookPoint::set_hook_radius);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "hook_radius"), "set_hook_radius", "get_hook_radius");
}

HookPoint::HookPoint() {}
HookPoint::~HookPoint() {}

void HookPoint::_ready() {
    // Добавляем точку в группу "hook_points", чтобы Player мог мгновенно найти все точки на уровне
    add_to_group("hook_points");
}

void HookPoint::set_hook_radius(const double p_radius) { hook_radius = p_radius; }
double HookPoint::get_hook_radius() const { return hook_radius; }
