/**
 * @file Cell.h
 * This file defines a Cell type used to represent the cells of the labyrinth,
 * as well as an enumeration for the different types of cells.
 */

#ifndef LAB_CELL_H
#define LAB_CELL_H

/**
 * @brief Possible types of cells.
 */
enum class CellType {
	empty,
	wall,
	treasure,
	box,
	mark,
	hunter,
	guard
};

/**
 * @brief Type representing a cell in the labyrinth.
 */
struct Cell {
	CellType _type = CellType::empty; // type of the cell
	int _index = -1;                  // index in the corresponding array (e.g. index in the labyrinth's `_guards` array)
	int _mark_index = -1;             // index in the labyrinth's `_marks` array

	/**
	 * @brief Default constructor, creates an empty cell with its index at -1.
	 */
	Cell() = default;

	/**
	 * @brief Constructs a cell.
	 * @param type The cell's type
	 */
	explicit Cell(CellType type) : _type(type) {}

	/**
	 * @brief Constructs a cell.
	 * @param type The cell's type
	 * @param index The associated index (e.g. the guard's index in the `_guards` list for a guard cell)
	 */
	Cell(CellType type, int index) : _type(type), _index(index) {
		if (type == CellType::mark) {
			_mark_index = _index;
		}
	}

	/**
	 * @brief Tells whether a cell is empty.
	 * @return True if the cell is empty or contains a mark
	 */
	bool is_empty() const { return _type == CellType::empty || _type == CellType::mark; }
};

#endif
