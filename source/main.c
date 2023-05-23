#include <tonc.h> // GBA library
#include <stdlib.h> // for srand
#include <time.h> // for seeding srand
// #include <stdio.h>
// #include <string.h>
#include "background.h"
#include "sprites.h"
#include "animations.h"
#include "helper.h"
#include "loadingScreen.h"


/* GLOBAL VALUES */
OBJ_ATTR obj_buffer[128]; // allocate space for 128 sprites
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer; // buffer for object affine
Map *map; // global pointer which give the map position
Sprite* players[PLAYER_MAX]; // array of player sprite pointers
Sprite* apples[APPLE_MAX]; // array of apple sprite pointers
Sprite* squirrels[SQUIRREL_MAX]; // array of squirrel sprite pointers
Sprite* buildings[BUILDINGS_MAX]; // array of squirrel sprite pointers
int building_health[8] = {100,100,100,100,100,100,100,100}; // all building have 100% health at first
int player_score = 0; // current player score
int squirrel_count = 0; // number of squirrels left in round

void display_text()
{
	tte_printf("#{es;P:2,0}Score: %d", player_score);
	tte_printf("#{P:2,12}Squirrels: %d", squirrel_count);

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


void set_squirrel_targets()
{
	// number of squirrels attacking {player, building 1, ..., building 9}
	int building_quantities[8] = {2,2,0,0,0,0,0,0};
	int player_quantity = 5;

	// for each value in that list
	for(int target_index = 0; target_index < 8; target_index++){

		int selected_value = building_quantities[target_index]; // select the value
		int building_index = (target_index * 2) + 1; // select target building index
		
		// count up from 0 to that value
		for(int i=0; i< selected_value; i++){
			Sprite *squirrel = squirrels[squirrel_count]; // select squirrel
			squirrel->target = building_index; // assign it a target
			spawn_squirrel(squirrel_count, squirrels); // spawn it in
			squirrel_count++; // increase squirrel count
		}
	}

	// assign squirrels to the player
	for(int i =0; i<player_quantity; i++){
		Sprite *squirrel = squirrels[squirrel_count]; // select squirrel
		squirrel->target = -1; // assign it a target (-1 for player)
		spawn_squirrel(squirrel_count, squirrels); // spawn it in
		squirrel_count++; // increase squirrel count
	}

}


void play_game()
{	
	
	set_squirrel_targets(); // just 1 round for now
	Sprite *player = players[0]; // define the player
	int showing_map = 0;
	display_text();

	while(1)
	{
		

		// slow player input a little
		for (int i = 0; i < 4; i++)
		{
			vid_vsync(); // wait for VBlank
			key_poll();	 // poll which keys are activated
		}

		traverse(player); // let player traverse map

		if(key_is_down(KEY_R) && showing_map == 0){
			REG_DISPCNT ^= DCNT_WIN0; // enable window 0
			display_text();
			showing_map = 1;
		}
		if (key_is_up(KEY_R) && showing_map == 1){
			REG_DISPCNT ^= DCNT_WIN0; // disable window 0
			showing_map = 0;
			display_text();
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
							note_play(NOTE_F, 1);
							apple_kill_animation(apple);
							squirrel_kill_animation(squirrel);
							player_score++;
							squirrel_count--;
							display_text();
						}

					}
				}
			}
		}

		// for each squirrel
		for(int i=0; i<SQUIRREL_MAX; i++){
			Sprite *squirrel = squirrels[i];
			if(squirrel->active == 1){
				if(squirrel->hidden == 1){
					if(off_screen(squirrel) == 0){
						obj_unhide(squirrel->mem_addr, ATTR1_SIZE_16);
					}
				}
				walk_animation(squirrel, 4);
				int squirrel_target = squirrel->target;
				if (squirrel_target == -1){
					move_towards(squirrel, player);
				}
				else{
					move_towards(squirrel, buildings[squirrel_target]);
				}

				// if squirrel is hitting building
				for( int i=0; i<BUILDINGS_MAX; i++){
					if( i%2 ){
						Sprite *building = buildings[i];
						if( collision(squirrel, building, SQUIRREL_HIT_BOX) ){
							note_play(NOTE_F, -2);
							building_health[i/2]--;
							display_text();
							squirrel_bump_animation(squirrel, building);
						}

						if(building_health[i/2] < 0){
							obj_hide(buildings[i]->mem_addr);
							obj_hide(buildings[i-1]->mem_addr);
						}


					}
				}


			}

		}

		finalize_sprite_positions(player); // update all sprite positions
		oam_copy(oam_mem, obj_buffer, 1 + APPLE_MAX + SQUIRREL_MAX + BUILDINGS_MAX); // update all sprites

	}
}

int main()
{
	oam_init(obj_buffer, 128); // initialize 128 sprites
	REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D; // Enable Objects & OBJ-VRAM as array

	irq_init(NULL);
    irq_add(II_VBLANK, NULL);

    sound_init();
	
	// loading_screen_init(obj_buffer, obj_aff_buffer);
	// loading_screen_display(obj_buffer);

	copy_sprite_data(); // move sprite data into obj_buffer

	set_player_attributes(obj_buffer, players); // populate the player Sprite Struct
	set_apples_attributes(obj_buffer, apples); // populate the apples Sprite Struct pointer array
	set_squirrels_attributes(obj_buffer, squirrels); // populate the squirrels Sprite Struct pointer array
	set_buildings_attributes(obj_buffer, obj_aff_buffer, buildings); // populate the buildings Sprite Struct pointer array

	map = map_init(); // enable the map
	mini_map_init(); // enable the mini map
	text_init(); // enable the text display


	// enable objs and backgrounds
	REG_DISPCNT |= DCNT_OBJ | DCNT_BG0 | DCNT_BG1| DCNT_BG2;


	REG_SND1CNT= SSQR_ENV_BUILD(12, 0, 1); // update sound to be shorter

	oam_copy(oam_mem, obj_buffer, 1 + APPLE_MAX + SQUIRREL_MAX + BUILDINGS_MAX); // update all sprites    

	window_init();
	
	play_game();

	while(1);

	return 0;
}