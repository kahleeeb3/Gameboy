#include <tonc.h> // GBA library
#include "sprites.h" // functions for sprite control
#include "background.h" // functions for background control
#include "animation.h" // functions for player input and sprite animation


void play_game()
{
	// TEMPORARY
	squirrels[0]->active = 1;
	squirrels[0]->target = -1;
	obj_unhide(squirrels[0]->mem_addr, ATTR1_SIZE_16);

	while(1){
		
		// slow player input a little
		for (int i = 0; i < 4; i++)
		{
			vid_vsync(); // wait for VBlank
			key_poll();	 // poll which keys are activated
		}

		input(); // get player input and move character
		update_sprite_positions(); // move sprites
		check_sprite_collisions(); // evaluate sprite collisions
		finalize_sprite_positions(); // update all sprite positions
		oam_copy(oam_mem, obj_buffer, 128); // copy sprite data from buffer to system mem

		
	}
}

int main()
{
	// Enable video mode 0, enable objects, and set OBJ-VRAM as array
	REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;

	// sound_init(); // configure sound
	// loading_screen_init(); // load data for loading screen
	// loading_screen_display(); // run loading screen animation

	// update the sprite data
	sprites_init(); // move sprite data into obj_buffer
	set_player_attributes(); // set the attributes for the player sprite
	set_apple_attributes(); // set the attributes for the apple sprites
	set_squirrel_attributes(); // set the attributes for the squirrel sprites
	set_building_attributes(); // set the attributes for the building sprites
	oam_copy(oam_mem, obj_buffer, 128); // copy sprite data from buffer to system mem

	// update the map data
	text_init(); // enable the text display on background 0
	map_init(); // copy background 1 data and create map struct
	mini_map_init(); // copy background 2 data for mini map
	window_init(); // enable window views

	// enable objs and backgrounds
	REG_DISPCNT |= DCNT_OBJ | DCNT_BG0 | DCNT_BG1| DCNT_BG2;
	
	play_game(); // play the actual game

	return 0;
}