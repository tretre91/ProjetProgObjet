#ifndef LAB_CHARACTER_H
#define LAB_CHARACTER_H

#include "Mover.h"

/**
 * @brief Class implementing a character
 * It has attributes and (helper) methods common to the Chasseur and Gardien
 * classes, which are missing from the Mover class
 */
class Character : public Mover
{
private:
    Labyrinthe* m_labyrinth;

protected:
    /**
     * @brief Attemps to move the character
     * @return true if the movement is valid
     */
    bool move_aux(double dx, double dy);

    /**
     * @brief Attemps to move the character, slides along obstacles
     * @return true J'ai aucune idée de ce que signifie la valeur de retour
     * @return false Si seulement on avait accès aux sources, à défaut d'avoir une bonnne doc ...
     */
    bool try_move(double dx, double dy);

public:
    /**
     * @brief Constructor, forwards the arguments to the Mover constructor
     */
    Character(int x, int y, Labyrinthe* l, const char* modele) : m_labyrinth(l), Mover(x, y, l, modele) {}
};

#endif
