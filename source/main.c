#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tonc.h>
#include "player.h"
#include "apple.h"
#include "squirrel.h"
#include "input.h"

OBJ_ATTR obj_buffer[128]; // allocate space for 128 sprites
Apple* apples[MAX_APPLES];
Squirrel* squirrels[MAX_SQUIRRELS];


void copy_tiles(const unsigned int *tileSet, int tileSetStart, int tileSetLen)
{
	memcpy(&tile_mem[4][tileSetStart], tileSet, tileSetLen);
}

void copy_palette(const unsigned int *palSet, int palSetStart, int palSetLen)
{
	memcpy(pal_obj_mem+palSetStart, palSet, palSetLen);
}

SpriteStruct *init_sprite(int sprite_num, int size, int max_frame, int tile_id, int pal_bank, int x, int y)
{
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

void throw_apple(SpriteStruct *player, Apple *apple)
{
	apple->thrown = 1; // indicate it's been thrown
	apple->x_pos = player->x_pos + 20; // new x pos
	apple->y_pos = player->y_pos + 15; // new y pos
	apple->direction = player->dir_facing; // apple direction
	obj_set_pos(apple->mem_location, apple->x_pos, apple->y_pos); // set position
	// obj_unhide(this_apple->mem_location, ATTR0_REG); // unhide the apple
}

void move_apple(Apple *apple)
{
	apple->x_pos = (apple->x_pos + (apple->direction * APPLE_SPEED) ); // update x position
	obj_set_pos(apple->mem_location, apple->x_pos, apple->y_pos); // set position
}

void kill_squirrel(Squirrel *squirrel)
{
	obj_hide(squirrel->mem_location);
	squirrel->hit = 1;
}

void check_hit_by_apple(Squirrel *squirrel, Apple *apple)
{
	// find center of apple
	int Ax = apple->x_pos + 5;
	int Ay = apple->y_pos + 5;
	
	// find center of the squirrel
	int Sx = squirrel->x_pos + 8;
	int Sy = squirrel->y_pos + 10;
	int hb = SQUIRREL_HIT_BOX; // hit box size 

	// if within x range
	if( (Ax >= (Sx-hb) ) && (Ax <= (Sx+hb) ) ) 
	{
		// if within y range
		if( (Ay >= (Sy-hb) ) && (Ay <= (Sy+hb) ) )
		{
			// remove apple
			obj_set_pos(apple->mem_location, 0 + (apple->index *8), 0); // set to 0,0
			apple->thrown = 0; // set as not thrown

			// remove squirrel
			kill_squirrel(squirrel);

		}
	}
}

void move_squirrel_towards_player(Squirrel *squirrel, SpriteStruct *player)
{
	// find squirrel center
	int Sx = squirrel->x_pos + 8;
	int Sy = squirrel->y_pos + 10;
	
	// find player center
	int Px = player->x_pos + 15;
	int Py = player->y_pos + 14;

	// find x and y distance between them
	int dx = Px - Sx;
	int dy = Py - Sy;

	// determine direction to move
	int x_dir = 1; // pos or neg 1
	int y_dir = 1; // pos or neg 1
	if(dx < 0)
		x_dir = -1;
	if(dy < 0)
		y_dir = -1;

	/* 
	 * Deciding how much to move in the x-direction and y-direction:
	 * Ideally, this would be done by finding the ratio between the distance needed to travel in each direction 
	 * and the hypotenuse of the right triangle created by the player and squirrel. However, we can only
	 * move in integer quantities of pixels. And in situations of Sufficiently small angles, we get a
	 * divide by zero error. So instead we will take the ratio between the x distance needed to travel and the
	 * y distance needed to travel. If that ratio is large, we should move solely in the x direction.
	 * If that angle is small, we should move in the y direction. If its roughly 1, we should move in both directions.
	 */

	int move_x;
	int move_y;
	u16 ratio = (x_dir*dx)/(y_dir*dy);

	if(ratio > 1.25) // x>>y
	{
		move_x = 1;
		move_y = 0;
	}
	if(ratio < 0.75) // y >> x
	{
		move_x = 0;
		move_y = 1;
	}
	else if((ratio >= 0.75) && (ratio <= 1.25)) // x >> y
	{
		move_x = 1;
		move_y = 1;
	}

	squirrel->x_pos += x_dir * move_x; // update x location
	squirrel->y_pos += y_dir * move_y; // update y location
	obj_set_pos(squirrel->mem_location, squirrel->x_pos, squirrel->y_pos);

	// if headed right but facing left
	if(x_dir == 1 && squirrel->dir_facing == -1)
	{
		squirrel->mem_location->attr1 ^= ATTR1_HFLIP;
		squirrel->dir_facing = 1;
	}
	// headed left but facing right
	else if(x_dir == -1 && squirrel->dir_facing == 1)
	{
		squirrel->mem_location->attr1 ^= ATTR1_HFLIP;
		squirrel->dir_facing = -1;
	}


	// if squirrel is in hit box, kill squirrel
	int hb = 8;
	if ((Sx >= (Px - hb)) && (Sx <= (Px + hb)))
	{
		// if within y range
		if ((Sy >= (Py - hb)) && (Sy <= (Py + hb)))
		{
			// remove squirrel
			kill_squirrel(squirrel);
		}
	}

}

void squirrel_walk(Squirrel *squirrel){
	int start_tile = 66;
	int end_frame = 4;
	int size = 16;
	int pal_bank = 2;
	int curr_frame = squirrel->curr_frame;
	int tpf = (size*size)/(8*8); // number of tiles that make up a single frame

	// update attribute 2
	squirrel->mem_location->attr2 = ATTR2_PALBANK(pal_bank) | (start_tile + (tpf * curr_frame));
	curr_frame = (curr_frame + 1) % end_frame;

	//update curr_frame
	squirrel->curr_frame = curr_frame;
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

		// for each apple
		for (int i = 0; i < MAX_APPLES; i++)
		{
			// define selected apple
			Apple *this_apple = apples[i];

			// if this apple hasn't been thrown
			if (this_apple->thrown == 0)
			{
				// and player wants to throw
				if (action == 1)
				{
					// throw the apple
					throw_apple(player, this_apple);
					break;
				}
			}

			// if this apple has already been thrown
			else
			{
				// move the apple
				move_apple(this_apple);

				// for each squirrel
				for( int i = 0; i<MAX_SQUIRRELS; i++){

					// define the selected squirrel
					Squirrel *this_squirrel = squirrels[i];

					// if squirrel has not been hit
					if( this_squirrel->hit == 0){
						
						// check if the squirrel is hit by an apple
						check_hit_by_apple(this_squirrel, this_apple);

					}

				}

				
				// if the apple is off the screen
				if( (this_apple->x_pos > SCREEN_WIDTH) || (this_apple->x_pos < 0) ){
					// murder that apple
					obj_set_pos(this_apple->mem_location, 0 + (this_apple->index *8), 0); // set to 0,0
					this_apple->thrown = 0; // set as not thrown
					// obj_hide(this_apple->mem_location); // hide the sprite
				}
			}
		
		}

		// for each squirrel
		for( int i = 0; i<MAX_SQUIRRELS; i++){

			// define the selected squirrel
			Squirrel *this_squirrel = squirrels[i];

			// if squirrel has not been hit
			if( this_squirrel->hit == 0){

				// move the squirrel towards player
				move_squirrel_towards_player(this_squirrel, player);
				squirrel_walk(this_squirrel);

			}

		}
		
		oam_copy(oam_mem, obj_buffer, 1+MAX_APPLES+MAX_SQUIRRELS); // update all objects

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

	copy_tiles(squirrelTiles, 66, squirrelTilesLen); // copy tile data to mem
	copy_palette(squirrelPal, 32, squirrelPalLen); // copy palette dat to mem
	
	SpriteStruct *player = init_sprite(0, 32, 4, 0, 0, 100, 50);

	// fill sprite slots 1 through MAX_APPLES with apple sprites
	for(int i = 0; i < MAX_APPLES; i++){
		// create new apple sprite
		init_sprite(i + 1, 8, 1, 64, 1, 0+(i*8), 0);

		// add to thrown apple list
		Apple *newApple = (Apple *)malloc(sizeof(Apple)); // create new apple struct
		newApple->mem_location = &obj_buffer[1+ i]; // set memory location
		newApple->index = i; // set position in apple thrown list
		newApple->thrown = 0; // set as not thrown yet
		apples[i] = newApple; // add to list of thrown apples

		// hide the apple
		// obj_hide(newApple->mem_location);
	}

	// fill sprite slots (1+MAX_APPLES) through MAX_SQUIRRELS with squirrel sprites
	for(int i = 0; i < MAX_SQUIRRELS; i++){

		int x = 20;
		int y = 16 + (i*16);

		// create new squirrel sprite
		init_sprite(1 + MAX_APPLES + i, 16, 4, 66, 2, x, y);

		Squirrel *newSquirrel = (Squirrel *) malloc(sizeof(Squirrel));
		newSquirrel->mem_location = &obj_buffer[1 + MAX_APPLES + i];
		newSquirrel->x_pos = x;
		newSquirrel->y_pos = y;
		newSquirrel->index = i;
		newSquirrel->hit = 0;
		newSquirrel->curr_frame = 1;
		newSquirrel->dir_facing = -1;
		squirrels[i] = newSquirrel;
	}

	oam_copy(oam_mem, obj_buffer, 1+MAX_APPLES+MAX_SQUIRRELS); // copy data from OAM buffer to real OAM

	play(player); // run the game

	while(1);
	
	return 0;
}