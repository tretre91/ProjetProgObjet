#include "Character.h"
#include "Cell.h"
#include "Labyrinthe.h"
#include "Position.h"
#include <cmath>

double Character::distance(double x1, double y1, double x2, double y2) {
	const double dx = x2 - x1;
	const double dy = y2 - y1;
	return std::sqrt(dx * dx + dy * dy);
}

bool Character::move_aux(double dx, double dy) {
	const Position source = Position::grid_position(_x, _y);
	const Position target = Position::grid_position(_x + dx, _y + dy);

	Cell& target_cell = _labyrinth->cell(target.x, target.y);

	if (source != target && !target_cell.is_empty()) {
		return false;
	}

	_x += dx;
	_y += dy;

	if (source != target) {
		Cell& source_cell = _labyrinth->cell(source.x, source.y);

		target_cell._type = source_cell._type;
		target_cell._index = source_cell._index;

		if (source_cell._mark_index != -1) {
			source_cell._type = CellType::mark;
			source_cell._index = source_cell._mark_index;
		} else {
			source_cell._type = CellType::empty;
		}
	}

	return true;
}

bool Character::try_move(double dx, double dy) {
	return move_aux(dx, dy) || move_aux(dx, 0.0) || move_aux(0.0, dy);
}

/**
 * @see https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm#All_cases
 */
bool Character::bresenham_collision(int x1, int y1, int x2, int y2) const {
	const int dx = std::abs(x2 - x1);
	const int step_x = x1 < x2 ? 1 : -1;
	const int dy = -std::abs(y2 - y1);
	const int step_y = y1 < y2 ? 1 : -1;
	int error = dx + dy;
	int e2;

	while (x1 != x2 || y1 != y2) {
		if (!_labyrinth->cell(x1, y1).is_empty()) {
			return true;
		}

		e2 = 2 * error;
		if (e2 >= dy) {
			if (x1 == x2) {
				break;
			}
			error += dy;
			x1 += step_x;
		}

		if (e2 <= dx) {
			if (y1 == y2) {
				break;
			}
			error += dx;
			y1 += step_y;
		}
	}

	return false;
}

bool Character::looks_at(double x, double y, double err) const {
	return std::abs(get_angle() - normalize_angle(rad_to_deg(std::atan2(y - _y, x - _x)))) < err;
}


bool Character::can_see(const Position& target) const {
	auto [x1, y1] = Position::grid_position(_x, _y);
	auto [x2, y2] = target;

	// We temporarily empty the target and source cell just in case x1 and x2 (or y1 and y2) get swapped
	// (as the algorithm would potentially start on an occupied cell and return false immediately)
	Cell& source_cell = _labyrinth->cell(x1, y1);
	const Cell original_source_cell = source_cell;
	source_cell._type = CellType::empty;

	Cell& target_cell = _labyrinth->cell(x2, y2);
	const Cell original_target_cell = target_cell;
	target_cell._type = CellType::empty;

	bool no_obstacles = !bresenham_collision(x1, y1, x2, y2);

	source_cell = original_source_cell;
	target_cell = original_target_cell;

	return no_obstacles;
}
