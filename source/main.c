#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tonc.h>
#include "player.h"
#include "apple.h"
#include "squirrel.h"
#include "input.h"

#define MAX_APPLES 5
#define APPLE_SPEED 7

OBJ_ATTR obj_buffer[128]; // allocate space for 128 sprites

int max_num_apples = 5;
int curr_apple_count = 0;

typedef struct{
	OBJ_ATTR *mem_location;
	int thrown;
	int x_pos;
	int y_pos;
	int direction;
	int index;
} Apple;

Apple* apples_thrown[5];


// GBA Memory Functions
void copy_tiles(const unsigned int *tileSet, int tileSetStart, int tileSetLen){
	memcpy(&tile_mem[4][tileSetStart], tileSet, tileSetLen);
}

void copy_palette(const unsigned int *palSet, int palSetStart, int palSetLen){
	memcpy(pal_obj_mem+palSetStart, palSet, palSetLen);
}

// init sprite struct for player
SpriteStruct *init_sprite(int sprite_num, int size, int max_frame, int tile_id, int pal_bank, int x, int y){
	// allocate space for sprite struct
	SpriteStruct *newSprite = (SpriteStruct *)malloc(sizeof(SpriteStruct));

	// set the values for the sprite struct
	newSprite->mem_location = &obj_buffer[sprite_num];
	newSprite->size = size;
	newSprite->start_tile = tile_id;
	newSprite->curr_frame = 1; // always start with frame 1
	newSprite->max_frame = max_frame;
	newSprite->dir_facing = 1; // always start facing right
	newSprite->x_pos = x;
	newSprite->y_pos = y;

	// set the initial attributes
	newSprite->mem_location->attr0 = ATTR0_4BPP; // all our sprites will be 4bpp
	newSprite->mem_location->attr2 = ATTR2_PALBANK(pal_bank) | tile_id;
	if(size == 32)
		newSprite->mem_location->attr1 = ATTR1_SIZE_32;
	else if(size == 16)
		newSprite->mem_location->attr1 = ATTR1_SIZE_16;
	else if(size == 8)
		newSprite->mem_location->attr1 = ATTR1_SIZE_8;

	obj_set_pos(newSprite->mem_location, x, y); // set initial position

	return newSprite;
}

void play(SpriteStruct *player)
{
	while (1)
	{
		// slow frame rate
		for (int i = 0; i < 4; i++)
		{
			vid_vsync(); // wait for VBlank
			key_poll();	 // poll which keys are activated
		}


		// get the user input
		int action = input(player, obj_buffer); // update player input
		// iterate over apples thrown
		for (int i = 0; i < MAX_APPLES; i++)
		{
			// define selected apple
			Apple *this_apple = apples_thrown[i];

			// if this apple hasn't been thrown
			if (this_apple->thrown == 0)
			{
				// and player wants to throw
				if (action == 1)
				{
					this_apple->thrown = 1; // indicate it's been thrown
					this_apple->x_pos = player->x_pos + 20; // new x pos
					this_apple->y_pos = player->y_pos + 15; // new y pos
					this_apple->direction = player->dir_facing; // apple direction
					obj_set_pos(this_apple->mem_location, this_apple->x_pos, this_apple->y_pos); // set position
					// obj_unhide(this_apple->mem_location, ATTR0_REG); // unhide the apple
					break;
				}
			}

			// if this apple has already been thrown
			else
			{
				// update x position
				this_apple->x_pos = (this_apple->x_pos + (this_apple->direction * APPLE_SPEED) );
				obj_set_pos(this_apple->mem_location, this_apple->x_pos, this_apple->y_pos); // set position

				if( (this_apple->x_pos > SCREEN_WIDTH) || (this_apple->x_pos < 0) ){
					// murder that apple
					obj_set_pos(this_apple->mem_location, 0 + (this_apple->index *8), 0); // set to 0,0
					this_apple->thrown = 0; // set as not thrown
					// obj_hide(this_apple->mem_location); // hide the sprite
				}
			}
		}
		oam_copy(oam_mem, obj_buffer, 1+MAX_APPLES); // update objects

	}
}

int main()
{
	oam_init(obj_buffer, 128); // initialize 128 sprites
	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D; // Enable Objects & OBJ-VRAM as array

	copy_tiles(playerTiles, 0, playerTilesLen); // copy tile data to mem
	copy_palette(playerPal, 0, playerPalLen); // copy palette dat to mem

	copy_tiles(appleTiles, 64, appleTilesLen); // copy tile data to mem
	copy_palette(applePal, 16, applePalLen); // copy palette dat to mem

	// copy_tiles(squirrelTiles, 66, squirrelTilesLen); // copy tile data to mem
	// copy_palette(squirrelPal, 32, squirrelPalLen); // copy palette dat to mem
	
	SpriteStruct *player = init_sprite(0, 32, 4, 0, 0, 100, 50);

	// fill sprite slots 1 through MAX_APPLES with apple sprites
	for(int i = 0; i < MAX_APPLES; i++){
		// create new apple sprite
		init_sprite(i + 1, 8, 1, 64, 1, 0+(i*8), 0);

		// add to thrown apple list
		Apple *newApple = (Apple *)malloc(sizeof(Apple)); // create new apple struct
		newApple->mem_location = &obj_buffer[i+ 1]; // set memory location
		newApple->index = i; // set position in apple thrown list
		newApple->thrown = 0; // set as not thrown yet
		apples_thrown[i] = newApple; // add to list of thrown apples

		// hide the apple
		// obj_hide(newApple->mem_location);
	}

	// SpriteStruct *squirrel = init_sprite(2, 16, 4, 66, 2, 50, 50);

	oam_copy(oam_mem, obj_buffer, 1+MAX_APPLES); // copy data from OAM buffer to real OAM

	play(player); // run the game

	while(1);
	
	return 0;
}