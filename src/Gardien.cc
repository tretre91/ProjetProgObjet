#include "Gardien.h"
#include <cmath>

std::random_device Gardien::rd;
std::mt19937 Gardien::gen(rd());
// random_angle(gen) returns a random integer between 0 and 359
std::uniform_int_distribution<> Gardien::random_angle(0, 359);

Gardien::Gardien(Labyrinthe* l, const char* modele) : Gardien(100, 100, l, modele) {}

Gardien::Gardien(int hp, int max_hp, Labyrinthe* l, const char* modele) : Character(120, 80, hp, max_hp, l, modele) {
    _angle = random_angle(gen);
}

void Gardien::update() {
    double angle = deg_to_rad(_angle);
    double dx;
    double dy;
    switch (m_state) {
    case State::patrol:
        dx = m_speed * std::cos(angle);
        dy = m_speed * std::sin(angle);

        while (!move_aux(dx, dy)) {
            _angle = random_angle(gen);
            angle = deg_to_rad(_angle);
            dx = m_speed * std::cos(angle);
            dy = m_speed * std::sin(angle);
        }
        break;
    case State::attack:
        break;
    default:
        break; // TODO
    }
}

bool Gardien::move(double dx, double dy) {
    return try_move(dx, dy);
}

void Gardien::fire(int angle_vertical) {}

bool Gardien::process_fireball(float dx, float dy) {
    return false;
}
