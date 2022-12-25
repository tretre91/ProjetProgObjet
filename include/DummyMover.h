#ifndef LAB_DUMMYMOVER_H
#define LAB_DUMMYMOVER_H

#include "Mover.h"
#include <stdexcept>

/**
 * @brief Class implementing a guard which does nothing.
 * This classe's only purpose is to call the hunter's update function.
 * @note Only one instance of this class can be created
 */
class DummyMover : public Mover
{
private:
	inline static bool is_instanciated = false;

public:
	DummyMover(Labyrinthe* l) : Mover(-10, -10, l, "Blade") {
		if (is_instanciated) {
			throw std::runtime_error("Only 1 dummy guard can be instanciated");
		}
		is_instanciated = true;
	}

	void update() override { _l->_guards[0]->update(); }

	bool process_fireball(float, float) override { return false; }
	bool move(double dx, double dy) override { return true; }
	void fire(int angle_vertical) override {}
};

#endif
