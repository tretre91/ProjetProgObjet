#include "Chasseur.h"
#include "Audio.h"
#include "Cell.h"
#include "Environnement.h"
#include "HUD.h"
#include "Labyrinthe.h"
#include "Mark.h"
#include "Position.h"


Chasseur::Chasseur(Labyrinthe* l) : Character(100, 80, Util::milliseconds{250}, 200, 200, l, nullptr) {
	_fire_error_step = 1.;
	_fire_sound = Audio::get("sounds/hunter_fire.wav");
	_hit_sound = Audio::get("sounds/hunter_hit.wav");
	_heal_sound = Audio::get("sounds/heal.wav");
}


void Chasseur::update() {
	if (_hp <= 0) {
		partie_terminee(false);
	}
	HUD::update();
}

bool Chasseur::on_cell_change(Cell& cell) {
	if (cell._type == CellType::treasure) {
		partie_terminee(true);
	} else if (cell._mark_index != -1) {
		Mark* mark = _l->_marks[cell._mark_index];
		switch (mark->type()) {
		case MarkType::heal:
			if (_hp < _max_hp) {
				HealingMark* heal = dynamic_cast<HealingMark*>(mark);
				hit(-(heal->_heal_amount), true);
				Mark* new_mark = new Mark{mark->_x, mark->_y};
				delete heal;
				_l->_marks[cell._mark_index] = new_mark;
				_l->update_mark(cell._mark_index);
			}
			break;
		case MarkType::teleporter:
			{
				TeleporterMark* dest = dynamic_cast<TeleporterMark*>(mark)->_target;
				cell._type = CellType::mark;
				if (dest != nullptr) {
					_x = (dest->_x + 0.5) * Environnement::scale;
					_y = (dest->_y + 0.5) * Environnement::scale;
					const auto [x, y] = Position::grid_position(_x, _y);
					_l->cell(x, y)._type = CellType::hunter;
				}
			}
			break;
		default:
			break;
		}
	}
	return true;
}

bool Chasseur::process_fireball(float dx, float dy) {
	// on bouge que dans le vide!
	const auto [new_x, new_y] = Position::grid_position(_fb->get_x() + dx, _fb->get_y() + dy);
	const Cell target = _l->cell(new_x, new_y);
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
		dist = Util::squared_distance(_x, _y, x, y);
		if (dist < min_distance && looks_at(x, y, 10) && can_see(x, y)) {
			target = _l->_guards[i];
			min_distance = dist;
		}
	}

	if (target != nullptr) {
		HUD::focus_guard(dynamic_cast<Character*>(target));
	}
}
