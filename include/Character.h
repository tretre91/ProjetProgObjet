#ifndef LAB_CHARACTER_H
#define LAB_CHARACTER_H

#include "Mover.h"
#include "Position.h"

/**
 * @brief Class implementing a character
 * It has attributes and (helper) methods common to the Chasseur and Gardien
 * classes, which are missing from the Mover class
 */
class Character : public Mover
{
private:
	Labyrinthe* m_labyrinth;

	/**
	 * @brief Tells wheter there is an object (cell != EMPTY) between two positions.
	 * This function uses the Bresenham line drawing algorithm to determine which cells
	 * intersect with the line between (x1,y1) and (x2,y2).
	 * @return true If there is an object between (x1,y1) and (x2,y2), false otherwise
	 */
	bool bresenham_collision(int x1, int y1, int x2, int y2) const;

protected:
	int m_max_hp = 100;
	int m_hp = 100;

	/**
	 * @brief Converts an angle from degrees to radians.
	 */
	static double deg_to_rad(int angle) { return angle * (M_PI / 180.0); }

	/**
	 * @brief Returns the euclidean distance between two points.
	 */
	static double distance(double x1, double y1, double x2, double y2);

	/**
	 * @brief Attemps to move the character.
	 * @return true if the movement is valid
	 */
	bool move_aux(double dx, double dy);

	/**
	 * @brief Attemps to move the character, slides along obstacles.
	 * @return true J'ai aucune idée de ce que signifie la valeur de retour
	 * @return false Si seulement on avait accès aux sources, à défaut d'avoir une bonnne doc ...
	 */
	bool try_move(double dx, double dy);

	/**
	 * @brief Tells wheter this character is looking in the direction of the position x, y
	 * @param err A tolerence parameter, must be positive or zero. Defaults to the global scale.
	 * @return true If this character is looking at the direction (x ± err, y ± err)
	 */
	bool looks_at(double x, double y, double err = Environnement::scale) const;

	/**
	 * @brief Tells wheter this character can see the cell at position (x, y)
	 * @note x and y are given as Mover coordinates
	 * @return true If the character can see at (x, y), that is, if there are no obstacles between
	 *              the character's cell and the target cell.
	 */
	bool can_see(double x, double y) const { return can_see(Position::grid_position(x, y)); }

	/**
	 * @brief Tells wheter this character can see the cell at the target position
	 * @return true If the character can see at target, that is, if there are no obstacles between
	 *              the character's cell and the target cell.
	 */
	bool can_see(const Position& target) const;

public:
	/**
	 * @brief Constructor, forwards the arguments to the Mover constructor.
	 * This constructors initializes a character with 100/100 hp
	 */
	Character(int x, int y, Labyrinthe* l, const char* modele) : Mover(x, y, l, modele), m_labyrinth(l) {}

	/**
	 * @brief Constructs a Character with a certain number (and a certain limit) of health points.
	 */
	Character(int x, int y, int hp, int max_hp, Labyrinthe* l, const char* modele) : Mover(x, y, l, modele), m_labyrinth(l), m_max_hp(max_hp), m_hp(hp) {}

	/**
	 * @brief Returns this character's current amount of health points.
	 */
	int current_hp() const { return m_hp; }

	/**
	 * @brief Returns a reference to this character's hp count.
	 */
	int& current_hp() { return m_hp; }

	void hit(int dmg);

	/**
	 * @brief Returns this character's maximum health.
	 */
	int max_hp() const { return m_max_hp; }
};

#endif
