#include "Gardien.h"
#include <Labyrinthe.h>
#include <cmath>

std::random_device Gardien::_rd;
std::mt19937 Gardien::_gen(_rd());
// random_angle(gen) returns a random integer between 0 and 359
std::uniform_int_distribution<> Gardien::_random_angle(0, 359);

Gardien::Gardien(Labyrinthe* l, const char* modele) : Gardien(100, 100, l, modele) {}

Gardien::Gardien(int hp, int max_hp, Labyrinthe* l, const char* modele) : Character(120, 80, hp, max_hp, l, modele) {
	_angle = _random_angle(_gen);
}

void Gardien::update() {
	// we move the guard off map if he is dead
	if (_state != State::dead && _hp <= 0) {
		_state = State::dead;
		rester_au_sol();
		auto [x, y] = Position::grid_position(_x, _y);
		dynamic_cast<Labyrinthe*>(_l)->mut_data(x, y) = EMPTY;
		_x = -1;
		_y = -1;
	}

	double angle = deg_to_rad(_angle);
	double dx;
	double dy;
	switch (_state) {
	case State::patrol:
		dx = _speed * std::cos(angle);
		dy = _speed * std::sin(angle);

		while (!move_aux(dx, dy)) {
			_angle = _random_angle(_gen);
			angle = deg_to_rad(_angle);
			dx = _speed * std::cos(angle);
			dy = _speed * std::sin(angle);
		}
		break;
	case State::attack:
		break;
	default:
		break;
	}
}

bool Gardien::move(double dx, double dy) {
	return try_move(dx, dy);
}

void Gardien::fire(int angle_vertical) {}

bool Gardien::process_fireball(float dx, float dy) {
	return false;
}
