#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"
#include "Exception.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

class Labyrinthe : public Environnement
{
private:
    std::vector<std::vector<char>> m_map;
    int m_width;
    int m_height;

    std::vector<Wall> m_walls;
    std::vector<Wall> m_posters;
    std::vector<Box> m_boxes;
    std::vector<Box> m_marks;
    std::vector<Mover*> m_guards;

    /**
     * @brief Returns the texture id associated to a file.
     */
    int texture_id(const std::string& filename);

    /**
     * @brief Parse the header of a maze file.
     *
     * This function parses the header in order to get the textures used in this file,
     * it returns a map from the chars used to identify a texture to the associated filename.
     *
     * @note This function places the stream in a state such that the next line read will
     *       be the beginning of the labyrinth.
     *
     * @param file A file stream of the file to parse
     * @return A map associating a character to a texture's filename
     */
    std::unordered_map<char, std::string> parse_header(std::ifstream& file);

    /**
     * @brief Parse a maze file and fill the member variables.
     *
     * @param filename The maze file's name
     * @return The lowest x coordinate of the maze
     * @throws ParseError If an error occured when parsing the labyrinth
     */
    int parse(std::ifstream& file);


public:
    Labyrinthe(const char* filename);

    ~Labyrinthe();

    /**
     * @brief Return the labyrinth's width.
     */
    int width() { return m_width; }

    /**
     * @brief Return the labyrinth's height.
     */
    int height() { return m_height; }

    /**
     * @brief Return the cell at position (x, y).
     */
    char data(int x, int y) { return m_map[y][x]; }
};

#endif
