#include <string.h>
#include <tonc.h>
#include "player.h"
#include "apple.h"
#include "squirrel.h"
#include <stdbool.h>


#define PLAYER_SPEED 5
#define PLAYER_FRAME1 0
#define PLAYER_FRAME2 16
#define PLAYER_FRAME3 32
#define PLAYER_FRAME4 48

int squirrel_frame = 0;


OBJ_ATTR obj_buffer[128]; // allocate space for 128 sprites
OBJ_ATTR *player= &obj_buffer[0]; // define the player sprite
OBJ_ATTR *apple = &obj_buffer[1]; // define the apple sprite
OBJ_ATTR *squirrel = &obj_buffer[2]; // define the squirrel sprite

int x= 96, y= 32; // default player position
int apple_x = 0, apple_y = 0; // apple position
bool apple_thrown = false;
int frame=0; // frame count to slow polling

void apple_move(){
	// put apple behind player
	apple_x = x+20;
	apple_y = y+15;

	obj_set_pos(apple, x+10, y+15); // set apple position
	oam_copy(oam_mem, obj_buffer, 2); // update buffer
	obj_unhide(apple, ATTR0_4BPP); // unhide apple
	apple_thrown = true;
}


void player_throw(){
	player->attr2 = PLAYER_FRAME3; // change to frame 3
	oam_copy(oam_mem, obj_buffer, 1); // update sprite
	// player->attr2 = PLAYER_FRAME4; // change to frame 1
	apple_move(); // create an apple animation
}

void player_walk(){
	player->attr2 = (player->attr2 + 16) % 32; // alternate frames 1&2
}

void player_skid(){
	// change to frame 4 and then back to frame 1 (skid effect)
	player->attr2 = PLAYER_FRAME4; // change to frame 4
	oam_copy(oam_mem, obj_buffer, 1); // update sprite
	player->attr1 ^= ATTR1_HFLIP; // horizontal flip sprite
	player->attr2 = PLAYER_FRAME4; // change to frame 1
}

void input()
{

	while(1)
	{
		// slowing down polling to make the changes visible
        for (int i = 0; i < 4; i++)
        {
			vid_vsync(); // wait for VBlank
			key_poll(); // poll user input
		}

		// move squirrel
		// 66, 74, 82, 90
		squirrel->attr2 = ATTR2_PALBANK(2) | (66 + 4*squirrel_frame);
		squirrel_frame = (squirrel_frame+1)%4;
		// squirrel->attr2 = ((squirrel->attr2 + 8) % 32);

		// if apple thrown, move in
		if(apple_thrown){
			apple_x = (apple_x+PLAYER_SPEED+2) % (SCREEN_WIDTH);
			obj_set_pos(apple, apple_x, apple_y);
			oam_copy(oam_mem, obj_buffer, 2); // update buffer
		}

		// if press A
		if(key_is_down(KEY_A)){
			player_throw();
		}

		// update position data
		x += PLAYER_SPEED*key_tri_horz();
		y += PLAYER_SPEED*key_tri_vert();
		
		// if facing right: ch1->attr1 = 0x0000
		bool facing_right = (player->attr1 & ATTR1_HFLIP) == 0x0000;
		bool facing_left = !facing_right;

		// if holding up or down, alternate first two sprite frames
		if(key_held(KEY_DOWN) || key_held(KEY_UP)){
			player_walk(); // alternate frames
		}

		// if moving left
		if(key_held(KEY_LEFT)){
			if(facing_right){
				player_skid(); // skid animation
			}
			else{
				player_walk(); // walk animation
			}
		}
		
		// if moving right
		if(key_held(KEY_RIGHT)){
			if(facing_left){
				player_skid(); // skid animation
			}
			else{
				player_walk(); // walk animation
			}
		}


		// diagonal movements
		if( (key_held(KEY_DOWN) && key_held(KEY_RIGHT)) || (key_held(KEY_UP) && key_held(KEY_RIGHT)) || 
			(key_held(KEY_DOWN) && key_held(KEY_LEFT)) || (key_held(KEY_UP) && key_held(KEY_LEFT)) ){
				player_walk(); // walk animation
			}

		obj_set_pos(player, x, y); // set player position
		oam_copy(oam_mem, obj_buffer, 3);	// update 1 sprite in obj_buffer

	}
}

void create_player_sprite(){
	// starts at title 0, uses palette bank 0, 0 palettes before this one
	u32 tid= 0 , pb= 0, poff = 0;

	// copy data to oam buffer
	memcpy(&tile_mem[4][tid], playerTiles, playerTilesLen); // copy sprite tiles
    memcpy(pal_obj_mem+poff, playerPal, playerPalLen); // copy sprite pallets

	// set object attributes
	obj_set_attr(player,
		ATTR0_4BPP, // 4 bytes per pixel
		ATTR1_SIZE_32, // 32x32 pixel sprite
		ATTR2_PALBANK(pb) | tid); // set palette bank and title id

	obj_set_pos(player, x, y); // set position

}

void create_apple_sprite(){
	// starts at title 64, uses palette bank 1, 16 palettes before this one
	u32 tid= 64 , pb= 1, poff = 16;

	// copy data to oam buffer
	memcpy(&tile_mem[4][tid], appleTiles, appleTilesLen); // copy sprite tiles into local storage
    memcpy(pal_obj_mem+poff, applePal, applePalLen); // copy sprite pallets

	// set object attributes
	obj_set_attr(apple,
		ATTR0_4BPP, // 4 bytes per pixel
		ATTR1_SIZE_8, // 8x8 pixel sprite
		ATTR2_PALBANK(pb) | tid); // set palette bank and title id

	obj_set_pos(apple, x+10, y+15); // set position
}

void create_squirrel_sprite(){
	// starts at title 66, uses palette bank 2, 32 palettes before this one
	u32 tid= 66 , pb= 2, poff = 32;
	int sx = 30, sy = 30; // initial position

	// copy data to oam buffer
	memcpy(&tile_mem[4][tid], squirrelTiles, squirrelTilesLen); // copy sprite tiles into local storage
    memcpy(pal_obj_mem+poff, squirrelPal, squirrelPalLen); // copy sprite pallets

	// set object attributes
	obj_set_attr(squirrel,
		ATTR0_4BPP, // 4 bytes per pixel
		ATTR1_SIZE_16, // 16x16 pixel sprite
		ATTR2_PALBANK(pb) | tid); // set palette bank and title id

	obj_set_pos(squirrel, sx, sy); // set position
}

int main()
{
	oam_init(obj_buffer, 128); // initialize 128 sprites
	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D; // Enable Objects & OBJ-VRAM as array
	create_player_sprite(); // initialize player
	create_apple_sprite(); // initialize apple
	create_squirrel_sprite(); // initialize squirrel
	obj_hide(apple); // hide the apple
	oam_copy(oam_mem, obj_buffer, 3); // copy data from OAM buffer to real OAM

	input(); // get input
	while(1);
	return 0;
}