#include "Position.h"
#include "Environnement.h"
#include <algorithm>

int Position::_max_x = 0;
int Position::_max_y = 0;

Position Position::grid_position(double x, double y) {
	Position p;
	p.x = std::clamp(static_cast<int>(x / Environnement::scale), 0, _max_x);
	p.y = std::clamp(static_cast<int>(y / Environnement::scale), 0, _max_y);
	return p;
}
