A player hunting a treasure and fighting guards in  maze

## Important

> les fichiers Environnement.h, Mover.h, FireBall.h, Sound.h ne doivent PAS être modifiés!
> Gardien.h, Chasseur.h, Chasseur.cpp, Labyrinthe.h et Labyrinthe.cpp qui sont des exemples et que vous devez réécrire en vous inspirant de ceux-là!
> Les répertoires sons, textures et modeles contiennent respectivement les sons (fichiers .wav ou .mp3) les images des murs, affiches etc. (fichiers .jpg ou .gif), et les modèles de gardiens (fichiers .md2 et .jpg).

> `void partie_terminee (bool)`
> Appeler cette fonction avec 'true' si le joueur a gagne et 'false' s'il a perdu. Dans les deux cas cela invalidera la possibilité de tir pour le chasseur et cela affichera le message 'You Win!' ou 'You Lose!!' en gros sur la fenêtre. 

> `void message (char* format, ...)`
> permet d'afficher un message en haut à gauche de la fenêtre. Cette fonction est du type 'printf', à nombre variable d'arguments de types quelconques. 

> `void Sound::play (float volume, float pan)`
> 'volume' est un nombre entre 0 et 1 (plus c'est grand, plus c'est sonore) 'pan' est nombre entre 0 (gauche) et 1 (droite). 

## Player pov spec

### Guards

2 states:
- Patrol (randow walk)
  A guard has an angle, his angle changes randomly when he hits an obstacle
- Attack (shoots you, goes in your direction), + bruit d'alarme incendie quand il voit le joueur
- Inflate the textures

A guard goes in attack mode when he sees the player
A guard cannot see beyond a box
When a guard dies, we can make him disappear, or change its (visual) state and keep him on the map

### Hunter

- Can see beyond a box
- Cannot shoot beyond a box
- Can put marks on the floor, then anything that walks on one is randomly TPed to another

### Projectiles

Bullets are twice as fast as the characters

### Obstacles

Boxes
Can be moved by walking into them
Could be destructible ?

### Bonus

Destructible environment (walls, boxes)

## Dev spec

### Maze file format

```
a poster.png
b    image.jpg
       +---+
+------+ T +-----+
|  G        G    |
|                a
|  M             |
+---------+      |
          |      |
		  | X  M |
		  |      |
		  |   C  |
		  +------+
```


### Hitboxes

Use a grid, two objects cannot be on the same cell

### Main

- a user function called at initialization
- a render function called in a loop, each 10ms?

### Classes



