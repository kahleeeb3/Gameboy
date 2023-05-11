#include "input.h"
#include <tonc.h>

void walk(SpriteStruct *sprite, int end_frame){
	// get start_tile from spriteStruct
	int start_tile = sprite->start_tile;
	// get curr_frame from spriteStruct
	int curr_frame = sprite->curr_frame;
	// get size from spriteStruct
	int size = sprite->size;
	// get pal_bank from spriteStruct
	int pal_bank = sprite->pal_bank;

	// update attribute 2
	sprite->mem_location->attr2 = ATTR2_PALBANK(pal_bank) | (start_tile + ((size/2) * curr_frame));
	curr_frame = (curr_frame + 1) % end_frame;

	//update curr_frame
	sprite->curr_frame = curr_frame;
}

void player_throw(SpriteStruct *player, OBJ_ATTR *obj_buffer){
    player->mem_location->attr2 = PLAYER_FRAME3; // change to frame 3
    oam_copy(oam_mem, obj_buffer, 1); // update sprite
}

void player_skid(SpriteStruct *player, OBJ_ATTR *obj_buffer){
    player->mem_location->attr2 = PLAYER_FRAME4; // switch to frame 4
    oam_copy(oam_mem, obj_buffer, 1); // update sprite
    player->mem_location->attr1 ^= ATTR1_HFLIP;

    if(player->dir_facing == 1)
        player->dir_facing = -1;
    else
        player->dir_facing = 1;
        
}

int input(SpriteStruct *player, OBJ_ATTR *obj_buffer){
    
    // if press A
    if(key_held(KEY_A)){
        // returning at the beginning means the
        // player can't move until they release the
        // A button.
        player->threw = 1;
        return 0;
    }
    else if(player->threw == 1){
        player_throw(player, obj_buffer);
        player->threw = 0;
        return 1; // let play function know throw took place

    }

    player->x_pos += PLAYER_SPEED*key_tri_horz();
    player->y_pos += PLAYER_SPEED*key_tri_vert();

    // if left key held
    if(key_held(KEY_LEFT)){
        // if player facing right
        if(player->dir_facing == 1){
            player_skid(player, obj_buffer); // player skid animation
        }
        else{
            walk(player, 2); // player skid animation
        }
    }

    // if right key held
    if(key_held(KEY_RIGHT)){
        // if player facing left
        if(player->dir_facing == -1){
            player_skid(player, obj_buffer); // player skid animation
        }
        else{
            walk(player, 2); // player skid animation
        }
    }

    // update player animation
    obj_set_pos(player->mem_location, player->x_pos, player->y_pos);

    oam_copy(oam_mem, obj_buffer, 1);
    return 0;
}