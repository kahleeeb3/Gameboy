#include "animations.h"


void move_sprite(Sprite *sprite, int x_dir, int y_dir, int speed)
{
	sprite->x_pos += x_dir*speed;
	sprite->y_pos += y_dir*speed;
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
	sprite->mem_addr->attr2 = ATTR2_PALBANK(pb) | (st + (tpf * cf)) | ATTR2_PRIO(1);
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
    player->mem_addr->attr2 = ATTR2_PALBANK(PLAYER_PAL_BANK) | PLAYER_FRAME4 | ATTR2_PRIO(1);
    turn_animation(player); // flip the sprite        
}

void squirrel_kill_animation(Sprite *squirrel)
{
    obj_hide(squirrel->mem_addr); // hide the sprite
    squirrel->active = 0; // set as not active (has been hit)
}

void apple_kill_animation(Sprite *apple)
{
	apple->active = 0;
	apple->x_pos = (apple->index * 8) + 2;
	apple->y_pos = 24;
}

void spawn_squirrel(int index, Sprite** squirrels)
{
	Sprite *squirrel = squirrels[index];
	int x = rand() % (SCREEN_WIDTH + 1);
	int y = rand() % (SCREEN_HEIGHT + 1);
	// int x = qran_range(100, 400);
	// int y = qran_range(200, 280);
	squirrel->x_pos = x;
	squirrel->y_pos = y;
	obj_unhide(squirrel->mem_addr, ATTR0_4BPP);
	squirrel->active = 1;
}

void apple_throw_init(Sprite *apple, Sprite *player)
{
    // find player center
	int Px = player->x_pos + (player->size/2) - 1;
	int Py = player->y_pos + (player->size/2) - 1;

    apple->x_pos = Px - (apple->size/2) - 1; // update x location
	apple->y_pos = Py - (apple->size/2) - 1 + 5; // update y location

    apple->active = 1; // set as active (has been thrown)
    apple->dir_facing = player->dir_facing; // which way the apple is going
}

void squirrel_bump_animation(Sprite *squirrel, Sprite *building)
{

	int bump_factor = 15; // how big the bump is

	/*
	 * This will be very similar to the move towards
	 * function. This is because we need to know in what
	 * direction to move the squirrel backwards after it
	 * bumps into a building.
	 */
	
	// find squirrel center
	int Sx = squirrel->x_pos + (squirrel->size/2) - 1;
	int Sy = squirrel->y_pos + (squirrel->size/2) - 1;

	int Bx = building->x_pos + (64) - 1;
	int By = building->y_pos;

	 // find x and y distance between them
	int dx = Bx - Sx;
	int dy = By - Sy;

	// determine direction to move
	int x_dir = 1; // pos or neg 1
	int y_dir = 1; // pos or neg 1
	if(dx < 0)
		x_dir = -1;
	if(dy < 0)
		y_dir = -1;


	// how much to move in what direction
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

	squirrel->x_pos -= bump_factor * x_dir * move_x; // update x location
	squirrel->y_pos -= bump_factor * y_dir * move_y; // update y location

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