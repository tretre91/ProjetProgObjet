#include "Character.h"
#include "Audio.h"
#include "Cell.h"
#include "Environnement.h"
#include "Labyrinthe.h"
#include "Position.h"
#include "Util.h"
#include "fmt/core.h"
#include <cmath>
#include <random>

Sound* Character::_wall_hit_sound = nullptr;
double Character::_max_distance;

void Character::init(Labyrinthe* l) {
	_wall_hit_sound = Audio::get("sounds/hit_wall.wav");
	const double width = l->width() * Environnement::scale;
	const double height = l->height() * Environnement::scale;
	_max_distance = width * width + height * height;
}

float Character::get_volume(double x, double y) {
	const double dx = _l->_guards[0]->_x - x;
	const double dy = _l->_guards[0]->_y - y;
	return 1. - (dx * dx + dy * dy) / _max_distance;
}

bool Character::move_aux(double dx, double dy) {
	const Position source = Position::grid_position(_x, _y);
	const Position target = Position::grid_position(_x + dx, _y + dy);

	Cell& target_cell = _l->cell(target.x, target.y);

	if (source != target) {
		if (!(on_cell_change(target_cell) && target_cell.is_empty())) {
			return false;
		}

		Cell& source_cell = _l->cell(source.x, source.y);

		target_cell._type = source_cell._type;
		target_cell._index = source_cell._index;

		if (source_cell._mark_index != -1) {
			source_cell._type = CellType::mark;
			source_cell._index = source_cell._mark_index;
		} else {
			source_cell._type = CellType::empty;
		}
	}

	_x += dx;
	_y += dy;

	return true;
}

bool Character::try_move(double dx, double dy) {
	return move_aux(dx, dy) || move_aux(dx, 0.0) || move_aux(0.0, dy);
}

void Character::hit(int dmg, bool play_sound) {
	_hp -= dmg;
	if (_hp > _max_hp) {
		_hp = _max_hp;
	}

	int err = static_cast<int>((static_cast<double>(_max_hp - _hp) / _max_hp) * _fire_error_step * 5.);
	_fire_angle_error = std::uniform_int_distribution<>{-err, err};

	if (play_sound) {
		if (dmg > 0) {
			_hit_sound->play(get_volume(_x, _y));
		} else if (dmg < 0) {
			_heal_sound->play(get_volume(_x, _y));
		}
	}
}

void Character::fire(int angle_vertical) {
	if (_fireball_ready && Util::clock::now() >= _last_fireball_time + _fireball_cooldown) {
		const int vertical_error = _fire_angle_error(Util::random_engine) / 2;
		const int horizontal_error = _fire_angle_error(Util::random_engine);
		_fire_sound->play(get_volume(_x, _y));
		_fb->init(_x, _y, 10., angle_vertical + vertical_error, _angle + horizontal_error);
		_fireball_ready = false;
		_last_fireball_time = Util::clock::now();
	}
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
		if (!_l->cell(x1, y1).is_empty()) {
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
	const int current_angle = get_angle();
	const int computed_angle = Util::normalize_angle(Util::rad_to_deg(std::atan2(y - _y, x - _x)));
	const int difference = Util::normalize_angle((current_angle - computed_angle) + 180) - 180;
	return std::abs(difference) < err;
}


bool Character::can_see(const Position& target) const {
	auto [x1, y1] = Position::grid_position(_x, _y);
	auto [x2, y2] = target;

	// We temporarily empty the target and source cell just in case x1 and x2 (or y1 and y2) get swapped
	// (as the algorithm would potentially start on an occupied cell and return false immediately)
	Cell& source_cell = _l->cell(x1, y1);
	const Cell original_source_cell = source_cell;
	source_cell._type = CellType::empty;

	Cell& target_cell = _l->cell(x2, y2);
	const Cell original_target_cell = target_cell;
	target_cell._type = CellType::empty;

	bool no_obstacles = !bresenham_collision(x1, y1, x2, y2);

	source_cell = original_source_cell;
	target_cell = original_target_cell;

	return no_obstacles;
}
