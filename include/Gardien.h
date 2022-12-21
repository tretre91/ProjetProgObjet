#ifndef GARDIEN_H
#define GARDIEN_H

#include "Character.h"
#include <random>

class Labyrinthe;

class Gardien : public Character
{
private:
    // variables used to generate random angles
    static std::random_device rd;
    static std::mt19937 gen;
    static std::uniform_int_distribution<> random_angle;

    enum class State {
        patrol,
        attack,
        dead
    };

    State m_state = State::patrol;
    double m_speed = 0.08;

    /**
     * @brief Converts an angle from degrees to radians
     */
    double deg_to_rad(int angle) { return angle * (M_PI / 180.0); }

public:
    Gardien(Labyrinthe* l, const char* modele);

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
