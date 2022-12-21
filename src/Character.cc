#include "Character.h"
#include "Labyrinthe.h"
#include "Position.h"

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
