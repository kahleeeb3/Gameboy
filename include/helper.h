#include "sprites.h"

#ifndef HELPER
#define HELPER
int get_center(Sprite *sprite);
int get_rel_x_pos(Sprite *sprite);
int get_rel_y_pos(Sprite *sprite);
int get_rel_diag_pos(Sprite *sprite);
int collision(Sprite *s1, Sprite *s2, int hb);
int off_screen(Sprite *sprite);
void text_init();
int building_off_screen(Sprite *building);
void sound_init();
void window_init();
void note_play(int note, int octave);
#endif