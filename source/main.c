#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tonc.h>
#include "player.h"
#include "apple.h"
#include "squirrel.h"
#include "input.h"

OBJ_ATTR obj_buffer[128]; // allocate space for 128 sprites
Sprite *player; // global pointer to the player sprite
Sprite* apples[APPLE_MAX]; // array of apple sprite pointers
Sprite* squirrels[APPLE_MAX]; // array of squirrel sprite pointers

void copy_tiles(const unsigned int *tileSet, int tileSetStart, int tileSetLen)
{
	memcpy(&tile_mem[4][tileSetStart], tileSet, tileSetLen);
}

void copy_palette(const unsigned int *palSet, int palSetStart, int palSetLen)
{
	memcpy(pal_obj_mem+palSetStart, palSet, palSetLen);
}

Sprite *init_sprite(int sprite_num, int size, int tile_id, int pal_bank, int active, int x, int y)
{
	// allocate space for sprite struct
	Sprite *newSprite = (Sprite *)malloc(sizeof(Sprite));
	
	// set the values for the sprite struct
	newSprite->mem_addr = &obj_buffer[sprite_num]; // pointer to location in object mem
	newSprite->size = size; // number of pixels in the horizontal direction
	newSprite->active = active; // if the sprite is an active part of the game (visibility)
	newSprite->start_tile = tile_id; // tile number of the first tile
	newSprite->pal_bank = pal_bank; // which palette bank the sprite uses
	newSprite->curr_frame = 1; // always start with frame 1
	newSprite->dir_facing = 1; // always start facing right
	newSprite->x_pos = x; // current x position
	newSprite->y_pos = y; // current y position
	// newSprite->index = 0; // we will set this in a different place

	// set object attribute 0
	newSprite->mem_addr->attr0 = ATTR0_4BPP; // all our sprites will be 4bpp
	
	// set object attribute 1
	if(size == 32)
		newSprite->mem_addr->attr1 = ATTR1_SIZE_32;
	else if(size == 16)
		newSprite->mem_addr->attr1 = ATTR1_SIZE_16;
	else if(size == 8)
		newSprite->mem_addr->attr1 = ATTR1_SIZE_8;

	// set object attribute 2
	newSprite->mem_addr->attr2 = ATTR2_PALBANK(pal_bank) | tile_id;

	// set object position attribute
	obj_set_pos(newSprite->mem_addr, x, y);

	// return the new sprite
	return newSprite;
}

void play()
{
	while (1)
	{
		// slow frame rate
		for (int i = 0; i < 4; i++)
		{
			vid_vsync(); // wait for VBlank
			key_poll();	 // poll which keys are activated
		}

		// move the player sprite
		int action = move_player(player);
		
		// for each apple
		for (int i = 0; i < APPLE_MAX; i++)
		{
			// define selected apple
			Sprite *apple = apples[i];

			// if this apple hasn't been thrown
			if(apple->active == 0)
			{
				// and the player wants to throw
				if(action == 1)
				{
					// move the apple to the players hand
					apple_throw_init(apple, player);
					break;
				}
			}

			// if this apple has already been thrown
			else
			{
				// move the apple
				apple_move_animation(apple);

				// if the apple is off the screen
				if(off_screen(apple) == 1){
					apple_kill_animation(apple); // remove apple
				}

				// for each squirrel
				for( int i = 0; i < SQUIRREL_MAX; i++)
				{
					// define selected squirrel
					Sprite *squirrel = squirrels[i];

					// if squirrel has not been hit
					if( squirrel->active == 1){

						// if apple hit squirrel
						if(collision(apple, squirrel, SQUIRREL_HIT_BOX) == 1){
							apple_kill_animation(apple); // remove apple
							squirrel_kill_animation(squirrel); // remove squirrel
						}
					}
				}


			}

		}

		// for each squirrel
		for( int i = 0; i < SQUIRREL_MAX; i++)
		{
			// define selected squirrel
			Sprite *squirrel = squirrels[i];

			// if squirrel has not been hit
			if( squirrel->active == 1){

				// move towards player
				squirrel_move_towards(squirrel, player);

			}
		}
		
		oam_copy(oam_mem, obj_buffer, 1 + APPLE_MAX + SQUIRREL_MAX);
	}
}


int main()
{
	oam_init(obj_buffer, 128); // initialize 128 sprites
	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D; // Enable Objects & OBJ-VRAM as array

	// copy tile data to memory
	copy_tiles(playerTiles, PLAYER_FRAME1, playerTilesLen); // player tiles
	copy_tiles(appleTiles, APPLE_FRAME1, appleTilesLen); // apple tiles
	copy_tiles(squirrelTiles, 66, squirrelTilesLen); // squirrel tiles

	// copy palette data to memory
	copy_palette(playerPal, PLAYER_PAL_OFFSET, playerPalLen); // player palette
	copy_palette(applePal, APPLE_PAL_OFFSET, applePalLen); // apple palette
	copy_palette(squirrelPal, SQUIRREL_PAL_OFFSET, squirrelPalLen); // squirrel palette
	
	// generate player sprite
	player = init_sprite(0, PLAYER_SIZE, PLAYER_FRAME1, PLAYER_PAL_BANK, 1, 100, 50);
	player->throwing = 0; // set that the player is not throwing

	// fill sprite slots 1 through APPLE_MAX with apple sprites
	for(int i = 0; i < APPLE_MAX; i++){

		int sprite_num = 1 + i;
		int y = 0; // all apples at top of screen
		int x = i*8; // space them apart by 8 pixels

		// create new apple sprite
		Sprite *newApple = init_sprite(sprite_num, APPLE_SIZE, APPLE_FRAME1, APPLE_PAL_BANK, 1, x, y);
		newApple->index = i; // set the index of the apple
		newApple->active = 0; // set as not active (has not been thrown yet)
		apples[i] = newApple; // add to list of apples
	}

	// fill sprite slots (1+APPLE_MAX) through SQUIRREL_MAX with squirrel sprites
	for(int i = 0; i < SQUIRREL_MAX; i++){

		int sprite_num = 1 + APPLE_MAX + i;
		int x = 20;
		int y = 16 + (i*16);

		// create new squirrel sprite
		Sprite *newSquirrel = init_sprite(sprite_num, SQUIRREL_SIZE, SQUIRREL_FRAME1, SQUIRREL_PAL_BANK, 0, x, y);
		newSquirrel->active = 1; // this is how you mark the squirrels as active
		newSquirrel->index = i; // set the index of the squirrel
		newSquirrel->dir_facing = -1; // the sprites were drawn facing the wrong direction. May fix later.
		squirrels[i] = newSquirrel; // add to list of squirrels
	}

	oam_copy(oam_mem, obj_buffer, 1 + APPLE_MAX + SQUIRREL_MAX); // copy data from OAM buffer to real OAM

	play();
	
	return 0;
}