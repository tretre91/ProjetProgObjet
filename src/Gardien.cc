#include "Gardien.h"
#include "Character.h"
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

	if (_state == State::dead) {
		return;
	}

	Mover* hunter = _l->_guards[0];
	if (can_see(hunter->_x, hunter->_y)) {
		_state = State::attack;
	} else {
		_state = State::patrol;
	}

	double angle = deg_to_rad(get_angle());

	switch (_state) {
	case State::patrol:
		while (!move_aux(_speed * std::cos(angle), _speed * std::sin(angle))) {
			set_angle(_random_angle(_gen));
			angle = deg_to_rad(get_angle());
		}
		break;
	case State::attack:
		angle = std::atan2(hunter->_y - _y, hunter->_x - _x);
		set_angle(rad_to_deg(angle));
		move(_speed * std::cos(angle), _speed * std::sin(angle));
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
