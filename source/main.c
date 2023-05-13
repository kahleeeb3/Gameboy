#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <tonc.h>
#include "player.h"
#include "apple.h"
#include "squirrel.h"
#include "input.h"

OBJ_ATTR obj_buffer[128]; // allocate space for 128 sprites
Sprite *player; // global pointer to the player sprite
Sprite* apples[APPLE_MAX]; // array of apple sprite pointers
Sprite* squirrels[APPLE_MAX]; // array of squirrel sprite pointers

int player_score = 0;
int squirrel_count = 0;

void copy_tiles(const unsigned int *tileSet, int tileSetStart, int tileSetLen)
{
	memcpy(&tile_mem[4][tileSetStart], tileSet, tileSetLen);
}

void copy_palette(const unsigned int *palSet, int palSetStart, int palSetLen)
{
	memcpy(pal_obj_mem+palSetStart, palSet, palSetLen);
}

void text_init()
{
	irq_init(NULL); // Initialize the interrupt system with no callback function
    irq_add(II_VBLANK, NULL); // Add a VBLANK interrupt with no callback function

	// Initialize the TTE library for displaying text in mode 4 (4bpp) with
    // 16x16 character cells using the Verdana 9 font with shading
	 tte_init_chr4c(
        0,                              // BG number.
        BG_CBB(0) | BG_SBB(10),         // BG control.
        0xF000,                         // Screen-entry base
        bytes2word(13, 15, 0, 0),       // Color attributes.
        CLR_WHITE,                      // Ink color
        &verdana9_b4Font,               // Verdana 9, with shade.
        (fnDrawg)chr4c_drawg_b4cts_fast // b4cts renderer, asm version
    );

	tte_init_con(); // Initialize the console I/O for text output
}


void squirrel_spawn(){
	Sprite *squirrel = squirrels[squirrel_count];
	int x = rand() % (SCREEN_WIDTH + 1);
	int y = rand() % (SCREEN_HEIGHT + 1);
	squirrel->x_pos = x;
	squirrel->y_pos = y;
	obj_set_pos(squirrel->mem_addr, x, y);
	obj_unhide(squirrel->mem_addr, ATTR0_4BPP);
	squirrel->active = 1;
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
	
	srand(time(NULL)); // seed random number generator
	text_init(); // initialize text placement
	
	// Overflow every ~1 second:
    // 0x4000 ticks @ FREQ_1024
    REG_TM2D = -0x4000;          // 0x4000 ticks till overflow
    REG_TM2CNT = TM_FREQ_1024;   // we're using the 1024 cycle timer

    REG_TM2CNT |= TM_ENABLE;
    REG_TM3CNT = TM_ENABLE | TM_CASCADE;

    u32 sec = -1;

	// track the round data
	int round_number = 1;
	int squirrels_spawned_in_round = 0;

	while (1)
	{

        // Wait for the VBLANK interrupt
        VBlankIntrWait();

        // Check if the current time has changed
        if (REG_TM3D != sec)
        {
            // Update the stored time
            sec = REG_TM3D;

            // Convert the time to hours, minutes, and seconds
            // u32 hours = sec / 3600;
            u32 minutes = (sec % 3600) / 60;
            u32 seconds = sec % 60;

            tte_printf("#{es;P:0,0}");
			tte_printf("#{P:2,0}Score: %d", player_score);
			tte_printf("#{P:2,12}Squirrels: %d", squirrel_count);
			tte_printf("#{P:210,0}%02d:%02d", minutes, seconds);
        }

		// slow frame rate
		for (int i = 0; i < 4; i++)
		{
			vid_vsync(); // wait for VBlank
			key_poll();	 // poll which keys are activated
		}

		// spawn some squirrels
		if( squirrels_spawned_in_round < SQUIRREL_MAX)
		{
			squirrel_spawn();
			squirrels_spawned_in_round++;
			squirrel_count++;
		}
		// if there are no more squirrels, start a new round
		if( (squirrel_count == 0) && (squirrels_spawned_in_round == SQUIRREL_MAX) )
		{
			round_number++;
			squirrels_spawned_in_round = 0;
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
						if(collision(apple, squirrel, SQUIRREL_HIT_BOX) == 1)
						{
							apple_kill_animation(apple); // remove apple
							squirrel_kill_animation(squirrel); // remove squirrel
							squirrel_count--;
							player_score++;
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
				
				// if squirrel hit the player
				if(collision(squirrel, player , PLAYER_HIT_BOX) == 1)
				{
					// remove squirrel
					squirrel_kill_animation(squirrel);
					player_score--;
					squirrel_count--;
				}

			}
		}
		
		
		if(player_score<0)
		{
			REG_DISPCNT = DCNT_MODE0 | DCNT_BG0; // Disable Objects & OBJ-VRAM as array
			tte_printf("#{es;P:84,50}GAME OVER");
			tte_printf("#{P:65,62}You Survived %d Round", round_number);
			tte_printf("#{P:67,86}press A to try again.");
			key_wait_till_hit(KEY_A);

			// reset scores
			player_score = 0;
			squirrel_count = 0;
			round_number = 1;
			squirrels_spawned_in_round = 0;

			// kill remaining squirrels
			for( int i = 0; i < SQUIRREL_MAX; i++)
				if(squirrels[i]->active == 1)
					squirrel_kill_animation(squirrels[i]);

			REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
		}
		
		oam_copy(oam_mem, obj_buffer, 1 + APPLE_MAX + SQUIRREL_MAX);
	}
}


int main()
{
	oam_init(obj_buffer, 128); // initialize 128 sprites
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D; // Enable Objects & OBJ-VRAM as array

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
		int x = 2 + (i*8); // space them apart by 8 pixels
		int y = 24;

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
		int y = 24 + 8 + (i*16);

		// create new squirrel sprite
		Sprite *newSquirrel = init_sprite(sprite_num, SQUIRREL_SIZE, SQUIRREL_FRAME1, SQUIRREL_PAL_BANK, 0, x, y);
		newSquirrel->index = i; // set the index of the squirrel
		newSquirrel->dir_facing = -1; // the sprites were drawn facing the wrong direction. May fix later.
		newSquirrel->active = 0; // set as not active (has been hit)
		squirrels[i] = newSquirrel; // add to list of squirrels
		obj_hide(newSquirrel->mem_addr); // hide the sprite
	}

	oam_copy(oam_mem, obj_buffer, 1 + APPLE_MAX + SQUIRREL_MAX); // copy data from OAM buffer to real OAM

	play();

	while(1);
	
	return 0;
}