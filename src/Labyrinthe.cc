#include "Labyrinthe.h"
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
	texture_dir = "new_textures";
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

	fmt::print(stderr, "Parsing {} ...\n", filename);
	int min_x = parse(file);
	fmt::print(stderr, "Done!\n");

	// initialization of the Position class
	Position::max_x = m_width;
	Position::max_y = m_height;

	// Update the map, correct the object's x positions and flip them on the y axis

	m_map = std::vector<std::vector<char>>(m_height);
	for (size_t i = 0; i < m_map.size(); i++) {
		m_map[i].resize(m_width, EMPTY);
	}

	const int max_y = m_height - 1;
	int tmp;

	for (Wall& wall : m_walls) {
		wall._x1 -= min_x;
		wall._x2 -= min_x;
		tmp = wall._y1;
		wall._y1 = max_y - wall._y2;
		wall._y2 = max_y - tmp;
		if (wall._x1 == wall._x2) {
			for (int y = wall._y1; y <= wall._y2; y++) {
				m_map[y][wall._x1] = 1;
			}
		} else {
			for (int x = wall._x1; x <= wall._x2; x++) {
				m_map[wall._y1][x] = 1;
			}
		}
	}

	for (Wall& poster : m_posters) {
		poster._x1 -= min_x;
		poster._x2 -= min_x;
		tmp = poster._y1;
		poster._y1 = max_y - poster._y2;
		poster._y2 = max_y - tmp;
	}

	for (Box& box : m_boxes) {
		box._x -= min_x;
		box._y = max_y - box._y;
		m_map[box._y][box._x] = 1;
	}

	for (Box& mark : m_marks) {
		mark._x -= min_x;
		mark._y = max_y - mark._y;
	}

	_treasor._x -= min_x;
	_treasor._y = max_y - _treasor._y;
	m_map[_treasor._y][_treasor._x] = 1;

	m_guards[0]->_x -= min_x * scale;
	m_guards[0]->_y = (max_y)*scale - m_guards[0]->_y;
	for (size_t i = 1; i < m_guards.size(); i++) {
		m_guards[i]->_x -= min_x * scale;
		m_guards[i]->_y = (max_y)*scale - m_guards[i]->_y;
		Position p = Position::grid_position(m_guards[i]->_x, m_guards[i]->_y);
		m_map[p.y][p.x] = 1;
	}

	m_guards.push_back(new DummyMover(this));


	// Set the Environment member variables
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
					Mover* guardian = new Gardien(this, "Samourai"); // TODO "Lezard","Blade","Serpent","Samourai"
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

	m_width = max_x - min_x + 1;
	m_height = y;

	return min_x;
}
