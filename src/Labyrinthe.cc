#include "Labyrinthe.h"
#include "Cell.h"
#include "Character.h"
#include "Chasseur.h"
#include "DummyMover.h"
#include "Environnement.h"
#include "Gardien.h"
#include "HUD.h"
#include "Mark.h"
#include "Position.h"
#include "Util.h"

#include <algorithm>
#include <cctype>
#include <fmt/core.h>
#include <limits>

Environnement* Environnement::init(char* filename) {
	// creation of the labyrinth
	texture_dir = "textures/backrooms";
	modele_dir = "modeles";
	Labyrinthe* lab = new Labyrinthe(filename);

	// initialization of the character class' static attributes
	Character::init(lab);

	// initialization of the hud
	HUD::init(lab);
	HUD::add_message("Welcome to the backrooms :)", Util::milliseconds{3000});

	return lab;
}


Labyrinthe::Labyrinthe(const char* filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		throw std::invalid_argument(fmt::format("No file named {}", filename));
	}

	// initialization of the Mark default textures
	Mark::_empty_texture = texture_id("empty.gif");
	Mark::_healing_texture = texture_id("heal.gif");
	Mark::_mark_texture = texture_id("p1.gif");

	// parsing of the labyrinth file
	_guards.push_back(new Chasseur(this));
	_guards.push_back(new DummyMover(this));

	fmt::print(stderr, "Parsing {} ...\n", filename);
	int min_x = parse(file);
	fmt::print(stderr, "Done!\n");

	// initialization of the Position class
	Position::_max_x = _width;
	Position::_max_y = _height;

	// Update the map and correct the object's x positions

	_map = std::vector<std::vector<Cell>>(_height);
	for (size_t i = 0; i < _map.size(); i++) {
		_map[i].resize(_width);
	}

	for (Wall& wall : _walls) {
		wall._x1 -= min_x;
		wall._x2 -= min_x;
		if (wall._x1 == wall._x2) {
			for (int y = wall._y1; y <= wall._y2; y++) {
				_map[y][wall._x1]._type = CellType::wall;
			}
		} else {
			for (int x = wall._x1; x <= wall._x2; x++) {
				_map[wall._y1][x]._type = CellType::wall;
			}
		}
	}

	for (Wall& poster : _posters) {
		poster._x1 -= min_x;
		poster._x2 -= min_x;
	}

	for (Box& box : _boxes) {
		box._x -= min_x;
		_map[box._y][box._x]._type = CellType::box;
	}

	for (size_t i = 0; i < _marks.size(); i++) {
		_marks[i]->_x -= min_x;
		_map[_marks[i]->_y][_marks[i]->_x] = Cell(CellType::mark, i);
	}

	_treasor._x -= min_x;
	_map[_treasor._y][_treasor._x]._type = CellType::treasure;

	_guards[0]->_x -= min_x * scale;
	const auto [hunter_x, hunter_y] = Position::grid_position(_guards[0]->_x, _guards[0]->_y);
	_map[hunter_y][hunter_x]._type = CellType::hunter;

	for (size_t i = 2; i < _guards.size(); i++) {
		_guards[i]->_x -= min_x * scale;
		const auto [x, y] = Position::grid_position(_guards[i]->_x, _guards[i]->_y);
		_map[y][x]._type = CellType::guard;
		_map[y][x]._index = i;
	}

	// Set the Environnement member variables
	Environnement::_walls = _walls.data();
	_nwall = _walls.size();
	Environnement::_picts = _posters.data();
	_npicts = _posters.size();
	Environnement::_boxes = _boxes.data();
	_nboxes = _boxes.size();
	Environnement::_marks = new Box[_marks.size()];
	std::transform(_marks.begin(), _marks.end(), Environnement::_marks, [](const Mark* mark) { return *static_cast<const Box*>(mark); });
	Environnement::_nmarks = _marks.size();
	Environnement::_guards = _guards.data();
	_nguards = _guards.size();
}

Labyrinthe::~Labyrinthe() {
	for (Mover* m : _guards) {
		delete m;
	}

	for (Mark* mark : _marks) {
		delete mark;
	}

	delete[] Environnement::_marks;
}

int Labyrinthe::texture_id(const std::string& filename) {
	static std::unordered_map<std::string, int> texture_cache;

	auto it = texture_cache.find(filename);
	if (it != texture_cache.end()) {
		return it->second;
	} else {
		const int id = wall_texture(fmt::format("{}/{}", texture_dir, filename).data());
		if (id == 0) {
			fmt::print(stderr, "WARNING: no texture file named '{}/{}'\n", texture_dir, filename);
		}
		texture_cache[filename] = id;
		return id;
	}
}

void Labyrinthe::update_mark(int index) {
	Environnement::_marks[index] = *static_cast<Box*>(_marks[index]);
	reconfigure();
}

std::unordered_map<char, std::string> Labyrinthe::parse_header(std::ifstream& file) {
	static const auto max_ignored_chars = std::numeric_limits<std::streamsize>::max();
	char c;
	std::string texture_filename;
	std::unordered_map<char, std::string> textures;

	auto cur_pos = file.tellg();
	bool done = false;

	while (!done && file >> c) {
		switch (c) {
		case '#':
			file.ignore(max_ignored_chars, '\n');
			break;
		case '+':
			file.seekg(cur_pos);
			done = true;
			break;
		default:
			if (std::isalpha(c)) { // nom de texture
				file >> texture_filename;
				textures[c] = texture_filename;
				file.ignore(max_ignored_chars, '\n');
			} else {
				throw ParseError(fmt::format("unknown character '{}' in maze header", c));
			}
			break;
		}
		cur_pos = file.tellg();
	}

	return textures;
}


int Labyrinthe::parse(std::ifstream& file) {
	std::unordered_map<char, std::string> textures = parse_header(file);
	std::unordered_map<char, TeleporterMark*> teleporters;
	std::unordered_map<int, int> partial_walls;

	int last_horizontal_wall = -1;

	// helper functions
	// Function used to add a teleporter mark
	auto add_teleporter = [&, this](int x, int y, char id) {
		const auto it = textures.find(id);
		int tex_id = it != textures.end() ? texture_id(it->second) : Mark::_mark_texture;
		if (auto it = teleporters.find(id); it != teleporters.end()) {
			TeleporterMark* mark = new TeleporterMark{x, y, tex_id, nullptr};
			if (it->second->_target == nullptr) {
				it->second->_target = mark;
				mark->_target = it->second;
				_marks.push_back(mark);
			} else {
				delete mark;
				throw ParseError(fmt::format("{}:{}, There are already 2 teleporters of type {}\n", x, y, id));
			}
		} else {
			TeleporterMark* mark = new TeleporterMark{x, y, tex_id, nullptr};
			_marks.push_back(mark);
			teleporters[id] = mark;
		}
	};

	// Function used to add a textured wall
	auto add_poster = [&, this](int x, int y, char id) {
		const auto it = textures.find(id);
		if (it == textures.end()) {
			throw ParseError(fmt::format("{}:{}, No texture associated with '{}'", y, x, id));
		}

		const bool on_horizontal_wall = last_horizontal_wall != -1;
		const bool on_vertical_wall = partial_walls.find(x) != partial_walls.end();
		if (on_horizontal_wall && on_vertical_wall) {
			throw ParseError(fmt::format("{}:{}, A texture cannot be at a wall intersection", y, x));
		}

		if (!on_horizontal_wall && !on_vertical_wall) {
			throw ParseError(fmt::format("{}:{}, A texture must be on a wall", y, x));
		}

		if (on_horizontal_wall) {
			_posters.push_back({x, y, x + 2, y, texture_id(it->second)});
		} else {
			_posters.push_back({x, y, x, y + 2, texture_id(it->second)});
		}
	};

	// Parsing loop
	bool hunter_found = false;
	bool treasure_found = false;
	int y = 0;
	int min_x = std::numeric_limits<int>::max();
	int max_x = std::numeric_limits<int>::min();
	std::string line;

	while (std::getline(file, line)) {
		for (int x = 0; x < static_cast<int>(line.size()); x++) {
			switch (line[x]) {
			case ' ': // emptyy space
				last_horizontal_wall = -1;
				if (partial_walls.find(x) != partial_walls.end()) {
					partial_walls.erase(x);
				}
				break;
			case '-': // TODO: indicate if a maze is ill-formed
			case '|':
				break;
			case 'X': // box
				_boxes.push_back({x, y, texture_id("caisse.jpg")});
				break;
			case 'M': // normal mark
				_marks.push_back(new Mark{x, y, texture_id("p1.gif")});
				break;
			case 'H': // heal mark
				_marks.push_back(new HealingMark{x, y, 20});
				break;
			case 'T': // treasure
				if (treasure_found) {
					throw ParseError(fmt::format("{}:{}, There can only be 1 treasure", y, x));
				}
				treasure_found = true;
				_treasor._x = x;
				_treasor._y = y;
				break;
			case 'C': // hunter
				if (hunter_found) {
					throw ParseError(fmt::format("{}:{}, There can only be 1 hunter", y, x));
				}
				hunter_found = true;
				_guards[0]->_x = x * scale;
				_guards[0]->_y = y * scale;
				break;
			case 'G': // guard
				_guards.push_back(new Gardien(this, "Samourai")); // TODO: use multiple models
				_guards.back()->_x = x * scale;
				_guards.back()->_y = y * scale;
				break;
			case '+': // wall edge
				if (x < min_x) {
					min_x = x;
				} else if (x > max_x) {
					max_x = x;
				}

				// check if this corresponds to a vertical wall
				if (auto it = partial_walls.find(x); it != partial_walls.end()) {
					_walls.push_back({x, it->second, x, y, 0});
				}
				partial_walls[x] = y;

				// same for the horizontal wall
				if (last_horizontal_wall != -1) {
					_walls.push_back({last_horizontal_wall, y, x, y, 0});
				}
				last_horizontal_wall = x;
				break;
			default: // textures, teleporters and invalid characters
				if (std::isdigit(line[x])) { // teleporter
					add_teleporter(x, y, line[x]);
				} else if (std::islower(line[x])) { // texture
					add_poster(x, y, line[x]);
				} else {
					throw ParseError(fmt::format("{}:{}, Unknown character '{}'", y, x, line[x]));
				}
				break;
			}
		}
		y++;
	}

	_width = max_x - min_x + 1;
	_height = y;

	return min_x;
}
