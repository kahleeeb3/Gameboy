#include <tonc.h> // GBA library
#include <stdlib.h> // for srand
#include <time.h> // for seeding srand
// #include <stdio.h>
// #include <string.h>
#include "background.h"
#include "sprites.h"
#include "animations.h"
#include "helper.h"

#define MAP_MAX_X 270
#define MAP_MAX_Y 260
#define PLAYER_MAX_X 208
#define PLAYER_MIN_X 0
#define PLAYER_MAX_Y 128
#define PLAYER_MIN_Y 0

OBJ_ATTR obj_buffer[128]; // allocate space for 128 sprites
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer; // buffer for object affine

Map *map; // global pointer which give the map position
Sprite* players[PLAYER_MAX]; // array of player sprite pointers
Sprite* apples[APPLE_MAX]; // array of apple sprite pointers
Sprite* squirrels[SQUIRREL_MAX]; // array of squirrel sprite pointers
Sprite* buildings[BUILDINGS_MAX]; // array of squirrel sprite pointers
int building_health[8] = {100,100,100,100,100,100,100,100}; // all building have 100% health at first

int player_score = 0;
int squirrel_count = 0;

void display_building_health()
{
	// these coords are just guesses rn
	tte_printf("#{P:50,63}%d", building_health[0]);
	tte_printf("#{P:86,51}%d", building_health[1]);
	tte_printf("#{P:121,55}%d", building_health[2]);
	tte_printf("#{P:155,54}%d", building_health[3]);
	tte_printf("#{P:90,112}%d", building_health[4]);
	tte_printf("#{P:121,112}%d", building_health[5]);
	tte_printf("#{P:154,104}%d", building_health[6]);
	tte_printf("#{P:168,79}%d", building_health[7]);
}

void display_score()
{
	tte_printf("#{P:2,0}Score: %d", player_score);
	tte_printf("#{P:2,12}Squirrels: %d", squirrel_count);
}

void clear_text_display(){
	tte_printf("#{es;}");
}

int building_off_screen(Sprite *building)
{
	int bottom = building->y_pos + 64;
	int right = building->x_pos + 128;
	int top = building->y_pos;

	if( (top > SCREEN_HEIGHT) || (bottom < 0) || ( right < 0) ){
		return 1;
	}

	return 0;
}

void finalize_sprite_positions(Sprite *player)
{
	// set the updated position for player
	obj_set_pos(player->mem_addr, player->x_pos, player->y_pos); // set player position

	// set the updated position for all active apples
	for (int i = 0; i < APPLE_MAX; i++)
	{
		Sprite *apple = apples[i];
		obj_set_pos(apple->mem_addr, apple->x_pos, apple->y_pos);
	}
	
	// set the updated position for all active squirrels
	for(int i=0; i<SQUIRREL_MAX; i++)
	{
		Sprite *squirrel = squirrels[i];
		if(squirrel->active == 1){
			obj_set_pos(squirrel->mem_addr, squirrel->x_pos, squirrel->y_pos);

		}
	}

	// set the updated position for all active buildings
	for(int i=0; i<BUILDINGS_MAX; i++)
	{
		Sprite *building = buildings[i];
		if(building->active == 1){
			obj_set_pos(building->mem_addr, building->x_pos, building->y_pos);
		}
	}

}

void move_all_sprites(int x_dir, int y_dir, int speed)
{
    // move all active apples
	for (int i = 0; i < APPLE_MAX; i++)
	{
		Sprite *apple = apples[i];
		if(apple->active == 1){
			move_sprite(apple, x_dir * -1, y_dir * -1, speed);
		}

	}
	
	// move all active squirrels
	for(int i=0; i<SQUIRREL_MAX; i++)
	{
		Sprite *squirrel = squirrels[i];
		if(squirrel->active == 1){
			move_sprite(squirrel, x_dir * -1, y_dir * -1, speed);
			if( off_screen(squirrel) == 1 ){
				squirrel->hidden = 1;
				obj_hide(squirrel->mem_addr);
			}
			else{
				if(squirrel->hidden){
					obj_unhide(squirrel->mem_addr, ATTR1_SIZE_16);
				}
			}

		}
	}

	// move all active buildings
	for(int i=0; i<BUILDINGS_MAX; i++)
	{
		Sprite *building = buildings[i];
		if(building->active == 1){
			move_sprite(building, x_dir * -1, y_dir * -1, speed);
			
			if( building_off_screen(building) == 1 ){
				building->hidden = 1;
				obj_hide(building->mem_addr);
			}
			else{
				if(building->hidden){
					obj_unhide(building->mem_addr, ATTR1_SIZE_64x32);
					scale_sprite(building, &obj_aff_buffer[i], i, 2); // scale by a factor of 2

				}
			}

		}
	}

}

void move_map(Map *map, int x_dir, int y_dir, int speed)
{
	map->x_pos += x_dir*speed;
	map->y_pos += y_dir*speed;

	REG_BG1HOFS= map->x_pos;
	REG_BG1VOFS= map->y_pos;

	move_all_sprites(x_dir, y_dir, speed);

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
	int showing_map = 0;

	// spawn all the squirrels
	for( int i=0; i<SQUIRREL_MAX; i++){
		spawn_squirrel(i, squirrels);
		squirrel_count++;
	}

	while(1)
	{
		VBlankIntrWait(); // Wait for the VBLANK interrupt
		
		// clear_text_display();
		display_score();

		// slow player input a little
		for (int i = 0; i < 4; i++)
		{
			vid_vsync(); // wait for VBlank
			key_poll();	 // poll which keys are activated
		}

		traverse(player); // let player traverse map

		if(key_is_down(KEY_R) && showing_map == 0){
			REG_DISPCNT ^= DCNT_WIN0 | DCNT_WIN1; // enable window 0 and 1
			display_building_health();
			showing_map = 1;
		}
		if (key_is_up(KEY_R) && showing_map == 1){
			REG_DISPCNT ^= DCNT_WIN0 | DCNT_WIN1; // enable window 0 and 1
			showing_map = 0;
			clear_text_display();
			display_score();
		}



		// for each apple
		for (int i = 0; i < APPLE_MAX; i++)
		{
			// define selected apple
			Sprite *apple = apples[i];

			if(apple->active == 0)
			{
				// if A is held
				if( key_held(KEY_A) )
				{
					player->throwing = 1; // set player as throwing
				}

				// if player has thrown
				else if (player->throwing == 1)
				{
					// player_throw_animation(player);
					apple_throw_init(apple, player);
					player->throwing = 0; // set as player not throwing anymore
				}
			}
			else
			{
				
				move_sprite(apple, apple->dir_facing, 0, APPLE_SPEED);

				if( off_screen(apple) )
				{
					apple_kill_animation(apple);
				}

				// if the apple hit a squirrel
				for(int i=0; i<SQUIRREL_MAX; i++)
				{
					Sprite *squirrel = squirrels[i];
					if(squirrel->active == 1){
						if( collision(apple, squirrel, SQUIRREL_HIT_BOX) == 1 ){
							squirrel_kill_animation(squirrel);
							player_score++;
							squirrel_count--;
							clear_text_display();
							display_score();
						}

					}
				}
			}
		}

		// move each squirrel
		for(int i=0; i<SQUIRREL_MAX; i++){
			Sprite *squirrel = squirrels[i];
			if(squirrel->active == 1){
				if(squirrel->hidden == 1){
					if(off_screen(squirrel) == 0){
						obj_unhide(squirrel->mem_addr, ATTR1_SIZE_16);
					}
				}
				walk_animation(squirrel, 4);
				// move_towards(squirrel, player);
			}

		}

		finalize_sprite_positions(player); // update all sprite positions
		oam_copy(oam_mem, obj_buffer, 1 + APPLE_MAX + SQUIRREL_MAX + BUILDINGS_MAX); // update all sprites

	}
}

void window_init()
{
    
	/*
	 *
	 * Im really only going to use window 0 for now
	 * this will be used for quick displaying the map
	 */
	
	typedef struct tagRECT_U8 { u8 ll, tt, rr, bb; } ALIGN4 RECT_U8;

	// define positions
	RECT_U8 win[2]= 
	{
		{ 48, 8,  193,  152 },   // win0: where map will be displayed
		{ 0, 0 ,SCREEN_WIDTH, SCREEN_HEIGHT }    // win1: the entire screen
	};
	
	// set positions
	REG_WIN0H= win[0].ll<<8 | win[0].rr;
    REG_WIN1H= win[1].ll<<8 | win[1].rr;
    REG_WIN0V= win[0].tt<<8 | win[0].bb;
    REG_WIN1V= win[1].tt<<8 | win[1].bb;

	// REG_DISPCNT |= DCNT_WIN0 | DCNT_WIN1; // enable window 0 and 1

	// define what goes in each window
	REG_WININ= WININ_BUILD( (WIN_BG0 | WIN_BG2) , (WIN_OBJ | WIN_BG0| WIN_BG1) );
	REG_WINOUT= WINOUT_BUILD(WIN_BG0, 0);
}

void mini_map_init()
{

	// copy data over to registers
	memcpy(pal_bg_mem+16, miniMapPal, miniMapPalLen);
    memcpy(&tile_mem[1][0], miniMapTiles, miniMapTilesLen);
    memcpy(&se_mem[26][0], miniMapMap, miniMapMapLen);

	// change the palette used	
	SCR_ENTRY *bg2_map= se_mem[26];
	for(int i=0; i<1024; i++){
		*bg2_map++ |= SE_PALBANK(1); // set to use palette 1
	}

    REG_BG2CNT = BG_REG_32x32 | BG_SBB(26)| BG_4BPP | BG_CBB(1);
	REG_BG2CNT |= BG_PRIO(2); // set priority
	REG_DISPCNT |= DCNT_BG2;
}


int main()
{
	oam_init(obj_buffer, 128); // initialize 128 sprites
	REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D; // Enable Objects & OBJ-VRAM as array

	copy_sprite_data(); // move sprite data into obj_buffer

	set_player_attributes(obj_buffer, players); // populate the player Sprite Struct
	set_apples_attributes(obj_buffer, apples); // populate the apples Sprite Struct pointer array
	set_squirrels_attributes(obj_buffer, squirrels); // populate the squirrels Sprite Struct pointer array
	set_buildings_attributes(obj_buffer, obj_aff_buffer, buildings); // populate the buildings Sprite Struct pointer array

	map = map_init(); // enable the map
	mini_map_init(); // enable the mini map
	text_init(); // enable the text display
	srand(time(NULL)); // seed random number generator

	oam_copy(oam_mem, obj_buffer, 1 + APPLE_MAX + SQUIRREL_MAX + BUILDINGS_MAX); // update all sprites    

	window_init();
	
	play_game();

	// while(1);

	return 0;
}