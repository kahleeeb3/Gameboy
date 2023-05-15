#include <tonc.h> // GBA library
#include <stdlib.h> // for srand
#include <time.h> // for seeding srand
// #include <stdio.h>
// #include <string.h>
#include "background.h"
#include "sprites.h"

OBJ_ATTR obj_buffer[128]; // allocate space for 128 sprites
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer; // buffer for object affine

Map *map; // global pointer which give the map position
Sprite* players[PLAYER_MAX]; // array of player sprite pointers
Sprite* apples[APPLE_MAX]; // array of apple sprite pointers
Sprite* squirrels[SQUIRREL_MAX]; // array of squirrel sprite pointers
Sprite* buildings[BUILDINGS_MAX]; // array of squirrel sprite pointers

int player_score = 0;
int squirrel_count = 0;

#define MAP_MAX_X 270
#define MAP_MAX_Y 260
#define PLAYER_MAX_X 208
#define PLAYER_MIN_X 0
#define PLAYER_MAX_Y 128
#define PLAYER_MIN_Y 29

void text_init()
{

	/*
	 * To be quite frank, the full functionality of this is a bit complex.
	 * Most of this code was borrowed from the library documentation.
	 * https://www.coranac.com/tonc/text/tte.htm
	 * Essentially it enables a set of text tiles (similar to sprites)
	 * and a palette of text colors to use. I provided as much documentation
	 * as possible here.
	 */

	REG_DISPCNT |= DCNT_BG0; // enbale background 0
	irq_init(NULL); // Initialize the interrupt system with no callback function
    irq_add(II_VBLANK, NULL); // Add a VBLANK interrupt with no callback function

	// Initialize the TTE library for displaying text in mode 4 (4bpp) with
    // 16x16 character cells using the Verdana 9 font with shading
	 tte_init_chr4c(
        0,                              // BG number.
        BG_CBB(2) | BG_SBB(10),         // BG control.
        0xF000,                         // Screen-entry base
        bytes2word(13, 15, 0, 0),       // Color attributes.
        CLR_WHITE,                      // Ink color
        &verdana9_b4Font,               // Verdana 9, with shade.
        (fnDrawg)chr4c_drawg_b4cts_fast // b4cts renderer, asm version
    );

	tte_init_con(); // Initialize the console I/O for text output
}

void move_sprite(Sprite *sprite, int x_dir, int y_dir, int speed)
{
	sprite->x_pos += x_dir*speed;
	sprite->y_pos += y_dir*speed;
}

void move_map(Map *map, int x_dir, int y_dir, int speed)
{
	map->x_pos += x_dir*speed;
	map->y_pos += y_dir*speed;

	REG_BG1HOFS= map->x_pos;
	REG_BG1VOFS= map->y_pos;

}

int get_center(Sprite *sprite)
{
	return (sprite->size/2) - 1;
}

int get_rel_y_pos(Sprite *sprite)
{
	// y value for center of screen
	int screen_y_center = SCREEN_HEIGHT/2 - 1;

	// y value for center of player
	int sprite_y_center = get_center(sprite);

	// relative center for the sprite
	int rel_y_center = (screen_y_center) - (sprite_y_center);

	// if sprite is below of the center of the screen
	if ( sprite->y_pos > rel_y_center)
		return -1;
	// if sprite is above of the center of the screen
	else if ( sprite->y_pos < rel_y_center)
		return 1;

	// player is center
	else
		return 0;
}

int get_rel_x_pos(Sprite *sprite)
{
	// x value for center of screen
	int screen_x_center = SCREEN_WIDTH/2 - 1;

	// x value for center of player
	int sprite_x_center = get_center(sprite);

	// relative center for the sprite
	int rel_x_center = (screen_x_center) - (sprite_x_center);

	// if sprite is left of the center of the screen
	if ( sprite->x_pos < rel_x_center)
		return -1;
	// if sprite is right of the center of the screen
	else if ( sprite->x_pos > rel_x_center)
		return 1;

	// player is center
	else
		return 0;
}

int get_rel_diag_pos(Sprite *sprite)
{
	int screen_x_center = SCREEN_WIDTH/2 - 1;
	int screen_y_center = SCREEN_HEIGHT/2 - 1;

	int sprite_x_center = get_center(sprite);
	int sprite_y_center = get_center(sprite);

	int rel_x_center = (screen_x_center) - (sprite_x_center);
	int rel_y_center = (screen_y_center) - (sprite_y_center);

	// Sprite is NE
	if ( (sprite->x_pos > rel_x_center) && (sprite->y_pos < rel_y_center) )
		return 1; // in first quadrant

	// Sprite is NW
	else if ( (sprite->x_pos < rel_x_center) && (sprite->y_pos < rel_y_center) )
		return 2; // in second quadrant

	// Sprite is SW
	else if ( (sprite->x_pos < rel_x_center) && (sprite->y_pos > rel_y_center) )
		return 3; // in third quadrant

	// Sprite is SE
	else if ( (sprite->x_pos > rel_x_center) && (sprite->y_pos > rel_y_center) )
		return 4; // in third quadrant

	else
		return -1; // player is center

}

void walk_animation(Sprite *sprite, int end_frame)
{
    // get start_tile from spriteStruct
	int st = sprite->start_tile;
	// get curr_frame from spriteStruct
	int cf = sprite->curr_frame;
	// get size from spriteStruct
	int s = sprite->size;
	// get pal_bank from spriteStruct
	int pb = sprite->pal_bank;
    // number of tiles that make up a single frame
    int tpf = (s*s)/(8*8);

	// update attribute 2
	sprite->mem_addr->attr2 = ATTR2_PALBANK(pb) | (st + (tpf * cf));
	cf = (cf + 1) % end_frame;

	//update curr_frame
	sprite->curr_frame = cf;
}

void turn_animation(Sprite *sprite)
{
    // flip the sprite
    sprite->mem_addr->attr1 ^= ATTR1_HFLIP;

    // change the sprite facing direction
    if(sprite->dir_facing == 1)
        sprite->dir_facing = -1;
    else
        sprite->dir_facing = 1;
}

void player_skid_animation(Sprite *player)
{
    player->mem_addr->attr2 = PLAYER_FRAME4; // switch to frame 4
    turn_animation(player); // flip the sprite        
}

void traverse(Sprite *player)
{
	
	int skid = 0; // bool for if a skid occurred
	int dx = key_tri_horz(); // tri-bool for x-axis movement
	int dy = key_tri_vert(); // tri-bool for y-axis movement
	
	// player moving diagonal
	if ( (dx != 0)  && (dy!= 0) )
	{

		int quadrant = get_rel_diag_pos(player);

		// Player moving NE
		if ( dx == 1 && dy == -1)
		{

			if(player->dir_facing == -1)
			{
				player_skid_animation(player);
				skid = 1;
			}
			
			if ( quadrant == 3 )
			{
				move_sprite(player, dx, dy, PLAYER_SPEED);
			}
			else
			{
				// if you can move the map right
				if ( map->x_pos < MAP_MAX_X )
				{
					move_map(map, dx, 0, PLAYER_SPEED);
				}
				else if (player->x_pos < PLAYER_MAX_X)
				{
					move_sprite(player, dx, 0, PLAYER_SPEED);
				}

				// if you can move the map up
				if ( map->y_pos > 0)
				{
					move_map(map, 0, dy, PLAYER_SPEED);
				}
				else if (player->y_pos > PLAYER_MIN_Y)
				{
					move_sprite(player, 0, dy, PLAYER_SPEED);
				}
			}

		}

		// Player moving SE
		else if ( dx == 1 && dy == 1)
		{
			
			if(player->dir_facing == -1)
			{
				player_skid_animation(player);
				skid = 1;
			}
			
			if ( quadrant == 2 )
			{
				move_sprite(player, dx, dy, PLAYER_SPEED);
			}
			else
			{
				// if you can move the map right
				if ( map->x_pos < MAP_MAX_X )
				{
					move_map(map, dx, 0, PLAYER_SPEED);
				}
				else if (player->x_pos < PLAYER_MAX_X)
				{
					move_sprite(player, dx, 0, PLAYER_SPEED);
				}

				// if you can move the map down
				if ( map->y_pos < MAP_MAX_Y )
				{
					move_map(map, 0, dy, PLAYER_SPEED);
				}
				else if (player->y_pos < PLAYER_MAX_Y)
				{
					move_sprite(player, 0, dy, PLAYER_SPEED);
				}
			}
		}

		// Player moving NW
		else if ( dx == -1 && dy == -1)
		{
			
			if(player->dir_facing == 1)
			{
				player_skid_animation(player);
				skid = 1;
			}
			
			if ( quadrant == 4 )
			{
				move_sprite(player, dx, dy, PLAYER_SPEED);
			}
			else
			{
				// if you can move the map left
				if ( map->x_pos > 0 )
				{
					move_map(map, dx, 0, PLAYER_SPEED);
				}
				else if (player->x_pos > PLAYER_MIN_X)
				{
					move_sprite(player, dx, 0, PLAYER_SPEED);
				}

				// if you can move the map up
				if ( map->y_pos > 0 )
				{
					move_map(map, 0, dy, PLAYER_SPEED);
				}
				else if (player->y_pos > PLAYER_MIN_Y)
				{
					move_sprite(player, 0, dy, PLAYER_SPEED);
				}
			}
			
		}

		// Player moving SW
		else if ( dx == -1 && dy == 1)
		{
			
			if(player->dir_facing == 1)
			{
				player_skid_animation(player);
				skid = 1;
			}
			
			if ( quadrant == 1 )
			{
				move_sprite(player, dx, dy, PLAYER_SPEED);
			}
			else
			{
				// if you can move the map left
				if ( map->x_pos > 0 )
				{
					move_map(map, dx, 0, PLAYER_SPEED);
				}
				else if (player->x_pos > PLAYER_MIN_X)
				{
					move_sprite(player, dx, 0, PLAYER_SPEED);
				}

				// if you can move the map down
				if ( map->y_pos < MAP_MAX_Y )
				{
					move_map(map, 0, dy, PLAYER_SPEED);
				}
				else if (player->y_pos < PLAYER_MAX_Y)
				{
					move_sprite(player, 0, dy, PLAYER_SPEED);
				}
			}
			
		}
	}

	// player moving in x direction
	else if ( dx != 0 )
	{

		// player moving right
		if ( dx == 1)
		{
			
			if(player->dir_facing == -1)
			{
				player_skid_animation(player);
				skid = 1;
			}
			
			// player is left of the center
			if( get_rel_x_pos(player) == -1 )
			{
				move_sprite(player, dx, 0, PLAYER_SPEED);
			}

			// map is not FULLY to the right
			else if (map->x_pos < MAP_MAX_Y)
			{
				move_map(map, dx, 0, PLAYER_SPEED);
			}
			else if ( player->x_pos < PLAYER_MAX_X )
			{
				move_sprite(player, dx, 0, PLAYER_SPEED);
			}
		}

		// player moving left
		else
		{
			
			if(player->dir_facing == 1)
			{
				player_skid_animation(player);
				skid = 1;
			}
			
			// player is right of the center
			if( get_rel_x_pos(player) == 1 )
			{
				move_sprite(player, dx, 0, PLAYER_SPEED);
			}

			// map is not FULLY to the left
			else if (map->x_pos > 0)
			{
				move_map(map, dx, 0, PLAYER_SPEED);
			}
			else if ( player->x_pos > PLAYER_MIN_X )
			{
				move_sprite(player, dx, 0, PLAYER_SPEED);
			}
		}
	}

	// player moving in y direction
	else if ( dy != 0 )
	{
		// player moving up
		if ( dy == -1)
		{
			// player is below of the center
			if( get_rel_y_pos(player) == -1 )
			{
				move_sprite(player, 0, dy, PLAYER_SPEED);
			}

			// map is not FULLY to the top
			else if (map->y_pos > 0)
			{
				move_map(map, 0, dy, PLAYER_SPEED);
			}
			else if( player-> y_pos > PLAYER_MIN_Y)
			{
				move_sprite(player, 0, dy, PLAYER_SPEED);
			}
		}

		// player moving down
		else
		{
			// player is above of the center
			if( get_rel_y_pos(player) == 1 )
			{
				move_sprite(player, 0, dy, PLAYER_SPEED);
			}

			// map is not FULLY to the bottom
			else if (map->y_pos < MAP_MAX_Y)
			{
				move_map(map, 0, dy, PLAYER_SPEED);
			}
			else if( player->y_pos < PLAYER_MAX_Y )
			{
				move_sprite(player, 0, dy, PLAYER_SPEED);
			}
		}
	}

	if( (dx != 0) || (dy!= 0) )
	{
		if( skid != 1)
		{
			walk_animation(player, 2);
		}
	}

}

void play_game()
{

	Sprite *player = players[0]; // define the player

	while(1)
	{
		VBlankIntrWait(); // Wait for the VBLANK interrupt


		// slow player input a little
		for (int i = 0; i < 4; i++)
		{
			vid_vsync(); // wait for VBlank
			key_poll();	 // poll which keys are activated
		}

		traverse(player);

		obj_set_pos(player->mem_addr, player->x_pos, player->y_pos); // set player position
		oam_copy(oam_mem, obj_buffer, 1 + APPLE_MAX + SQUIRREL_MAX + BUILDINGS_MAX); // update all sprites

	}
}

int main()
{
	oam_init(obj_buffer, 128); // initialize 128 sprites
	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D; // Enable Objects & OBJ-VRAM as array

	copy_sprite_data(); // move sprite data into obj_buffer

	set_player_attributes(obj_buffer, players); // populate the player Sprite Struct
	set_apples_attributes(obj_buffer, apples); // populate the apples Sprite Struct pointer array
	set_squirrels_attributes(obj_buffer, squirrels); // populate the squirrels Sprite Struct pointer array
	set_buildings_attributes(obj_buffer, obj_aff_buffer, buildings); // populate the buildings Sprite Struct pointer array

	map = map_init(); // enable the map
	text_init(); // enable the text display
	srand(time(NULL)); // seed random number generator

	oam_copy(oam_mem, obj_buffer, 1 + APPLE_MAX + SQUIRREL_MAX + BUILDINGS_MAX); // update all sprites

	play_game();

	// while(1);

	return 0;
}