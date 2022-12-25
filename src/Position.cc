#include "Position.h"
#include "Environnement.h"
#include <algorithm>

int Position::max_x = 0;
int Position::max_y = 0;

Position Position::grid_position(double x, double y) {
	Position p;
	p.x = std::clamp(static_cast<int>(x / Environnement::scale), 0, max_x);
	p.y = std::clamp(static_cast<int>(y / Environnement::scale), 0, max_y);
	return p;
}
