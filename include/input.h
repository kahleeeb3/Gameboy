#include <tonc.h>

// Player constants
#define PLAYER_SPEED 			5
#define PLAYER_SIZE 			32
#define PLAYER_PAL_OFFSET 		0
#define PLAYER_PAL_BANK 		0
#define PLAYER_HIT_BOX			8
#define PLAYER_FRAME1 			0
#define PLAYER_FRAME2 			16
#define PLAYER_FRAME3 			32
#define PLAYER_FRAME4 			48

// Apple constants
#define APPLE_MAX 				5
#define APPLE_SPEED 			7
#define APPLE_SIZE 				8
#define APPLE_PAL_OFFSET 		16
#define APPLE_PAL_BANK			1
#define APPLE_FRAME1 			64

// Squirrel constants
#define SQUIRREL_MAX 			5
#define SQUIRREL_SPEED 			1
#define SQUIRREL_SIZE 			16
#define SQUIRREL_PAL_OFFSET 	32
#define SQUIRREL_PAL_BANK 		2
#define SQUIRREL_HIT_BOX 		4
#define SQUIRREL_FRAME1 		66
#define SQUIRREL_FRAME2 		70
#define SQUIRREL_FRAME3 		74
#define SQUIRREL_FRAME4 		78


typedef struct{
	OBJ_ATTR *mem_addr; // pointer to location in object mem
	int throwing; // bool value for if the player sprite threw an apple
	int size; // number of pixels in the horizontal direction
    int active; // if the sprite is an active part of the game (visibility)
	int start_tile; // tile number of the first tile
	int pal_bank; // which palette bank
	int curr_frame; // which animation frame the sprite is currently on
	int dir_facing; // what direction the sprite is facing
	int x_pos; // current x position
	int y_pos; // current y position
	int index; // which sprite occurrence this is (for duplicate sprites)
}Sprite;

void walk_animation(Sprite *sprite, int end_frame);
int move_player(Sprite *player);
void apple_throw_init(Sprite *apple, Sprite *player);
void apple_move_animation(Sprite *apple);
int off_screen(Sprite *sprite);
int collision(Sprite *s1, Sprite *s2, int hb);
void apple_kill_animation(Sprite *apple);
void squirrel_kill_animation(Sprite *squirrel);
void squirrel_move_towards(Sprite *squirrel, Sprite *destination_sprite);