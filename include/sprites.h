#include <tonc.h>


#ifndef SPRITES
#define SPRITES

// Define Max Quantities
#define PLAYER_MAX      1
#define APPLE_MAX 		5
#define SQUIRREL_MAX	5
#define BUILDINGS_MAX 	8

// These will be moved to input later
#define PLAYER_SPEED 		5
#define APPLE_SPEED 		7
#define SQUIRREL_SPEED 		1
#define PLAYER_HIT_BOX		8
#define SQUIRREL_HIT_BOX 	4

// Tile index for sprite frames
#define PLAYER_FRAME1 		0
#define PLAYER_FRAME2		16
#define PLAYER_FRAME3		32
#define PLAYER_FRAME4		48
#define APPLE_FRAME1 		64
#define SQUIRREL_FRAME1 	66
#define SQUIRREL_FRAME2		70
#define SQUIRREL_FRAME3		74
#define SQUIRREL_FRAME4		78
#define BUILDING_FRAME1     82
#define BUILDING_FRAME2     146
#define BUILDING_FRAME3     210
#define BUILDING_FRAME4     274
#define BUILDING_FRAME5     338
#define BUILDING_FRAME6     402
#define BUILDING_FRAME7     466
#define BUILDING_FRAME8     530

// Palette index for spites
#define PLAYER_PAL_OFFSET 		0
#define APPLE_PAL_OFFSET 		16
#define SQUIRREL_PAL_OFFSET 	32
#define BUILDINGS_PAL_OFFSET    48

// define palette banks
#define PLAYER_PAL_BANK 		0
#define APPLE_PAL_BANK			1
#define SQUIRREL_PAL_BANK 		2
#define BUILDINGS_PAL_BANK 		3


// Define Sprite Sizes
#define PLAYER_SIZE 		32
#define APPLE_SIZE 			8
#define SQUIRREL_SIZE 		16
#define BUILDINGS_SIZE      64

// all sprites will have a palette size of 32
// because there is 8 colors at 4 bytes per pixel
// meaning each color is 4 bytes (16 unique colors)
#define PalLen 32

// Sprite Tile Values
#define playerTilesLen 2048
#define appleTilesLen 32
#define squirrelTilesLen 512
#define buildingTilesLen 2048

typedef struct{
	OBJ_ATTR *mem_addr; // pointer to location in object mem
	int throwing; // bool value for if the player sprite threw an apple
	int hidden; // bool for if the sprite is visible or not
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

void copy_tiles(const unsigned int *tileSet, int tileSetStart, int tileSetLen);
void copy_palette(const unsigned int *palSet, int palSetStart, int palSetLen);
Sprite *mkSpriteStruct(OBJ_ATTR *obj_buffer_location, int size, int tile_id, int pal_bank, int active, int x, int y);
void copy_sprite_data();
void scale_sprite(Sprite *sprite, OBJ_AFFINE *obj_aff_buffer_location, int obj_affine_num, int scale_factor);

void set_player_attributes(OBJ_ATTR *obj_buffer, Sprite** players);
void set_apples_attributes(OBJ_ATTR *obj_buffer, Sprite** apples);
void set_squirrels_attributes(OBJ_ATTR *obj_buffer, Sprite** squirrels);
void set_buildings_attributes(OBJ_ATTR *obj_buffer, OBJ_AFFINE *obj_aff_buffer, Sprite** buildings);

#endif // SPRITES