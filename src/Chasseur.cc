#include "Chasseur.h"
#include "Cell.h"
#include "HUD.h"
#include "Labyrinthe.h"
#include "Position.h"

using namespace std::chrono_literals;

Sound* Chasseur::_hunter_fire; // bruit de l'arme du chasseur.
Sound* Chasseur::_hunter_hit;  // cri du chasseur touché.
Sound* Chasseur::_wall_hit;    // on a tapé un mur.


Chasseur::Chasseur(Labyrinthe* l) : Character(100, 80, l, nullptr) {}


void Chasseur::update() {
	HUD::update();
}

bool Chasseur::process_fireball(float dx, float dy) {
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	const float x = (_x - _fb->get_x()) / Environnement::scale;
	const float y = (_y - _fb->get_y()) / Environnement::scale;
	const float dist2 = x * x + y * y;
	HUD::clear();
	// on bouge que dans le vide!
	const auto [new_x, new_y] = Position::grid_position(_fb->get_x() + dx, _fb->get_y() + dy);
	const Cell target = dynamic_cast<Labyrinthe*>(_l)->cell(new_x, new_y);
	if (target.is_empty()) {
		HUD::add_message(fmt::format("Woooshh ..... {}", static_cast<int>(dist2)));
		// il y a la place.
		return true;
	}

	// collision...
	if (target._type == CellType::guard) {
		dynamic_cast<Character*>(_l->_guards[target._index])->hit(20);
	}
	// calculer la distance maximum en ligne droite.
	const float dmax2 = (_l->width()) * (_l->width()) + (_l->height()) * (_l->height());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit->play(1. - dist2 / dmax2);
	HUD::add_message("Booom...", 2s);
	return false;
}


void Chasseur::fire(int angle_vertical) {
	HUD::add_message("Woooshh...");
	_hunter_fire->play();
	_fb->init(/* position initiale de la boule */ _x, _y, 10.,
	  /* angles de vis�e */ angle_vertical, _angle);
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
		dist = distance(_x, _y, x, y);
		if (dist < min_distance && looks_at(x, y, 10) && can_see(x, y)) {
			target = _l->_guards[i];
			min_distance = dist;
		}
	}

	if (target != nullptr) {
		HUD::focus_guard(dynamic_cast<Character*>(target));
	}
}
