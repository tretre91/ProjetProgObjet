#ifndef LAB_CELL_H
#define LAB_CELL_H

/**
 * @brief Possible types of cells.
 */
enum class CellType {
	empty = 0,
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
	CellType _type = CellType::empty;
	int _index = -1;

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
	Cell(CellType type, int index) : _type(type), _index(index) {}

	/**
	 * @brief Tells whether a cell is empty.
	 * @return True if the cell is empty or contains a mark
	 */
	bool is_empty() const { return _type == CellType::empty || _type == CellType::mark; }
};

#endif
