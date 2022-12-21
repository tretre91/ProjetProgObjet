#include "Chasseur.h"
#include "Labyrinthe.h"
#include <fmt/core.h>

Sound* Chasseur::_hunter_fire; // bruit de l'arme du chasseur.
Sound* Chasseur::_hunter_hit;  // cri du chasseur touché.
Sound* Chasseur::_wall_hit;    // on a tapé un mur.


Chasseur::Chasseur(Labyrinthe* l) : Character(100, 80, l, nullptr) {}


bool Chasseur::process_fireball(float dx, float dy) {
    // calculer la distance entre le chasseur et le lieu de l'explosion.
    float x = (_x - _fb->get_x()) / Environnement::scale;
    float y = (_y - _fb->get_y()) / Environnement::scale;
    float dist2 = x * x + y * y;
    // on bouge que dans le vide!
    if (EMPTY == _l->data((int)((_fb->get_x() + dx) / Environnement::scale), (int)((_fb->get_y() + dy) / Environnement::scale))) {
        message("Woooshh ..... %d", (int)dist2);
        // il y a la place.
        return true;
    }
    // collision...
    // calculer la distance maximum en ligne droite.
    float dmax2 = (_l->width()) * (_l->width()) + (_l->height()) * (_l->height());
    // faire exploser la boule de feu avec un bruit fonction de la distance.
    _wall_hit->play(1. - dist2 / dmax2);
    message("Booom...");
    return false;
}


void Chasseur::fire(int angle_vertical) {
    message("Woooshh...");
    _hunter_fire->play();
    _fb->init(/* position initiale de la boule */ _x, _y, 10.,
      /* angles de vis�e */ angle_vertical, _angle);
}


void Chasseur::right_click(bool shift, bool control) {
    if (shift)
        _l->_guards[1]->rester_au_sol();
    else
        _l->_guards[1]->tomber();
}
