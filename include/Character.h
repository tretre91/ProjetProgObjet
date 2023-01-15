#ifndef LAB_CHARACTER_H
#define LAB_CHARACTER_H

#include "Audio.h"
#include "Mover.h"
#include "Position.h"
#include <cmath>

/**
 * @brief Class implementing a character
 * It has attributes and (helper) methods common to the Chasseur and Gardien
 * classes, which are missing from the Mover class
 */
class Character : public Mover
{
private:
	Labyrinthe* _labyrinth;
	static double _max_distance;

	/**
	 * @brief Tells wheter there is an object (cell != EMPTY) between two positions.
	 * This function uses the Bresenham line drawing algorithm to determine which cells
	 * intersect with the line between (x1,y1) and (x2,y2).
	 * @return true If there is an object between (x1,y1) and (x2,y2), false otherwise
	 */
	bool bresenham_collision(int x1, int y1, int x2, int y2) const;

protected:
	static Sound* _wall_hit_sound;

	int _max_hp = 100;
	int _hp = 100;
	bool _fireball_ready = true;
	Sound* _fire_sound;
	Sound* _hit_sound;

	// TODO: Move these utility funtions to a dedicated module

	/**
	 * @brief Converts an angle from degrees to radians.
	 */
	static double deg_to_rad(int angle) { return angle * (M_PI / 180.0); }

	/**
	 * @brief Converts an angle from radians to degrees.
	 */
	static int rad_to_deg(double angle) { return std::round(angle * (180.0 / M_PI)); }

	/**
	 * @brief Normalizes an angle in degrees
	 * @return The equivalent angle in the range [0, 360)
	 */
	static int normalize_angle(int angle) { return (angle % 360 + 360) % 360; }

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
	 * @return true if the character moved, false otherwise
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
	 * @brief Initializes the static attributes.
	 * @param l The labyrinth
	 */
	static void init(Labyrinthe* l);

	/**
	 * @brief Get the volume at which a sound should be played given its position.
	 * @return Return a volume which increases as the distance from the hunter decreases
	 */
	float get_volume(double x, double y);

	/**
	 * @brief Constructor, forwards the arguments to the Mover constructor.
	 * This constructors initializes a character with 100/100 hp
	 */
	Character(int x, int y, Labyrinthe* l, const char* modele) : Character(x, y, 100, 100, l, modele) {}

	/**
	 * @brief Constructs a Character with a certain number (and a certain limit) of health points.
	 */
	Character(int x, int y, int hp, int max_hp, Labyrinthe* l, const char* modele) : Mover(x, y, l, modele), _labyrinth(l), _max_hp(max_hp), _hp(hp) {}

	/**
	 * @brief Returns this character's current amount of health points.
	 */
	int hp() const { return _hp; }

	/**
	 * @brief Decreases the hp of this character by a certain amount.
	 * @param dmg The amount of damages this character takes, its hp will be c.hp() - dmg
	 * @param play_soud Tells whether the Character's hit sound should be played
	 */
	void hit(int dmg, bool play_sound = true);

	/**
	 * @brief Returns this character's maximum health.
	 */
	int max_hp() const { return _max_hp; }

	/**
	 * @brief Get the character's angle.
	 * @return The character's angle, in degrees
	 */
	virtual int get_angle() const { return _angle; }

	/**
	 * @brief Set the character's angle.
	 * @param angle The angle in degrees
	 */
	virtual void set_angle(int angle) { _angle = angle; }

	/**
	 * @brief Shoots a fireball
	 * @param vertical_angle The vertical angle at which the fireball will be shot
	 */
	virtual void fire(int vertical_angle) override;
};

#endif
