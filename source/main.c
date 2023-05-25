#include <tonc.h> // GBA library
#include "sprites.h" // functions for sprite control
#include "background.h" // functions for background control

int main()
{
	// Enable video mode 0, enable objects, and set OBJ-VRAM as array
	REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;

	// sound_init(); // configure sound
	loading_screen_init();
	loading_screen_display();

	/*
	sprites_init(); // move sprite data into obj_buffer
	set_player_attributes(); // set the attributes for the player sprite
	set_apple_attributes(); // set the attributes for the apple sprites
	set_squirrel_attributes(); // set the attributes for the squirrel sprites
	set_building_attributes(); // set the attributes for the building sprites
	oam_copy(oam_mem, obj_buffer, 128); // copy sprite data from buffer to system mem


	// enable backgrounds
	REG_DISPCNT |= DCNT_BG0 | DCNT_BG1| DCNT_BG2;

	text_init(); // enable the text display on background 0
	map_init(); // copy background 1 data and create map struct
	mini_map_init(); // copy background 2 data
	*/
	
	while(1);

	return 0;
}