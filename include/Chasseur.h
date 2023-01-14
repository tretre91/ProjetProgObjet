#ifndef CHASSEUR_H
#define CHASSEUR_H

#include "Character.h"
#include "Sound.h"

class Labyrinthe;

class Chasseur : public Character
{
public:
	// The sounds of the hunter
	static Sound* _hunter_fire; // bruit de l'arme du chasseur.
	static Sound* _hunter_hit;  // cri du chasseur touché.
	static Sound* _wall_hit;    // on a tapé un mur.

	/**
	 * @brief Construct a new Chasseur object
	 */
	Chasseur(Labyrinthe* l);

	/**
	 * @brief Attemps to move the character along the x and y axis
	 * See Character::try_move
	 * @return true if the hunter was moved, false otherwise
	 */
	bool move(double dx, double dy) override { return try_move(dx, dy); }

	/**
	 * @brief Function called at each update
	 * Updates the HUD
	 */
	void update() override;

	/**
	 * @brief Moves the fireball along the x and y axis
	 * @return false if the fireball collided with something
	 */
	bool process_fireball(float dx, float dy) override;

	/**
	 * @brief Shoots a fireball
	 * @param angle_vertical The vertical angle at which the fireball will be shot
	 */
	void fire(int angle_vertical) override;

	/**
	 * @brief Get the hunter's angle.
	 * @return The hunter's angle in degrees
	 */
	int get_angle() const override { return normalize_angle(-(_angle - 90)); }

	/**
	 * @brief Function called when the user right-clicks
	 * @param shift Flag indicating if shift was held when clicking
	 * @param control Flag indicating if ctrl was held when clicking
	 */
	void right_click(bool shift, bool control) override;
};

#endif
