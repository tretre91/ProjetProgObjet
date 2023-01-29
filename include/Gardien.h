#ifndef LAB_GARDIEN_H
#define LAB_GARDIEN_H

#include "Character.h"
#include <random>

class Labyrinthe;

class Gardien : public Character
{
private:
	// used to generate random angles
	static std::uniform_int_distribution<> _random_angle;

	/**
	 * @brief Enum for the different states of a guard
	 */
	enum class State {
		patrol,
		attack,
		dead
	};

	State _state = State::patrol;
	double _speed = 0.2;
	double _range = 100.;

public:
	/**
	 * @brief Constructor.
	 */
	Gardien(Labyrinthe* l, const char* modele);

	/**
	 * @brief Constructs a guard with a certain amount of hp.
	 * @param hp The starting hp
	 * @param max_hp The hp limit
	 */
	Gardien(int hp, int max_hp, Labyrinthe* l, const char* modele);

	/**
	 * @brief Function called at each update
	 * Moves the guard and checks for a change of state
	 */
	void update() override;

	/**
	 * @brief Attemps to move a guard along the x and y axis
	 * - In patrol mode, this method implements a random walk, when a guard
	 * hits a wall, he randomly turns in another direction.
	 * - In attack mode, the guard "looks at" the hunter and moves towards
	 * it.
	 * @return true If the guard has moved
	 */
	bool move(double dx, double dy) override;

	/**
	 * @brief Fucntion called when the guard moves to a different cell.
	 * @param cell The target cell
	 * @return false if the cell is a teleporter
	 */
	bool on_cell_change(Cell& cell) override;

	/**
	 * @brief Moves the fireball along the x and y axis
	 * @return false if the fireball collided with something
	 */
	bool process_fireball(float dx, float dy) override;

	/**
	 * @brief Get this guard's angle in degrees.
	 * @return The guards angle in my coordinate system (0° on the x+ axis, counter-clockwise rotation)
	 */
	int get_angle() const override { return Util::normalize_angle(_angle + 90); }

	/**
	 * @brief Set this guard's angle.
	 * @param angle An angle in degrees in my coordinate system (0° on the x+ axis, counter-clockwise rotation)
	 */
	void set_angle(int angle) override { _angle = Util::normalize_angle(angle - 90); }
};

#endif
