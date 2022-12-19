#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"
#include <string>
#include <unordered_map>
#include <vector>

#define LAB_WIDTH 80
#define LAB_HEIGHT 25

class Labyrinthe : public Environnement
{
private:
    char _data[LAB_WIDTH][LAB_HEIGHT];
    std::vector<std::vector<char>> m_map;
    int m_width;
    int m_height;

    std::vector<Wall> m_walls;
    std::vector<Wall> m_posters;
    std::vector<Box> m_boxes;
    std::vector<Box> m_marks;
    std::vector<Mover*> m_guards;

    /**
     * @brief Parse a maze file and fill the member variables
     *
     * @param filename The maze file's name
     */
    void parse(const char* filename);

    std::unordered_map<char, std::string> parse_header(std::ifstream& file);

public:
    Labyrinthe(const char* filename);
	
    ~Labyrinthe();

    int width() { return m_width; } // retourne la largeur du labyrinthe.

    int height() { return m_height; } // retourne la longueur du labyrinthe.

    char data(int i, int j) {
        return m_map[j][i];
    } // retourne la case (i, j).
};

#endif
