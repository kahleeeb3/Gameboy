#include "input.h"
#include <tonc.h>


//GENERAL ANIMATION FUNCTIONS
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

int off_screen(Sprite *sprite)
{
    if( (sprite->x_pos > SCREEN_WIDTH) || (sprite->x_pos < 0) )
    {
        return 1; // sprite is off screen
    }
    else
    {
        return 0; // sprite is on screen
    }
}

int collision(Sprite *s1, Sprite *s2, int hb)
{
    // center of sprite 1
    int x1 = s1->x_pos + (s1->size/2);
    int y1 = s1->y_pos + (s1->size/2);

    // center of sprite 2
    int x2 = s2->x_pos + (s2->size/2);
    int y2 = s2->y_pos + (s2->size/2);

    // if within x range
	if( (x1 >= (x2-hb) ) && (x1 <= (x2+hb) ) ) 
	{
		// if within y range
		if( (y1 >= (y2-hb) ) && (y1 <= (y2+hb) ) )
		{
			return 1; // hit success
		}
	}

    return 0; // hit fail

}

// APPLE ANIMATIONS
void apple_move_animation(Sprite *apple)
{
    apple->x_pos = (apple->x_pos + (apple->dir_facing * APPLE_SPEED) ); // update x position
    obj_set_pos(apple->mem_addr, apple->x_pos, apple->y_pos); // set position
}

void apple_throw_init(Sprite *apple, Sprite *player)
{
    // find player center
	int Px = player->x_pos + 15;
	int Py = player->y_pos + 14;

    apple->x_pos = Px-5; // update x location
	apple->y_pos = Py; // update y location

    obj_set_pos(apple->mem_addr, apple->x_pos, apple->y_pos); // set position
    apple->active = 1; // set as active (has been thrown)
    apple->dir_facing = player->dir_facing; // which way the apple is going
}

void apple_kill_animation(Sprite *apple)
{
    obj_set_pos(apple->mem_addr, apple->index *8 , 0); // move to static position
    apple->active = 0; // set as not active (not thrown yet)
}

// SQUIRREL ANIMATIONS
void squirrel_kill_animation(Sprite *squirrel)
{
    obj_hide(squirrel->mem_addr); // hide the sprite
    squirrel->active = 0; // set as not active (has been hit)
}

void squirrel_move_towards(Sprite *squirrel, Sprite *destination_sprite)
{
    // find squirrel center
	int Sx = squirrel->x_pos + 8;
	int Sy = squirrel->y_pos + 10;

    // find destination centerr
	int Px = destination_sprite->x_pos + (destination_sprite->size/2);
	int Py = destination_sprite->y_pos + (destination_sprite->size/2);

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
	obj_set_pos(squirrel->mem_addr, squirrel->x_pos, squirrel->y_pos);

	// if headed right but facing left
	if(x_dir == 1 && squirrel->dir_facing == -1)
	{
		squirrel->mem_addr->attr1 ^= ATTR1_HFLIP;
		squirrel->dir_facing = 1;
	}
	// headed left but facing right
	else if(x_dir == -1 && squirrel->dir_facing == 1)
	{
		squirrel->mem_addr->attr1 ^= ATTR1_HFLIP;
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
			squirrel_kill_animation(squirrel);
		}
	}


    
    walk_animation(squirrel, 4);

}


// ANIMATIONS FOR THE PLAYER SPRITE
void player_skid_animation(Sprite *player)
{
    player->mem_addr->attr2 = PLAYER_FRAME4; // switch to frame 4
    turn_animation(player); // flip the sprite        
}

void player_throw_animation(Sprite *player)
{
    player->mem_addr->attr2 = PLAYER_FRAME3; // change to frame 3
}


int move_player(Sprite *player)
{

    // if A is held
    if(key_held(KEY_A))
    {
        // returning at the beginning means the
        // player can't move until they release the
        // A button.
        player->throwing = 1; // set player as throwing
        return 0; // return 0 until A released
    }
    // if player has thrown
    else if(player->throwing == 1)
    {
        player_throw_animation(player);
        player->throwing = 0; // set as player not throwing anymore
        return 1; // let play function know throw took place
    }

    
    // if directional key is pressed
    if(key_tri_horz() || key_tri_vert())
    {
        // update player position based on key input
        player->x_pos += PLAYER_SPEED*key_tri_horz();
        player->y_pos += PLAYER_SPEED*key_tri_vert();

        // if left key held and player facing right
        if(key_held(KEY_LEFT) && (player->dir_facing == 1))
        {
                player_skid_animation(player); // player skid animation
        }

        // if right key held and player facing left
        else if(key_held(KEY_RIGHT) && (player->dir_facing == -1) )
        {
                player_skid_animation(player); // player skid animation
        }

        // if player is just walking
        else
        {
            walk_animation(player, 2); // player walk animation
        }

        



    }
    
    

    // update player animation
    obj_set_pos(player->mem_addr, player->x_pos, player->y_pos);

    return 0;
}