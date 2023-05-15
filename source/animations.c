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