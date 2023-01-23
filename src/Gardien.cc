#include "Gardien.h"
#include "Audio.h"
#include "Cell.h"
#include "Character.h"
#include "Environnement.h"
#include "Util.h"
#include <Labyrinthe.h>
#include <cmath>

// random_angle(gen) returns a random integer between 0 and 359
std::uniform_int_distribution<> Gardien::_random_angle(0, 359);

Gardien::Gardien(Labyrinthe* l, const char* modele) : Gardien(100, 100, l, modele) {}

Gardien::Gardien(int hp, int max_hp, Labyrinthe* l, const char* modele) : Character(120, 80, Util::duration{1000}, hp, max_hp, l, modele) {
	_angle = _random_angle(Util::random_engine);
	_fire_sound = Audio::get("sons/guard_fire.wav");
	_hit_sound = Audio::get("sons/oof.wav");
}

void Gardien::update() {
	// we move the guard off map if it is dead
	if (_state != State::dead && _hp <= 0) {
		_state = State::dead;
		rester_au_sol();
		const auto [x, y] = Position::grid_position(_x, _y);
		dynamic_cast<Labyrinthe*>(_l)->cell(x, y)._type = CellType::empty;
		_x = -2;
		_y = -2;
	}

	if (_state == State::dead) {
		return;
	}

	Mover* hunter = _l->_guards[0];
	if (looks_at(hunter->_x, hunter->_y, 5) && can_see(hunter->_x, hunter->_y)) {
		_state = State::attack;
	} else {
		_state = State::patrol;
	}

	double angle = Util::deg_to_rad(get_angle());

	switch (_state) {
	case State::patrol:
		while (!move_aux(_speed * std::cos(angle), _speed * std::sin(angle))) {
			_angle = _random_angle(Util::random_engine);
			angle = Util::deg_to_rad(get_angle());
		}
		break;
	case State::attack:
		angle = std::atan2(hunter->_y - _y, hunter->_x - _x);
		set_angle(Util::rad_to_deg(angle));
		if (Util::distance(_x, _y, hunter->_x, hunter->_y) > _range) {
			move(_speed * std::cos(angle), _speed * std::sin(angle));
		} else {
			_angle *= -1;
			fire(0);
			_angle *= -1;
		}
		break;
	default:
		break;
	}
}

bool Gardien::move(double dx, double dy) {
	return try_move(dx, dy);
}

bool Gardien::process_fireball(float dx, float dy) {
	const auto [new_x, new_y] = Position::grid_position(_fb->get_x() + dx, _fb->get_y() + dy);
	const Cell target = dynamic_cast<Labyrinthe*>(_l)->cell(new_x, new_y);
	if (target.is_empty() || (target._type == CellType::guard && _l->_guards[target._index] == this)) {
		return true;
	}

	// collision
	if (target._type == CellType::hunter) {
		dynamic_cast<Character*>(_l->_guards[0])->hit(20);
	}

	_wall_hit_sound->play(get_volume(_fb->get_x(), _fb->get_y()));

	_fireball_ready = true;
	return false;
}
