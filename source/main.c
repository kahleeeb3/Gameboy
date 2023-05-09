#include <string.h>
#include <tonc.h>
#include "player.h"

OBJ_ATTR obj_buffer[128]; // allocate space for 128 sprites
OBJ_ATTR *player= &obj_buffer[0]; // define the player sprite


void input()
{

	while(1)
	{
		vid_vsync(); // wait for VBlank
		key_poll(); // poll user input
			
		u16 frame1 = 0; // first frame
		u16 frame2 = 16; // second frame
		u16 frame3 = 32; // third frame
		u16 frame4 = 48; // fourth frame
		
		if(key_hit(KEY_LEFT))
			player->attr2 = frame1;
			
		if(key_hit(KEY_RIGHT))
			player->attr2 = frame2;

		if(key_hit(KEY_UP))
			player->attr2 = frame3;

		if(key_hit(KEY_DOWN))
			player->attr2 = frame4;
		
		if(key_hit(KEY_A))
			player->attr2 = (player->attr2 + 16) % 64;
		
		oam_copy(oam_mem, obj_buffer, 1);	// update 1 sprite in obj_buffer

	}
}



int main()
{
	
	oam_init(obj_buffer, 128); // initialize 128 sprites

	// Display =   Mode 0| Background 0| Enable Objects | OBJ-VRAM as array
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
	
	// create sprite
	memcpy(&tile_mem[4][0], playerTiles, playerTilesLen); // copy sprite tiles
    memcpy(pal_obj_mem, playerPal, playerPalLen); // copy sprite pallets
	
	obj_set_attr(player, 
		ATTR0_4BPP, // set as 4bpp
		ATTR1_SIZE_32, // 32x32 pixel sprite
		0);

	obj_set_pos(player, 96, 32);
	
	
	// update 1 sprite of data in OAM
	oam_copy(oam_mem, obj_buffer, 1);
	
	// while(1);

	// take input
	input();

	return 0;
}