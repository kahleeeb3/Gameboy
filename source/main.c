#include <tonc.h> // GBA library
#include "sprites.h" // functions for sprite control

int main()
{
	// Enable video mode 0, enable objects, and set OBJ-VRAM as array
	REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;

	copy_sprite_data(); // move sprite data into obj_buffer
	set_player_attributes(); // set the attributes for the player sprite
	set_apple_attributes(); // set the attributes for the apple sprites
	set_squirrel_attributes(); // set the attributes for the squirrel sprites
	set_building_attributes(); // set the attributes for the building sprites

	oam_copy(oam_mem, obj_buffer, 128);
	
	while(1);

	return 0;
}