#include "Labyrinthe.h"
#include "Cell.h"
#include "Chasseur.h"
#include "DummyMover.h"
#include "Gardien.h"
#include "HUD.h"
#include "Position.h"

#include <cctype>
#include <fmt/core.h>
#include <limits>

Environnement* Environnement::init(char* filename) {
	// initialization of the sound objects
	Chasseur::_hunter_fire = new Sound("sons/hunter_fire.wav"); // bruit de l'arme du chasseur.
	Chasseur::_hunter_hit = new Sound("sons/hunter_hit.wav");   // cri du chasseur touché.
	Chasseur::_wall_hit = new Sound("sons/hit_wall.wav");       // on a tapé un mur.

	// creation of the labyrinth
	texture_dir = "textures/backrooms";
	modele_dir = "modeles";
	Labyrinthe* lab = new Labyrinthe(filename);

	// initialization of the hud
	HUD::init(lab);

	return lab;
}


Labyrinthe::Labyrinthe(const char* filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		throw std::invalid_argument(fmt::format("No file named {}", filename));
	}

	m_guards.push_back(new Chasseur(this));
	m_guards.push_back(new DummyMover(this));

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

	for (Wall& wall : m_walls) {
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

	for (Wall& poster : m_posters) {
		poster._x1 -= min_x;
		poster._x2 -= min_x;
	}

	for (Box& box : m_boxes) {
		box._x -= min_x;
		_map[box._y][box._x]._type = CellType::box;
	}

	for (Box& mark : m_marks) {
		mark._x -= min_x;
		_map[mark._y][mark._x]._type = CellType::mark;
	}

	_treasor._x -= min_x;
	_map[_treasor._y][_treasor._x]._type = CellType::treasure;

	m_guards[0]->_x -= min_x * scale;
	const auto [hunter_x, hunter_y] = Position::grid_position(m_guards[0]->_x, m_guards[0]->_y);
	_map[hunter_y][hunter_x]._type = CellType::hunter;

	for (size_t i = 2; i < m_guards.size(); i++) {
		m_guards[i]->_x -= min_x * scale;
		Position p = Position::grid_position(m_guards[i]->_x, m_guards[i]->_y);
		_map[p.y][p.x]._type = CellType::guard;
		_map[p.y][p.x]._index = i;
	}

	// Set the Environnement member variables
	_walls = m_walls.data();
	_nwall = m_walls.size();
	_picts = m_posters.data();
	_npicts = m_posters.size();
	_boxes = m_boxes.data();
	_nboxes = m_boxes.size();
	_marks = m_marks.data();
	_nmarks = m_marks.size();
	_guards = m_guards.data();
	_nguards = m_guards.size();
}

Labyrinthe::~Labyrinthe() {
	for (Mover* m : m_guards) {
		delete m;
	}

	delete Chasseur::_hunter_fire;
	delete Chasseur::_hunter_hit;
	delete Chasseur::_wall_hit;
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

	const int box_texture = texture_id("caisse.jpg");
	const int mark_texture = texture_id("p1.gif");

	std::string line;
	std::unordered_map<int, int> partial_walls;
	int last_horizontal_wall = -1;
	int y = 0;
	int min_x = std::numeric_limits<int>::max();
	int max_x = std::numeric_limits<int>::min();
	bool hunter_found = false;
	bool treasure_found = false;

	while (std::getline(file, line)) {
		for (int x = 0; x < line.size(); x++) {
			switch (line[x]) {
			case ' ':
				last_horizontal_wall = -1;
				if (partial_walls.find(x) != partial_walls.end()) {
					partial_walls.erase(x);
				}
				break;
			case '-': // TODO: indicate if a maze is ill-formed
			case '|':
				break;
			case 'X':
				m_boxes.push_back({x, y, box_texture});
				break;
			case 'M':
				m_marks.push_back({x, y, mark_texture});
				break;
			case 'T':
				if (treasure_found) {
					throw ParseError(fmt::format("{}:{}, There can only be 1 treasure", y, x));
				}
				treasure_found = true;
				_treasor._x = x;
				_treasor._y = y;
				break;
			case 'C':
				if (hunter_found) {
					throw ParseError(fmt::format("{}:{}, There can only be 1 hunter", y, x));
				}
				hunter_found = true;
				m_guards[0]->_x = x * scale;
				m_guards[0]->_y = y * scale;
				break;
			case 'G':
				{
					Mover* guardian = new Gardien(this, "Carrot"); // TODO "Lezard","Blade","Serpent","Samourai"
					m_guards.push_back(guardian);
					guardian->_x = x * scale;
					guardian->_y = y * scale;
				}
				break;
			case '+':
				if (x < min_x) {
					min_x = x;
				} else if (x > max_x) {
					max_x = x;
				}

				// check if this corresponds to a vertical wall
				if (auto it = partial_walls.find(x); it != partial_walls.end()) {
					m_walls.push_back({x, it->second, x, y, 0});
				}
				partial_walls[x] = y;

				// same for the horizontal wall
				if (last_horizontal_wall != -1) {
					m_walls.push_back({last_horizontal_wall, y, x, y, 0});
				}
				last_horizontal_wall = x;
				break;
			default: // textures and invalid characters
				{
					if (!std::islower(line[x])) {
						throw ParseError(fmt::format("{}:{}, Unknown character '{}'", y, x, line[x]));
					}

					const auto it = textures.find(line[x]);
					if (it == textures.end()) {
						throw ParseError(fmt::format("{}:{}, No texture associated with '{}'", y, x, line[x]));
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
						m_posters.push_back({x, y, x + 2, y, texture_id(it->second)});
					} else {
						m_posters.push_back({x, y, x, y + 2, texture_id(it->second)});
					}
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
