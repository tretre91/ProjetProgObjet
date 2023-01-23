#include "Chasseur.h"
#include "Audio.h"
#include "Cell.h"
#include "Environnement.h"
#include "HUD.h"
#include "Labyrinthe.h"
#include "Position.h"


Chasseur::Chasseur(Labyrinthe* l) : Character(100, 80, Util::duration{250}, 100, 100, l, nullptr) {
	_fire_error_step = 2; // TODO: 2 c'est un petit peu violent quand même hein
	_fire_sound = Audio::get("sons/hunter_fire.wav");
	_hit_sound = Audio::get("sons/hunter_hit.wav");
}


void Chasseur::update() {
	if (_hp <= 0) {
		partie_terminee(false);
	}
	HUD::update();
}

bool Chasseur::process_fireball(float dx, float dy) {
	// on bouge que dans le vide!
	const auto [new_x, new_y] = Position::grid_position(_fb->get_x() + dx, _fb->get_y() + dy);
	const Cell target = dynamic_cast<Labyrinthe*>(_l)->cell(new_x, new_y);
	if (target.is_empty()) {
		// il y a la place.
		return true;
	}

	// collision...
	if (target._type == CellType::guard) {
		dynamic_cast<Character*>(_l->_guards[target._index])->hit(20);
	}

	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit_sound->play(get_volume(_fb->get_x(), _fb->get_y()));

	_fireball_ready = true;
	return false;
}

void Chasseur::right_click(bool shift, bool control) {
	if (control) {
		HUD::focus_guard(nullptr);
		return;
	}

	double min_distance = std::numeric_limits<double>::max();
	Mover* target = nullptr;

	double x, y;
	double dist;

	for (int i = 2; i < _l->_nguards; i++) {
		x = _l->_guards[i]->_x;
		y = _l->_guards[i]->_y;
		dist = Util::distance(_x, _y, x, y); // TODO: use the squared distance
		if (dist < min_distance && looks_at(x, y, 10) && can_see(x, y)) {
			target = _l->_guards[i];
			min_distance = dist;
		}
	}

	if (target != nullptr) {
		HUD::focus_guard(dynamic_cast<Character*>(target));
	}
}
