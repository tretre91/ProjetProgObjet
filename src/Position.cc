#include "Position.h"
#include "Environnement.h"
#include <algorithm>

int Position::_max_x = 0;
int Position::_max_y = 0;

Position Position::grid_position(double x, double y) {
	// TODO: _max_x - 1 ?
	return Position{
		std::clamp(static_cast<int>(x / Environnement::scale), 0, _max_x),
		std::clamp(static_cast<int>(y / Environnement::scale), 0, _max_y)
	};
}
