#include "sprites.h"
#include "spriteData.h" // for sprite data
#include <string.h> // for memcpy
#include <stdlib.h> // for malloc

OBJ_ATTR obj_buffer[128];  // allocate space for 128 sprites
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer; // buffer for object affine
Sprite *player; // global pointer to the player sprite
Sprite* apples[APPLE_MAX]; // array of building sprite pointers
Sprite* squirrels[SQUIRREL_MAX]; // array of building sprite pointers
Sprite* buildings[BUILDINGS_MAX]; // array of building sprite pointers


void sprites_init()
{
    oam_init(obj_buffer, 128); // initialize 128 sprites

    // copy sprite tiles into tile memory
    memcpy(&tile_mem[4][PLAYER_TILE_ID], playerTiles, playerTilesLen);
    memcpy(&tile_mem[4][APPLE_TILE_ID], appleTiles, appleTilesLen);
    memcpy(&tile_mem[4][SQUIRREL_TITLE_ID], squirrelTiles, squirrelTilesLen);


    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*0)], detchon1Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*1)], detchon2Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*2)], hays1Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*3)], hays2Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*4)], center1Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*5)], center2Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*6)], baxter1Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*7)], baxter2Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*8)], lilly1Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*9)], lilly2Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*10)], sparks1Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*11)], sparks2Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*12)], goodrich1Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*13)], goodrich2Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*14)], chapel1Tiles, buildingTilesLen);
    memcpy(&tile_mem[4][BUILDING_TILE_ID+(32*15)], chapel2Tiles, buildingTilesLen);

    // copy sprite palettes into object palette memory
    memcpy(pal_obj_mem+0, playerPal, palLen);
    memcpy(pal_obj_mem+16, applePal, palLen);
    memcpy(pal_obj_mem+32, squirrelPal, palLen);
    memcpy(pal_obj_mem+48, squirrel2Pal, palLen);
    memcpy(pal_obj_mem+64, buildingsPal, palLen);

}

void scale_sprite(Sprite *sprite, OBJ_AFFINE *obj_aff_buffer_location, int obj_affine_num, int scale_factor)
{
	// the building sprites need to be affine so that we can scale them
	sprite->mem_addr->attr0 |= ATTR0_AFF | ATTR0_AFF_DBL;
	sprite->mem_addr->attr1 |= ATTR1_AFF_ID(obj_affine_num);
	OBJ_AFFINE *sprite_aff = obj_aff_buffer_location;
	obj_aff_identity(sprite_aff);
	FIXED scale = (1<<8)/scale_factor;
	obj_aff_scale(sprite_aff, scale, scale);
}

Sprite *mkSpriteStruct(int ob_index, int size, int tile_id, int pal_bank, int x, int y, int hbx, int hby)
{
	// allocate space for sprite struct
	Sprite *newSprite = (Sprite *)malloc(sizeof(Sprite));

    // set the struct values
    newSprite->mem_addr = &obj_buffer[ob_index];
	// newSprite->hidden = hidden; // set elsewhere
    newSprite->active = 0; // no sprite should be active at start
	newSprite->size = size;
	newSprite->start_tile = tile_id;
	newSprite->curr_tile = tile_id; // current tile is the start tile
	newSprite->pal_bank = pal_bank;
	newSprite->dir_facing = 1; // all sprites should start facing right
	newSprite->x_pos = x;
	newSprite->y_pos = y;
	// newSprite->target = 0; // set elsewhere
	newSprite->hbx = hbx;
	newSprite->hby = hby;
	// newSprite->index = 0; // set elsewhere

    // set object attribute 0
    newSprite->mem_addr->attr0 = ATTR0_4BPP; // all sprites are 4BPP

    // set object attribute 1
	if(size == 64)
		newSprite->mem_addr->attr1 = ATTR1_SIZE_64;
	else if(size == 32)
		newSprite->mem_addr->attr1 = ATTR1_SIZE_32;
	else if(size == 16)
		newSprite->mem_addr->attr1 = ATTR1_SIZE_16;
	else if(size == 8)
		newSprite->mem_addr->attr1 = ATTR1_SIZE_8;
	else if(size == -1)
	{
		// this is reserved for buildings
		newSprite->mem_addr->attr0 |= ATTR0_WIDE;
		newSprite->mem_addr->attr1 = ATTR1_SIZE_64x32;
	}

    // set object attribute 2
    newSprite->mem_addr->attr2 = ATTR2_PALBANK(newSprite->pal_bank) | newSprite->curr_tile;
	newSprite->mem_addr->attr2 |=  ATTR2_PRIO(1); // set priority


    obj_set_pos(newSprite->mem_addr, x, y); // set the objects position


    return newSprite;
}

void set_player_attributes()
{
    // screen resolution is 240x160
	// player sprite is 32x32
	// sprite location is given by the top left index
	// thus player is center if at
	// x = (240/2 - 1) - (32/2 - 1) = 102
	// y = (160/2 - 1) - (32/2 - 1) = 64
    int x = 102;
    int y = 64;

    // set sprite struct values
    player = mkSpriteStruct(0, 32, PLAYER_TILE_ID, PLAYER_PAL_BANK, x, y, 8, 8);

    // set unassigned values
    player->hidden = 0; // is not hidden
    // player->target = 0; // has no target
    player->index = 1; // is player 1
}

void set_apple_attributes()
{
    // fill sprite slots 1 through APPLE_MAX with apple sprites
	for(int i = 0; i < APPLE_MAX; i++){

		int sprite_num = PLAYER_MAX + i;
		int x = 2 + (i*8); // space them apart by 8 pixels
		int y = 24;

		// create new apple sprite
		Sprite *new_apple = mkSpriteStruct(sprite_num, 8, APPLE_TILE_ID, APPLE_PAL_BANK, x, y, 0, 0);
		new_apple->index = i; // set the index of the apple
		new_apple->active = 0; // set as not active (has not been thrown yet)
		apples[i] = new_apple; // add to list of apples
	}
}

void set_squirrel_attributes()
{
	// fill sprite slots (1+APPLE_MAX) through SQUIRREL_MAX with squirrel sprites
	for(int i = 0; i < SQUIRREL_MAX; i++){

		int sprite_num = PLAYER_MAX + APPLE_MAX + i;
		// set squirrels in the corner for now since they are hidden anyway
		int x = 0;
		int y = 0;

		// create new squirrel sprite
		Sprite *new_squirrel = mkSpriteStruct(sprite_num, 16, SQUIRREL_TITLE_ID, SQUIRREL_PAL_BANK, x, y, 4, 4);
		new_squirrel->index = i; // set the index of the squirrel
		new_squirrel->dir_facing = -1; // the sprites were drawn facing the wrong direction. May fix later.
		new_squirrel->active = 0; // set as not active (has been hit)
		squirrels[i] = new_squirrel; // add to list of squirrels
		obj_hide(new_squirrel->mem_addr); // hide the sprite
	}
}

void set_building_attributes()
{
    // array of building locations
	int building_positions[8][2] = {
		{-183, -71}, 	// detchon
		{-70, -128}, 	// hays
		{69, -97}, 		// center
		{205, -127},	// baxter
		{-51, 134}, 	// lilly
		{83, 127}, 		// sparks
		{188, 96}, 		// goodrich
		{248, 3} 		// chapel
		};

	// array of building hit box sizes {hbx, hby}
	int building_hit_boxes[8][2] = {
		{38, 32}, 		// detchon
		{64, 50}, 		// hays
		{65, 26}, 		// center
		{58, 46}, 		// baxter
		{37, 63}, 		// lilly
		{38, 38}, 		// sparks
		{37, 22}, 		// goodrich
		{41, 22} 		// chapel
		};
	
	int building_num = 0; // track index in position array

    for(int i = 0; i < BUILDINGS_MAX; i++){

        // define the index in object memory
        int sprite_num = PLAYER_MAX + APPLE_MAX + SQUIRREL_MAX + i;

        // gt x/y pos and hit box sizes from array
        int x = building_positions[building_num][0];
		int y = building_positions[building_num][1];
		int hbx = building_hit_boxes[building_num][0];
		int hby = building_hit_boxes[building_num][1];

        // if second half of the sprite
		if( (i%2) != 0){
			y += 64; // move the sprite below top half
			building_num++; // increment building number
		}

        // set sprite struct values
        Sprite *new_building = mkSpriteStruct(sprite_num, -1, BUILDING_TILE_ID+(i*32), BUILDINGS_PAL_BANK, x, y, hbx, hby);

        // set unassigned values
        new_building->index = i; // which building in the building array is this
		new_building->active = 1; // all buildings are active at start

        scale_sprite(new_building, &obj_aff_buffer[i], i, 2); // scale by a factor of 2


        // determine if building should be hidden
		int bottom = new_building->y_pos + 64;
		int right = new_building->x_pos + 128;
		int top = new_building->y_pos;

		if( (top > SCREEN_HEIGHT) || (bottom < 0) || ( right < 0) ){
			new_building->hidden = 1; // mark as hidden
			obj_hide(new_building->mem_addr); // hide the building sprites
		}
        buildings[i] = new_building; // add to list of buildings

    }
    obj_aff_copy(obj_aff_mem, obj_aff_buffer, BUILDINGS_MAX);
}