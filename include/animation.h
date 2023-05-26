#ifndef ANIMATION
#define ANIMATION

// TRAVEL SPEED
#define PLAYER_SPEED 		5
#define APPLE_SPEED 		7
#define SQUIRREL_SPEED 		1

// MAP BORDERS
#define MAP_MAX_X 270
#define MAP_MAX_Y 260
#define PLAYER_MAX_X 208
#define PLAYER_MIN_X 0
#define PLAYER_MAX_Y 128
#define PLAYER_MIN_Y 0

extern int skid; // bool for if a player skid occurred
extern int showing_map; // bool for if map is showing
extern int throwing; // bool for if player is throwing

void input();
void finalize_sprite_positions();
void update_sprite_positions();
void check_sprite_collisions();


#endif /*ANIMATION*/