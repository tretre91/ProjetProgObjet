#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Cell.h"
#include "Environnement.h"
#include "Exception.h"
#include "Mark.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

class Labyrinthe : public Environnement
{
private:
	std::vector<std::vector<Cell>> _map;
	int _width;
	int _height;

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
	 * @return A map associating a character to a texture file
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
	std::vector<Wall> _walls;
	std::vector<Wall> _posters;
	std::vector<Mark*> _marks;
	std::vector<Box> _boxes;
	std::vector<Mover*> _guards;

	Labyrinthe(const char* filename);

	~Labyrinthe();

	/**
	 * @brief Return the labyrinth's width.
	 */
	int width() { return _width; }

	/**
	 * @brief Return the labyrinth's height.
	 */
	int height() { return _height; }

	/**
	 * @brief Returns 0 if the cell at (x,y) is empty, 1 otherwise
	 */
	char data(int x, int y) { return !_map[y][x].is_empty(); }

	/**
	 * @brief Return the cell at position (x, y).
	 */
	Cell cell(int x, int y) const { return _map[y][x]; }

	/**
	 * @brief Return a reference to the cell at position (x, y)
	 */
	Cell& cell(int x, int y) { return _map[y][x]; }

	/**
	 * @brief Update a mark and reconfigure the labyrinth.
	 * @param i The mark's index
	 */
	void update_mark(int i);
};

#endif
