#include "Character.h"
#include "Labyrinthe.h"
#include "Position.h"

double Character::distance(double x1, double y1, double x2, double y2) {
    const double dx = x2 - x1;
    const double dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

bool Character::move_aux(double dx, double dy) {
    const Position source = Position::grid_position(_x, _y);
    const Position target = Position::grid_position(_x + dx, _y + dy);

    char& target_cell = m_labyrinth->mut_data(target.x, target.y);
    if (source == target || EMPTY == target_cell) {
        char& source_cell = m_labyrinth->mut_data(source.x, source.y);
        std::swap(source_cell, target_cell); // TODO
        _x += dx;
        _y += dy;
        return true;
    }
    return false;
}

bool Character::try_move(double dx, double dy) {
    return move_aux(dx, dy) || move_aux(dx, 0.0) || move_aux(0.0, dy);
}

bool Character::bresenham_collision(int x1, int y1, int x2, int y2) const {
    if (x1 == x2) { // vertical line
        if (y1 > y2) {
            std::swap(y1, y2);
        }

        for (; y1 < y2; y1++) {
            if (m_labyrinth->data(x1, y1) != EMPTY) {
                return true;
            }
        }
    } else { // other cases
        if (x1 > x2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        int e = x2 - x1;
        const int dx = 2 * e;
        const int dy = (y2 - y1) * 2;
        while (x1 < x2) {
            if (m_labyrinth->data(x1, y1) != EMPTY) {
                return true;
            }
            x1++;
            e -= dy;
            if (e <= 0) {
                y1++;
                e += dx;
            }
        }
    }
    return false;
}

bool Character::looks_at(double x, double y, double err) const {
    const double dx = x - _x;
    const double dy = y - _y;
    const double distance = std::sqrt(dx * dx + dy * dy);
    const double angle = deg_to_rad(_angle - 90);
    // we invert the sin because the y axis points downwards
    return std::abs(distance * std::cos(angle) - dx) < err && std::abs(distance * -std::sin(angle) - dy) < err;
}


bool Character::can_see(const Position& target) const {
    auto [x1, y1] = Position::grid_position(_x, _y);
    auto [x2, y2] = target;

    // We temporarily empty the target and source cell just in case x1 and x2 (or y1 and y2) get swapped
    // (as the algorithm would potentially start on an occupied cell and return false immediately)
    char& source_cell = m_labyrinth->mut_data(x1, y1);
    const char original_source_cell = source_cell;
    source_cell = EMPTY;

    char& target_cell = m_labyrinth->mut_data(x2, y2);
    const char original_target_cell = target_cell;
    target_cell = EMPTY;

    bool no_objects = !bresenham_collision(x1, y1, x2, y2);

    source_cell = original_source_cell;
    target_cell = original_target_cell;

    return no_objects;
}
