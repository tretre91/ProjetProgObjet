#ifndef LAB_POSITION_H
#define LAB_POSITION_H

/**
 * @brief Struct representing a position on the labyrinth's grid
 */
struct Position {
private:
    // variables set to the max dimensions of the labyrinth
    // used in the grid_position method
    static int max_x;
    static int max_y;
    friend class Labyrinthe;

public:
    int x = 0;
    int y = 0;

    /**
     * @brief Converts a Mover position to a grid position
     */
    static Position grid_position(double x, double y);

    Position() = default;
    Position(int x, int y) : x(x), y(y) {}
    friend bool operator==(const Position& lhs, const Position& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
};

#endif
