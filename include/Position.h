/**
 * @file Position.h
 * This file defines a simple struct used to represent a position in the labyrinth's
 * grid and a function to convert from Mover coordinates to grid coordinates.
 */

#ifndef LAB_POSITION_H
#define LAB_POSITION_H

/**
 * @brief Struct representing a position on the labyrinth's grid
 */
struct Position {
private:
	// variables set to the max dimensions of the labyrinth
	// used in the grid_position method
	static int _max_x;
	static int _max_y;
	friend class Labyrinthe;

public:
	int x = 0;
	int y = 0;

	/**
	 * @brief Converts a Mover position to a grid position
	 */
	static Position grid_position(double x, double y);

	/**
	 * @brief Default constructor, initializes the position to (0, 0).
	 */
	Position() = default;

	/**
	 * @brief Constructs a position with given x and y coordinates.
	 */
	Position(int x, int y) : x(x), y(y) {}

	// comparison operators
	friend bool operator==(const Position& lhs, const Position& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
	friend bool operator!=(const Position& lhs, const Position& rhs) { return !(lhs == rhs); }
};

#endif
