#include <tonc.h> // GBA library
#include "sprites.h" // functions for sprite control
#include "background.h" // functions for background control
#include "animation.h" // functions for player input and sprite animation
#include "helper.h"



int countdown_time = 3;
u32 time_elapsed;
int squirrels_attacking_player;
int squirrels_attacking_building[8];

void set_round_data(int round_number)
{
	switch (round_number){
		case 1: // one squirrel attack player
			squirrels_attacking_player = 1;
			squirrels_attacking_building[0] = 0;
			squirrels_attacking_building[1] = 0;
			squirrels_attacking_building[2] = 0;
			squirrels_attacking_building[3] = 0;
			squirrels_attacking_building[4] = 0;
			squirrels_attacking_building[5] = 0;
			squirrels_attacking_building[6] = 0;
			squirrels_attacking_building[7] = 0;
			break;
		case 2:
			squirrels_attacking_player = 0;
			squirrels_attacking_building[0] = 1;
			squirrels_attacking_building[1] = 1;
			squirrels_attacking_building[2] = 1;
			squirrels_attacking_building[3] = 0;
			squirrels_attacking_building[4] = 0;
			squirrels_attacking_building[5] = 0;
			squirrels_attacking_building[6] = 0;
			squirrels_attacking_building[7] = 0;
			break;
		case 3:
			squirrels_attacking_player = 5;
			squirrels_attacking_building[0] = 1;
			squirrels_attacking_building[1] = 1;
			squirrels_attacking_building[2] = 1;
			squirrels_attacking_building[3] = 1;
			squirrels_attacking_building[4] = 1;
			squirrels_attacking_building[5] = 1;
			squirrels_attacking_building[6] = 1;
			squirrels_attacking_building[7] = 1;
			break;
		case 4:
			squirrels_attacking_player = 20;
			squirrels_attacking_building[0] = 3;
			squirrels_attacking_building[1] = 3;
			squirrels_attacking_building[2] = 3;
			squirrels_attacking_building[3] = 3;
			squirrels_attacking_building[4] = 3;
			squirrels_attacking_building[5] = 3;
			squirrels_attacking_building[6] = 3;
			squirrels_attacking_building[7] = 3;
			break;
		default:
			squirrels_attacking_player = qran_range(0, 5);
			squirrels_attacking_building[0] = qran_range(0, 5);
			squirrels_attacking_building[1] = qran_range(0, 5);
			squirrels_attacking_building[2] = qran_range(0, 5);
			squirrels_attacking_building[3] = qran_range(0, 5);
			squirrels_attacking_building[4] = qran_range(0, 5);
			squirrels_attacking_building[5] = qran_range(0, 5);
			squirrels_attacking_building[6] = qran_range(0, 5);
			squirrels_attacking_building[7] = qran_range(0, 5);
			break;
	}
}

void assign_squirrel_targets()
{
	// for each value in that list
	for(int target_index = 0; target_index < 8; target_index++){

		int selected_value = squirrels_attacking_building[target_index]; // select the value
		int building_index = (target_index * 2) + 1; // select target building index
		
		// count up from 0 to that value
		for(int i=0; i< selected_value; i++){
			Sprite *squirrel = squirrels[squirrel_count]; // select squirrel
			squirrel->target = building_index; // assign it a target
			spawn_squirrel(squirrel_count); // spawn it in
			squirrel_count++; // increase squirrel count
			squirrel->pal_bank = 2; // change squirrel color
		}
	}

	// assign squirrels to the player
	for(int i =0; i<squirrels_attacking_player; i++){
		Sprite *squirrel = squirrels[squirrel_count]; // select squirrel
		squirrel->target = -1; // assign it a target (-1 for player)
		spawn_squirrel(squirrel_count); // spawn it in
		squirrel_count++; // increase squirrel count
		squirrel->pal_bank = 3; // set squirrel color
	}


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
	round_number = 1;
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
				set_round_data(round_number);
				assign_squirrel_targets(); // spawn squirrels
				countdown_time--; // decrease countdown time
			}

		}

		// if player died
		if(player_score < 0){
			game_over();
		}

		// if all the squirrels are dead
		if(squirrel_count == 0 && countdown_time == -2){
			round_number++;
			countdown_time = 3;
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
	loading_screen_init(); // load data for loading screen
	loading_screen_display(); // run loading screen animation

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