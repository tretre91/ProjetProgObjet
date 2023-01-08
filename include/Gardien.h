#ifndef LAB_GARDIEN_H
#define LAB_GARDIEN_H

#include "Character.h"
#include <random>

class Labyrinthe;

class Gardien : public Character
{
private:
	// variables used to generate random angles
	static std::random_device _rd;
	static std::mt19937 _gen;
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
	double _speed = 0.1;

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
	void update();

	/**
	 * @brief Attemps to move a guard along the x and y axis
	 * - In patrol mode, this method implements a random walk, when a guard
	 * hits a wall, he randomly turns in another direction.
	 * - In attack mode, the guard "looks at" the hunter and moves towards
	 * it.
	 * @return true If the guard has moved
	 */
	bool move(double dx, double dy);

	/**
	 * @brief Method used to make a guard shoot
	 * @note does nothing for the moment
	 * @param vertical_angle The vertical angle at which the fireball will be shot
	 */
	void fire(int angle_vertical);

	/**
	 * @brief Moves the fireball along the x and y axis
	 * @return false if the fireball collided with something
	 */
	bool process_fireball(float dx, float dy);
};

#endif
