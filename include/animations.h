#include <tonc.h>
#include "sprites.h"
#include "background.h"

void move_sprite(Sprite *sprite, int x_dir, int y_dir, int speed);
void walk_animation(Sprite *sprite, int end_frame);
void turn_animation(Sprite *sprite);
void player_skid_animation(Sprite *player);
void squirrel_kill_animation(Sprite *squirrel);
void apple_kill_animation(Sprite *apple);
void spawn_squirrel(int index, Sprite** squirrels);
void apple_throw_init(Sprite *apple, Sprite *player);
void move_towards(Sprite *sprite, Sprite *destination_sprite);