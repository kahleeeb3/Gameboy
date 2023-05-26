#include <tonc.h> // GBA library
#include "animation.h"
#include "sprites.h"
#include "background.h"

// initialize global values
int skid = 0; // bool for if a player skid occurred
int showing_map = 0; // bool for if map is showing
int throwing = 0; // bool for if player is throwing


// helper functions
int get_center(Sprite *sprite)
{
	return (sprite->size/2) - 1;
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

int off_screen(Sprite *sprite)
{
    
	int l = sprite->x_pos;
	int r = sprite->x_pos + sprite->size - 1;
	int t = sprite->y_pos;
	int b = sprite->y_pos + sprite->size - 1;
	
	if( (l > SCREEN_WIDTH) || (r < 0) ){
		return 1;
	}
	else if( (t > SCREEN_HEIGHT) || (b < 0) ){
		return 1;
	}
	else{
		return 0;
	}
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

int collision(Sprite *s1, Sprite *s2, int hbx, int hby)
{
    
	// center of sprite 1
	int x1 = s1->x_pos + (s1->size/2) - 1;
	int y1 = s1->y_pos + (s1->size/2) - 1;

	// center of sprite 2
	int x2 = 0;
	int y2 = 0;
	
	
	// center of sprite 2 (non-buildings)
	if(s2->size != -1){
		x2 = s2->x_pos + (s2->size/2) - 1;
		y2 = s2->y_pos + (s2->size/2) - 1;
	}

	// for buildings we need to do something different
	else{
		x2 = s2->x_pos + (64) - 1;
		y2 = s2->y_pos;
	}

    // if within x range
	if( (x1 >= (x2-hbx) ) && (x1 <= (x2+hbx) ) ) 
	{
		// if within y range
		if( (y1 >= (y2-hby) ) && (y1 <= (y2+hby) ) )
		{
			return 1; // hit success
		}
	}

    return 0; // hit fail

}


// animations
void move_sprite(Sprite *sprite, int x_dir, int y_dir, int speed)
{
	sprite->x_pos += x_dir*speed;
	sprite->y_pos += y_dir*speed;
}

void move_towards(Sprite *sprite, Sprite *destination_sprite)
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

	sprite->x_pos += x_dir * move_x; // update x location
	sprite->y_pos += y_dir * move_y; // update y location

	// if headed right but facing left
	if(x_dir == 1 && sprite->dir_facing == -1)
	{
		sprite->mem_addr->attr1 ^= ATTR1_HFLIP;
		sprite->dir_facing = 1;
	}
	// headed left but facing right
	else if(x_dir == -1 && sprite->dir_facing == 1)
	{
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



// input functions
void shift_sprite_positions(int x_dir, int y_dir, int speed)
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

	shift_sprite_positions(x_dir, y_dir, speed);

}

void finalize_sprite_positions()
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
    if( key_held(KEY_A) )
    {
        throwing = 1; // set player as throwing
    }

    // DIRECTIONAL MOVEMENT
    int dx = key_tri_horz(); // tri-bool for x-axis movement
	int dy = key_tri_vert(); // tri-bool for y-axis movement
	skid = 0; // reset skid value


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

	// if moving in some direction
    if( (dx != 0) || (dy!= 0) )
	{
		if( skid != 1)
		{
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
		if(apple->active)
		{
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
				move_towards(squirrel, player); // move towards player
			}
			else{
				move_towards(squirrel, buildings[squirrel_target]); // move towards target
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
				if( apple->active )
				{
					// if apple collides with squirrel
					if( collision(apple, squirrel, squirrel->hbx, squirrel->hby) ){
						squirrel_kill_animation(squirrel); // kill squirrel
						apple_kill_animation(apple); // kill apple
					}
				}
			}

		}

	}
}