#include <string.h>
#include <tonc.h>
#include "player.h"
#include <stdbool.h>


#define PLAYER_SPEED 5
#define PLAYER_FRAME1 0
#define PLAYER_FRAME2 16
#define PLAYER_FRAME3 32
#define PLAYER_FRAME4 48


OBJ_ATTR obj_buffer[128]; // allocate space for 128 sprites
OBJ_ATTR *player= &obj_buffer[0]; // define the player sprite

int x= 96, y= 32; // default player position
int frame=0; // frame count to slow polling


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


		// update position data
		x += PLAYER_SPEED*key_tri_horz();
		y += PLAYER_SPEED*key_tri_vert();
		

		// if facing right: ch1->attr1 = 0x0000
		bool facing_right = (player->attr1 & ATTR1_HFLIP) == 0x0000;
		bool facing_left = !facing_right;

		// if holding up or down, alternate first two sprite frames
		if(key_held(KEY_DOWN) || key_held(KEY_UP)){
			player->attr2 = (player->attr2 + 16) % 32; // alternate frames
		}



		// if moving left
		if(key_held(KEY_LEFT)){
			if(facing_right){
				// change to frame 4 and then back to frame 1 (skid effect)
				player->attr2 = PLAYER_FRAME4; // change to frame 4
				oam_copy(oam_mem, obj_buffer, 1); // update sprite
				player->attr1 ^= ATTR1_HFLIP; // horizontal flip sprite
				player->attr2 = PLAYER_FRAME4; // change to frame 1
			}
			else{
				player->attr2 = (player->attr2 + 16) % 32; // alternate frames
			}
		}
		
		// if moving right
		if(key_held(KEY_RIGHT)){
			if(facing_left){
				// change to frame 4 and then back to frame 1 (skid effect)
				player->attr2 = PLAYER_FRAME4; // change to frame 4
				oam_copy(oam_mem, obj_buffer, 1); // update sprite
				player->attr1 ^= ATTR1_HFLIP; // horizontal flip sprite
				player->attr2 = PLAYER_FRAME4; // change to frame 1
			}
			else{
				player->attr2 = (player->attr2 + 16) % 32; // alternate frames
			}
		}


		obj_set_pos(player, x, y);
		oam_copy(oam_mem, obj_buffer, 1);	// update 1 sprite in obj_buffer


	}
}


void create_player_sprite(){
	memcpy(&tile_mem[4][0], playerTiles, playerTilesLen); // copy sprite tiles
    memcpy(pal_obj_mem, playerPal, playerPalLen); // copy sprite pallets
	obj_set_attr(player, ATTR0_4BPP, ATTR1_SIZE_32,0); // set attributes
	obj_set_pos(player, x, y); // set position
	oam_copy(oam_mem, obj_buffer, 1);	// update 1 sprite in obj_buffer

}


int main()
{
	oam_init(obj_buffer, 128); // initialize 128 sprites
	REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D; // Enable Objects & OBJ-VRAM as array
	create_player_sprite(); // create player
	input(); // get input
	while(1);
	return 0;
}