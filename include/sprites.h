#ifndef SPRITES
#define SPRITES

#include <tonc.h> // GBA library

// max values
#define PLAYER_MAX              1
#define APPLE_MAX               5
#define SQUIRREL_MAX            50
#define BUILDINGS_MAX           16

// tile values
#define PLAYER_TILE_ID          0
#define APPLE_TILE_ID           64
#define SQUIRREL_TITLE_ID	    66
#define BUILDING_TILE_ID        82

// palette values
#define PLAYER_PAL_BANK 		0
#define APPLE_PAL_BANK          1
#define SQUIRREL_PAL_BANK       2
#define BUILDINGS_PAL_BANK 		4

// sprite struct
typedef struct{
	OBJ_ATTR *mem_addr; // pointer to sprite location in object mem
	int hidden; // bool for if the sprite is visible or not
    int active; // if the sprite is an active part of the game (visibility)
	int size; // number of pixels in the horizontal direction
	int start_tile; // starting index in tile memory
	int curr_tile; // current index in tile memory (for animation)
	int pal_bank; // which palette bank
	int dir_facing; // what direction the sprite is facing
	int x_pos; // current x position
	int y_pos; // current y position
	int target; // which sprite are they targeting (for squirrels only)
	int hbx; // hit box x size
	int hby; // hit box y size
	int index; // which sprite occurrence this is (for duplicate sprites)
}Sprite;

extern OBJ_ATTR obj_buffer[128]; // allocate space for 128 sprites
extern OBJ_AFFINE *obj_aff_buffer; // buffer for object affine
extern Sprite *player; // global pointer to the player sprite
extern Sprite* apples[APPLE_MAX]; // array of building sprite pointers
extern Sprite* squirrels[SQUIRREL_MAX]; // array of building sprite pointers
extern Sprite* buildings[BUILDINGS_MAX]; // array of building sprite pointers

extern int round_number; // current round number
extern float player_score; // players score
extern int squirrel_count; // number of squirrels still alive
extern float building_health[8]; // health for each building
extern int minutes;
extern int seconds;

void sprites_init();
void scale_sprite(Sprite *sprite, OBJ_AFFINE *obj_aff_buffer_location, int obj_affine_num, int scale_factor);
void set_player_attributes();
void set_apple_attributes();
void set_squirrel_attributes();
void set_building_attributes();
void finalize_sprite_positions();

#endif /* SPRITES */