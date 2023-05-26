#ifndef HELPER
#define HELPER

#include <tonc.h> // GBA library
#include "sprites.h"

// CALCULATIONS
int get_center(Sprite *sprite);
int get_rel_x_pos(Sprite *sprite);
int get_rel_y_pos(Sprite *sprite);
int get_rel_diag_pos(Sprite *sprite);
int off_screen(Sprite *sprite);
int building_off_screen(Sprite *building);
int collision(Sprite *s1, Sprite *s2, int hbx, int hby);

// SOUNDS
void sound_init();
void note_play(int note, int octave);

// TIMERS
void timer_init();
u32 timer_reset();
u32 timer_update();
int time_change(u32 time_elapsed);

#endif /*HELPER*/