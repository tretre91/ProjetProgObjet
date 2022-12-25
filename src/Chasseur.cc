#include "Chasseur.h"
#include "HUD.h"
#include "Labyrinthe.h"

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
	float x = (_x - _fb->get_x()) / Environnement::scale;
	float y = (_y - _fb->get_y()) / Environnement::scale;
	float dist2 = x * x + y * y;
	HUD::clear();
	// on bouge que dans le vide!
	if (EMPTY == _l->data((int)((_fb->get_x() + dx) / Environnement::scale), (int)((_fb->get_y() + dy) / Environnement::scale))) {
		HUD::add_message(fmt::format("Woooshh ..... {}", static_cast<int>(dist2)));
		// il y a la place.
		return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float dmax2 = (_l->width()) * (_l->width()) + (_l->height()) * (_l->height());
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

	for (int i = 1; i < _l->_nguards - 1; i++) {
		x = _l->_guards[i]->_x;
		y = _l->_guards[i]->_y;
		dist = distance(_x, _y, x, y);
		if (dist < min_distance && looks_at(x, y) && can_see(x, y)) {
			target = _l->_guards[i];
			min_distance = dist;
		}
	}

	if (target != nullptr) {
		HUD::focus_guard(dynamic_cast<Character*>(target));
	}
}
