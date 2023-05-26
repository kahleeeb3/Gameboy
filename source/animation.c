#include <tonc.h> // GBA library
#include "animation.h"
#include "sprites.h"
#include "background.h"
#include "helper.h"

// initialize global values
int skid = 0; // bool for if a player skid occurred
int showing_map = 0; // bool for if map is showing
int throwing = 0; // bool for if player is throwing
int repairing = 0; // bool for if player is repairing building

// animations
void move_sprite(Sprite *sprite, int x_dir, int y_dir, int speed)
{
	sprite->x_pos += x_dir*speed;
	sprite->y_pos += y_dir*speed;
}

void move_towards(Sprite *sprite, Sprite *destination_sprite, int speed)
{
    // find squirrel center
	int Sx = sprite->x_pos + (sprite->size/2) - 1;
	int Sy = sprite->y_pos + (sprite->size/2) - 1;

    int Px = 0;
	int Py = 0;
	
	// find destination center
	if(destination_sprite->size != -1){
		Px = destination_sprite->x_pos + (destination_sprite->size/2) - 1;
		Py = destination_sprite->y_pos + (destination_sprite->size/2) - 1;
	}
	// for buildings we need to do something different
	else{
		Px = destination_sprite->x_pos + (64) - 1;
		Py = destination_sprite->y_pos;
	}
	

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

	sprite->x_pos += x_dir * move_x * speed; // update x location
	sprite->y_pos += y_dir * move_y * speed; // update y location

	// if headed right but facing left
	if(x_dir == 1 && sprite->dir_facing == -1){
		sprite->mem_addr->attr1 ^= ATTR1_HFLIP;
		sprite->dir_facing = 1;
	}
	// headed left but facing right
	else if(x_dir == -1 && sprite->dir_facing == 1){
		sprite->mem_addr->attr1 ^= ATTR1_HFLIP;
		sprite->dir_facing = -1;
	}
    
}

void walk_animation(Sprite *sprite, int end_frame)
{
    // get start_tile from spriteStruct
	int st = sprite->start_tile;
	// get curr_tile from spriteStruct
	int cf = sprite->curr_tile;
	// get size from spriteStruct
	int s = sprite->size;
	// get pal_bank from spriteStruct
	int pb = sprite->pal_bank;
    // number of tiles that make up a single frame
    int tpf = (s*s)/(8*8);

	// update attribute 2
	sprite->mem_addr->attr2 = ATTR2_PALBANK(pb) | (st + (tpf * cf)) | ATTR2_PRIO(1);
	cf = (cf + 1) % end_frame;

	//update curr_tile
	sprite->curr_tile = cf;
}

void turn_animation(Sprite *sprite)
{
    // flip the sprite
    sprite->mem_addr->attr1 ^= ATTR1_HFLIP;

    // change the sprite facing direction
    if(sprite->dir_facing == 1){
        sprite->dir_facing = -1;
	}
    else{
        sprite->dir_facing = 1;
	}
}

void player_skid_animation(Sprite *player)
{
    player->mem_addr->attr2 = ATTR2_PALBANK(PLAYER_PAL_BANK) | (PLAYER_TILE_ID + 48) | ATTR2_PRIO(1);
    turn_animation(player); // flip the sprite        
}

void apple_throw_init(Sprite *apple)
{
    // find player center
	int Px = player->x_pos + (player->size/2) - 1;
	int Py = player->y_pos + (player->size/2) - 1;

    apple->x_pos = Px - (apple->size/2) - 1; // update x location
	apple->y_pos = Py - (apple->size/2) - 1 + 5; // update y location

    apple->active = 1; // set as active (has been thrown)
    apple->dir_facing = player->dir_facing; // which way the apple is going
}

void spawn_squirrel(int index)
{
	// the squirrels are going to have to spawn relative to some
	// stationary object on the background (a building)
	// ill simply choose the bottom half of hays for this
	int bx = buildings[3]->x_pos; // building x pos
	int by = buildings[3]->y_pos; // building y pos
	
	Sprite *squirrel = squirrels[index];
	int x = qran_range(bx, bx+310); // choose some x pos
	int y = qran_range(by+85, by+170); // choose some y pos
	squirrel->x_pos = x;
	squirrel->y_pos = y;
	obj_unhide(squirrel->mem_addr, ATTR0_4BPP);
	squirrel->active = 1;
}

void apple_kill_animation(Sprite *apple)
{
	apple->active = 0;
	apple->x_pos = (apple->index * 8) + 2;
	apple->y_pos = 24;
}

void squirrel_kill_animation(Sprite *squirrel)
{
    obj_hide(squirrel->mem_addr); // hide the sprite
    squirrel->active = 0; // set as not active (has been hit)
}

void building_kill_animation(int building_index)
{
    Sprite *top_half = buildings[building_index-1];
	Sprite *bottom_half = buildings[building_index];
	
	obj_hide(top_half->mem_addr); // hide the sprite
	obj_hide(bottom_half->mem_addr); // hide the sprite
    top_half->active = 0; // set as not active (has been hit)
    bottom_half->active = 0; // set as not active (has been hit)

}

// input functions
void shift_sprite_positions(int x_dir, int y_dir, int speed)
{
    // move all active apples
	for (int i = 0; i < APPLE_MAX; i++){
		Sprite *apple = apples[i];
		if(apple->active == 1){
			move_sprite(apple, x_dir * -1, y_dir * -1, speed);
		}

	}
	
	// move all active squirrels
	for(int i=0; i<SQUIRREL_MAX; i++){
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
	for(int i=0; i<BUILDINGS_MAX; i++){
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

	shift_sprite_positions(x_dir, y_dir, speed);

}

void input()
{
	// if R key is down and map is not showing
    if( key_is_down(KEY_R) && showing_map == 0 ){
        REG_DISPCNT |= DCNT_WIN0; // enable window 0 (show mini map)
        showing_map = 1;
    }

    // if key is not down and map is showing
    else if ( key_is_up(KEY_R) && showing_map == 1 ){
        REG_DISPCNT &= ~DCNT_WIN0; // disable window 0 (hide mini map)
        showing_map = 0;
    }

    // if A is held
    if( key_held(KEY_A) ){
        throwing = 1; // set player as throwing
    }

	if( key_held(KEY_B) ){
		repairing = 1; // set player as repairing
	}
	else{
		repairing = 0; // set player as not repairing
	}

	/* ===================================================================
	 * DIRECTIONAL MOVEMENT
	 * This is a lengthy one. it can probably be condensed at some point
	 * however, the only way I could get it to work effectively was typing
	 * it out in this rather ugly fashion. enjoy :)
	 =================================================================== */
    int dx = key_tri_horz(); // tri-bool for x-axis movement
	int dy = key_tri_vert(); // tri-bool for y-axis movement
	skid = 0; // reset skid value


    // player moving diagonal
	if ( (dx != 0)  && (dy!= 0) ){

		int quadrant = get_rel_diag_pos(player);

		// Player moving NE
		if ( dx == 1 && dy == -1){
			if(player->dir_facing == -1){
				player_skid_animation(player);
				skid = 1;
			}
			if ( quadrant == 3 ){
				move_sprite(player, dx, dy, PLAYER_SPEED);
			}
			else{
				// if you can move the map right
				if ( map->x_pos < MAP_MAX_X ){
					move_map(map, dx, 0, PLAYER_SPEED);
				}
				else if (player->x_pos < PLAYER_MAX_X){
					move_sprite(player, dx, 0, PLAYER_SPEED);
				}
				// if you can move the map up
				if ( map->y_pos > 0){
					move_map(map, 0, dy, PLAYER_SPEED);
				}
				else if (player->y_pos > PLAYER_MIN_Y){
					move_sprite(player, 0, dy, PLAYER_SPEED);
				}
			}

		}

		// Player moving SE
		else if ( dx == 1 && dy == 1){
			if(player->dir_facing == -1){
				player_skid_animation(player);
				skid = 1;
			}
			if ( quadrant == 2 ){
				move_sprite(player, dx, dy, PLAYER_SPEED);
			}
			else{
				// if you can move the map right
				if ( map->x_pos < MAP_MAX_X ){
					move_map(map, dx, 0, PLAYER_SPEED);
				}
				else if (player->x_pos < PLAYER_MAX_X){
					move_sprite(player, dx, 0, PLAYER_SPEED);
				}
				// if you can move the map down
				if ( map->y_pos < MAP_MAX_Y ){
					move_map(map, 0, dy, PLAYER_SPEED);
				}
				else if (player->y_pos < PLAYER_MAX_Y){
					move_sprite(player, 0, dy, PLAYER_SPEED);
				}
			}
		}

		// Player moving NW
		else if ( dx == -1 && dy == -1){
			if(player->dir_facing == 1){
				player_skid_animation(player);
				skid = 1;
			}
			if ( quadrant == 4 ){
				move_sprite(player, dx, dy, PLAYER_SPEED);
			}
			else{
				// if you can move the map left
				if ( map->x_pos > 0 ){
					move_map(map, dx, 0, PLAYER_SPEED);
				}
				else if (player->x_pos > PLAYER_MIN_X){
					move_sprite(player, dx, 0, PLAYER_SPEED);
				}
				// if you can move the map up
				if ( map->y_pos > 0 ){
					move_map(map, 0, dy, PLAYER_SPEED);
				}
				else if (player->y_pos > PLAYER_MIN_Y){
					move_sprite(player, 0, dy, PLAYER_SPEED);
				}
			}
			
		}

		// Player moving SW
		else if ( dx == -1 && dy == 1){
			if(player->dir_facing == 1){
				player_skid_animation(player);
				skid = 1;
			}
			if ( quadrant == 1 ){
				move_sprite(player, dx, dy, PLAYER_SPEED);
			}
			else{
				// if you can move the map left
				if ( map->x_pos > 0 ){
					move_map(map, dx, 0, PLAYER_SPEED);
				}
				else if (player->x_pos > PLAYER_MIN_X){
					move_sprite(player, dx, 0, PLAYER_SPEED);
				}
				// if you can move the map down
				if ( map->y_pos < MAP_MAX_Y ){
					move_map(map, 0, dy, PLAYER_SPEED);
				}
				else if (player->y_pos < PLAYER_MAX_Y){
					move_sprite(player, 0, dy, PLAYER_SPEED);
				}
			}
			
		}
	}

	// player moving in x direction
	else if ( dx != 0 ){

		// player moving right
		if ( dx == 1){
			
			if(player->dir_facing == -1){
				player_skid_animation(player);
				skid = 1;
			}
			
			// player is left of the center
			if( get_rel_x_pos(player) == -1 ){
				move_sprite(player, dx, 0, PLAYER_SPEED);
			}

			// map is not FULLY to the right
			else if (map->x_pos < MAP_MAX_Y){
				move_map(map, dx, 0, PLAYER_SPEED);
			}
			else if ( player->x_pos < PLAYER_MAX_X ){
				move_sprite(player, dx, 0, PLAYER_SPEED);
			}
		}

		// player moving left
		else{
			
			if(player->dir_facing == 1){
				player_skid_animation(player);
				skid = 1;
			}
			
			// player is right of the center
			if( get_rel_x_pos(player) == 1 ){
				move_sprite(player, dx, 0, PLAYER_SPEED);
			}

			// map is not FULLY to the left
			else if (map->x_pos > 0){
				move_map(map, dx, 0, PLAYER_SPEED);
			}
			else if ( player->x_pos > PLAYER_MIN_X ){
				move_sprite(player, dx, 0, PLAYER_SPEED);
			}
		}
	}

	// player moving in y direction
	else if ( dy != 0 ){
		// player moving up
		if ( dy == -1){
			// player is below of the center
			if( get_rel_y_pos(player) == -1 ){
				move_sprite(player, 0, dy, PLAYER_SPEED);
			}

			// map is not FULLY to the top
			else if (map->y_pos > 0){
				move_map(map, 0, dy, PLAYER_SPEED);
			}
			else if( player-> y_pos > PLAYER_MIN_Y){
				move_sprite(player, 0, dy, PLAYER_SPEED);
			}
		}

		// player moving down
		else{
			// player is above of the center
			if( get_rel_y_pos(player) == 1 ){
				move_sprite(player, 0, dy, PLAYER_SPEED);
			}

			// map is not FULLY to the bottom
			else if (map->y_pos < MAP_MAX_Y){
				move_map(map, 0, dy, PLAYER_SPEED);
			}
			else if( player->y_pos < PLAYER_MAX_Y ){
				move_sprite(player, 0, dy, PLAYER_SPEED);
			}
		}
	}

	// if moving in some direction
    if( (dx != 0) || (dy!= 0) ){
		if( skid != 1){
			walk_animation(player, 2);
		}
	}

}

// main simulation
void update_sprite_positions()
{
	
	// for each apple sprite
	for (int i = 0; i < APPLE_MAX; i++){
		Sprite *apple = apples[i]; // define selected apple

		// if apple is active
		if(apple->active){
			move_sprite(apple, apple->dir_facing, 0, APPLE_SPEED); // move the apple

			// // if the apple is off the screen
			if( off_screen(apple) )
			{
				apple_kill_animation(apple);
			}
		}

		// if not active but player is throwing
		else if (throwing && key_is_up(KEY_A)){
			apple_throw_init(apple); // throw the apple
			throwing = 0;
		}
	}
	
	
	// for each squirrel sprite
	for(int i=0; i<SQUIRREL_MAX; i++){
		Sprite *squirrel = squirrels[i]; // define selected squirrel

		// if the squirrel is active
		if( squirrel->active ){


			// if it was off screen but now is not
			if(!squirrel->hidden){
				if(off_screen(squirrel) == 0){
					obj_unhide(squirrel->mem_addr, ATTR1_SIZE_16);
				}
			}

			walk_animation(squirrel, 4); // walk animation

			int squirrel_target = squirrel->target; // determine target
			
			// if the target is the player
			if (squirrel_target == -1){
				move_towards(squirrel, player, SQUIRREL_SPEED); // move towards player
			}
			else{
				move_towards(squirrel, buildings[squirrel_target], SQUIRREL_SPEED); // move towards target
			}

		}

	}
}

void check_sprite_collisions()
{
	// for each squirrel sprite
	for(int i=0; i<SQUIRREL_MAX; i++){
		Sprite *squirrel = squirrels[i]; // define selected squirrel

		// if the squirrel is active
		if( squirrel->active ){

			// for each apple sprite
			for (int i = 0; i < APPLE_MAX; i++){
				Sprite *apple = apples[i]; // define selected apple

				// if apple is active
				if( apple->active ){
					// if apple collides with squirrel
					if( collision(apple, squirrel, squirrel->hbx, squirrel->hby) ){
						squirrel_kill_animation(squirrel); // kill squirrel
						apple_kill_animation(apple); // kill apple
						player_score++; // increase player score
						squirrel_count--; // decrease squirrel count
					}
				}
			}

			// for each building: check if squirrel is hitting it
			for( int i = 0; i <BUILDINGS_MAX; i++){
				// if its the second half of the building
				if( i%2 ){
					Sprite *building = buildings[i]; // select the building

					// if squirrel is hitting the building it is targeting
					if( collision(squirrel, building, building->hbx, building->hby) && squirrel->target == i){
						move_towards(squirrel, building, SQUIRREL_SPEED*-1); // negate the move forward (stay still)
						if( building_health[i/2] < 0 ){
							building_kill_animation(i); // hide the building
							squirrel->target = -1; // have squirrel attack player
							squirrel->pal_bank = 3;
						}
						building_health[i/2] -= 0.01;
					}
				}
			}

			// if the squirrel is hitting a player and is targeting the player
			if( collision(squirrel, player, player->hbx, player->hby) && squirrel->target == -1 ){
				player_score -=5;
				squirrel_kill_animation(squirrel);
			}

		}

	}

	// for each building: check if player is on it
	for( int i = 0; i <BUILDINGS_MAX; i++){
		// if its the second half of the building
		if( i%2 ){
			Sprite *building = buildings[i]; // select the building
			// if the player is on this building and repairing
			if( collision(player, building, building->hbx, building->hby) && repairing){
				if( (player_score > -1) && (building_health[i/2] < 100) ){
					building_health[i/2] +=  0.06;
					player_score -= 0.06;
				}
			}
		}
	}

}