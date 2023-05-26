#include <tonc.h> // GBA library
#include "sprites.h" // functions for sprite control
#include "background.h" // functions for background control
#include "animation.h" // functions for player input and sprite animation
#include "helper.h"



int countdown_time = 3;
u32 time_elapsed;


void temp()
{
	// TEMPORARY
	spawn_squirrel(0);
	spawn_squirrel(1);
	spawn_squirrel(2);
	spawn_squirrel(3);
	spawn_squirrel(4);
	spawn_squirrel(5);
	spawn_squirrel(6);
	spawn_squirrel(7);
	spawn_squirrel(8);
	squirrels[0]->target = 1;
	squirrels[1]->target = 3;
	squirrels[2]->target = 5;
	squirrels[3]->target = 7;
	squirrels[4]->target = 9;
	squirrels[5]->target = 11;
	squirrels[6]->target = 13;
	squirrels[7]->target = 15;
	squirrels[8]->target = -1;

	squirrels[8]->pal_bank = 3;
}

void game_over()
{
	REG_DISPCNT ^= DCNT_OBJ; // Disable Objects
	REG_DISPCNT ^= DCNT_BG1; // disable background 1
	REG_DISPCNT ^= DCNT_BG2; // disable background 2
	REG_DISPCNT |= DCNT_WIN0; // enable window 0
	tte_printf("#{es;P:84,50}GAME OVER");
	tte_printf("#{P:65,62}You Survived %d Round", round_number);
	tte_printf("#{P:67,86}press A to try again.");
	
	key_wait_till_hit(KEY_A);

	REG_DISPCNT |= DCNT_OBJ; // enable Objects
	REG_DISPCNT |= DCNT_BG1; // enable background 1
	REG_DISPCNT |= DCNT_BG2; // enable background 2
	REG_DISPCNT ^= DCNT_WIN0; // disable window 0

	// reset scores
	player_score = 0;
	squirrel_count = 0;
	time_elapsed = timer_reset();
	countdown_time = 3;

	// remove all squirrels
	for(int i = 0; i<SQUIRREL_MAX; i++){
		if(squirrels[i]->active){
			squirrel_kill_animation(squirrels[i]);
			squirrels[i]->active = 0;
		}
	}

	// reset building health
	for(int i = 0; i<8; i++){
		building_health[i] = 100;
	}

}

void play_game()
{
	
	time_elapsed = timer_reset(); // reset timer

	while(1){
		
		if(player_score < 0){
			game_over();
		}

		// slow player input a little
		for (int i = 0; i < 4; i++){
			vid_vsync(); // wait for VBlank
			key_poll();	 // poll which keys are activated
		}

		// if the time elapsed has changed (every second)
		if( time_change(time_elapsed) ){
			time_elapsed = timer_update();
			minutes = (time_elapsed % 3600) / 60; // Convert the time to minutes
            seconds = time_elapsed % 60; // Convert the time to seconds
			update_text_display();

			if(countdown_time >= 0){
				tte_printf("#{P:117,12}%d", countdown_time);
				countdown_time--; // decrease countdown time
			}
			else if (countdown_time == -1){
				temp(); // spawn squirrels
				countdown_time--; // decrease countdown time
			}

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

	sound_init(); // configure sound
	timer_init(); // initialize timer
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