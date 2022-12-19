#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"

#include <fmt/core.h>
#include <fstream>
#include <limits>
#include <unordered_map>
#include <vector>

Sound* Chasseur::_hunter_fire; // bruit de l'arme du chasseur.
Sound* Chasseur::_hunter_hit;  // cri du chasseur touché.
Sound* Chasseur::_wall_hit;    // on a tapé un mur.

Environnement* Environnement::init(char* filename) {
    return new Labyrinthe(filename);
}


Labyrinthe::Labyrinthe(const char* filename) {
    parse(filename);
    return;
    // TODO: remove below
    // les 4 murs.
    static Wall walls[] = {
      {0, 0, LAB_WIDTH - 1, 0, 0},
      {LAB_WIDTH - 1, 0, LAB_WIDTH - 1, LAB_HEIGHT - 1, 0},
      {LAB_WIDTH - 1, LAB_HEIGHT - 1, 0, LAB_HEIGHT - 1, 0},
      {0, LAB_HEIGHT - 1, 0, 0, 0},
    };
    // une affiche.
    //  (attention: pour des raisons de rapport d'aspect, les affiches doivent faire 2 de long)
    static Wall affiche[] = {
      {4, 0, 6, 0, 0},  // premi�re affiche.
      {8, 0, 10, 0, 0}, // autre affiche.
    };
    // 3 caisses.
    static Box caisses[] = {
      {70, 12, 0},
      {10, 5, 0},
      {65, 22, 0},
    };

    /* DEB - NOUVEAU */
    // 2 marques au sol.
    static Box marques[] = {
      {50, 14, 0},
      {20, 15, 0},
    };
    /* FIN - NOUVEAU */

    // juste un mur autour et les 3 caisses et le tr�sor dedans.
    for (int i = 0; i < LAB_WIDTH; ++i)
        for (int j = 0; j < LAB_HEIGHT; ++j) {
            if (i == 0 || i == LAB_WIDTH - 1 || j == 0 || j == LAB_HEIGHT - 1)
                _data[i][j] = 1;
            else
                _data[i][j] = EMPTY;
        }
    // indiquer qu'on ne marche pas sur une caisse.
    _data[caisses[0]._x][caisses[0]._y] = 1;
    _data[caisses[1]._x][caisses[1]._y] = 1;
    _data[caisses[2]._x][caisses[2]._y] = 1;
    // les 4 murs.
    _nwall = 4;
    _walls = walls;
    // deux affiches.
    _npicts = 2;
    _picts = affiche;
    // la deuxi�me � une texture diff�rente.
    char tmp[128];
    sprintf(tmp, "%s/%s", texture_dir, "voiture.jpg");
    _picts[1]._ntex = wall_texture(tmp);
    // 3 caisses.
    _nboxes = 3;
    _boxes = caisses;

    /* DEB - NOUVEAU */
    // mettre une autre texture � la deuxi�me caisse.
    sprintf(tmp, "%s/%s", texture_dir, "boite.jpg");
    caisses[1]._ntex = wall_texture(tmp);

    // mettre les marques au sol.
    sprintf(tmp, "%s/%s", texture_dir, "p1.gif");
    marques[0]._ntex = wall_texture(tmp);
    sprintf(tmp, "%s/%s", texture_dir, "p3.gif");
    marques[1]._ntex = wall_texture(tmp);
    _nmarks = 2;
    _marks = marques;
    /* FIN - NOUVEAU */

    // le tr�sor.
    _treasor._x = 10;
    _treasor._y = 10;
    _data[_treasor._x][_treasor._y] = 1; // indiquer qu'on ne marche pas sur le tr�sor.
    // le chasseur et les 4 gardiens.
    _nguards = 5;
    _guards = new Mover*[_nguards];
    _guards[0] = new Chasseur(this);
    _guards[1] = new Gardien(this, "Lezard");
    _guards[2] = new Gardien(this, "Blade");
    _guards[2]->_x = 90.;
    _guards[2]->_y = 70.;
    _guards[3] = new Gardien(this, "Serpent");
    _guards[3]->_x = 60.;
    _guards[3]->_y = 10.;
    _guards[4] = new Gardien(this, "Samourai");
    _guards[4]->_x = 130.;
    _guards[4]->_y = 100.;
    // indiquer qu'on ne marche pas sur les gardiens.
    _data[(int)(_guards[1]->_x / scale)][(int)(_guards[1]->_y / scale)] = 1;
    _data[(int)(_guards[2]->_x / scale)][(int)(_guards[2]->_y / scale)] = 1;
    _data[(int)(_guards[3]->_x / scale)][(int)(_guards[3]->_y / scale)] = 1;
    _data[(int)(_guards[4]->_x / scale)][(int)(_guards[4]->_y / scale)] = 1;
}

Labyrinthe::~Labyrinthe() {
    for (Mover* m : m_guards) {
        delete m;
    }
}

void Labyrinthe::parse(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::invalid_argument(fmt::format("ERROR: Labyrinthe, no file named {}", filename));
    }

    std::unordered_map<char, std::string> textures = parse_header(file);

    fmt::print("Parsed the following textures:\n");
    for (auto& [name, val] : textures) {
        fmt::print("- {}: {}\n", name, val);
    }

    // return; // TODO: remove

    const int box_texture = wall_texture(fmt::format("{}/caisse.jpg", texture_dir).data());
    const int mark_texture = wall_texture(fmt::format("{}/p1.gif", texture_dir).data());

    m_guards.push_back(new Chasseur(this));

    std::string line;
    std::unordered_map<int, int> partial_walls;
    int last_horizontal_wall = -1;
    int y = 0;
    int min_x = std::numeric_limits<int>::max();
    int max_x = std::numeric_limits<int>::min();

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
            case 'x':
            case 'X':
                m_boxes.push_back({x, y, box_texture});
                break;
            case 'm':
            case 'M':
                m_marks.push_back({x, y, mark_texture});
                break;
            case 'T': // TODO
                _treasor._x = x;
                _treasor._y = y;
                break;
            case 'C':
                m_guards[0]->_x = x * scale;
                m_guards[0]->_y = y * scale;
                break;
            case 'G':
                {
                    Mover* guardian = new Gardien(this, "Samourai"); // TODO
                    m_guards.push_back(guardian);
                    guardian->_x = x * scale;
                    guardian->_y = y * scale;
                    break;
                }
            case '+': // TODO
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
            default:
                break;
            }
        }
        y++;
    }

    m_width = max_x - min_x + 1;
    m_height = y;

    m_map = std::vector<std::vector<char>>(m_height);
    for (size_t i = 0; i < m_map.size(); i++) {
        m_map[i].resize(m_width);
    }

    for (const Wall& wall : m_walls) {
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


    for (const Box& box : m_boxes) {
        m_map[box._y][box._x] = 1;
    }

    m_map[_treasor._y][_treasor._x] = 1;

    for (size_t i = 1; i < m_guards.size(); i++) {
        m_map[static_cast<int>(m_guards[i]->_y / scale)][static_cast<int>(m_guards[i]->_x / scale)] = 1;
    }

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
                throw std::runtime_error(fmt::format("ERROR: When parsing maze file, unknown character '{}'", c));
            }
            break;
        }
        cur_pos = file.tellg();
    }

    return textures;
}
