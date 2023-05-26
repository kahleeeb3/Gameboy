#include <tonc.h> // GBA library
#include <string.h> // for memcpy
#include <stdlib.h> // for malloc
#include "sprites.h"
#include "spriteData.h"
#include "background.h"
#include "backgroundData.h"

Map *map; // global pointer which give the map position


void text_init()
{

	/*
	 * To be quite frank, the full functionality of this is a bit complex.
	 * Most of this code was borrowed from the library documentation.
	 * https://www.coranac.com/tonc/text/tte.htm
	 * Essentially it enables a set of text tiles (similar to sprites)
	 * and a palette of text colors to use. I provided as much documentation
	 * as possible here.
	 */

	// REG_DISPCNT |= DCNT_BG0; // enable background 0
	// irq_init(NULL); // Initialize the interrupt system with no callback function
    // irq_add(II_VBLANK, NULL); // Add a VBLANK interrupt with no callback function

	// Initialize the TTE library for displaying text in mode 4 (4bpp) with
    // 16x16 character cells using the Verdana 9 font with shading
	 tte_init_chr4c(
        0,                              // BG number.
        BG_CBB(2) | BG_SBB(7),         // BG control. (i just changed these a little until it worked?)
        0xF000,                         // Screen-entry base
        bytes2word(13, 15, 0, 0),       // Color attributes.
        CLR_WHITE,                      // Ink color
        &verdana9_b4Font,               // Verdana 9, with shade.
        (fnDrawg)chr4c_drawg_b4cts_fast // b4cts renderer, asm version
    );


	tte_init_con(); // Initialize the console I/O for text output
}

void map_init()
{

	map = (Map *)malloc(sizeof(Map));

    memcpy(pal_bg_mem, backgroundPal, backgroundPalLen);
    memcpy(&tile_mem[0][0], backgroundTiles, backgroundTilesLen);
    memcpy(&se_mem[28][0], backgroundMap, backgroundMapLen);

    // set the background 1 attributes
	REG_BG1CNT= BG_CBB(0) | BG_SBB(28) | BG_4BPP | BG_REG_64x64;
	REG_BG1CNT |= BG_PRIO(1); // set background priority

    // the center of the map is roughly (150, 160)
	// map boundaries are x->{0, 270}, y->{0, 260}
	REG_BG1HOFS = 150;
    REG_BG1VOFS = 160;

	map->x_pos = 150;
	map->y_pos = 160;

}

void mini_map_init()
{

	// copy data over to registers
	memcpy(pal_bg_mem+16, miniMapPal, miniMapPalLen);
    memcpy(&tile_mem[1][0], miniMapTiles, miniMapTilesLen);
    memcpy(&se_mem[26][0], miniMapMap, miniMapMapLen);

	// change the palette used	
	SCR_ENTRY *bg2_map= se_mem[26];
	for(int i=0; i<1024; i++){
		*bg2_map++ |= SE_PALBANK(1); // set to use palette 1
	}

    REG_BG2CNT = BG_REG_32x32 | BG_SBB(26)| BG_4BPP | BG_CBB(1);
	REG_BG2CNT |= BG_PRIO(2); // set priority
}

void loading_screen_init()
{
	// (1) BG0: CP3 Logo
	memcpy(pal_bg_mem, cp3gamesPal, cp3gamesPalLen);
    memcpy(&tile_mem[0][0], cp3gamesTiles, cp3gamesTilesLen);
    memcpy(&se_mem[29][0], cp3gamesMap, cp3gamesMapLen);
	REG_BG0CNT= BG_CBB(0) | BG_SBB(29) | BG_4BPP | BG_REG_32x32;
	REG_BG0HOFS = 0; // horizontal offset to 0
	REG_BG0VOFS = 0; // vertical offset to 0

	// (2) GB2: Start Text
	memcpy(pal_bg_mem+16, startScreenPal, startScreenPalLen);
    memcpy(&tile_mem[1][0], startScreenTiles, startScreenTilesLen);
    memcpy(&se_mem[30][0], startScreenMap, startScreenMapLen);
	REG_BG1CNT = BG_REG_32x32 | BG_SBB(30)| BG_4BPP | BG_CBB(1);
	REG_BG1HOFS = 0; // horizontal offset to 0
	REG_BG1VOFS = 0; // vertical offset to 0
	SCR_ENTRY *bg1_map= se_mem[30]; // pointer to screen entry 30
	for(int i=0; i<1024; i++){ // for each char block (32x32)
		*bg1_map++ |= SE_PALBANK(1); // set to use palette 1
	}

	// (3) BG1: Wabash Map
	memcpy(pal_bg_mem+32, chapelImagePal, chapelImagePalLen);
    memcpy(&tile_mem[2][0], chapelImageTiles, chapelImageTilesLen);
    memcpy(&se_mem[31][0], chapelImageMap, chapelImageMapLen);
	REG_BG2CNT = BG_REG_32x32 | BG_SBB(31)| BG_4BPP | BG_CBB(2);
	REG_BG2HOFS = 0; // horizontal offset to 0
	REG_BG2VOFS = 0; // vertical offset to 0
	SCR_ENTRY *bg2_map= se_mem[31]; // pointer to screen entry 31
	for(int i=0; i<1024; i++){	// for each char block (32x32)
		*bg2_map++ |= SE_PALBANK(2); // set to use palette 2
	}

	// (4) Evil Squirrel Sprite
	memcpy(&tile_mem[4][0], loadingScreenSquirrelTiles, loadingScreenSquirrelTilesLen);
	memcpy(pal_obj_mem, loadingScreenSquirrelPal, loadingScreenSquirrelPalLen);
	OBJ_ATTR *lsSquirrel = &obj_buffer[0]; // pointer to first object in the object buffer
	obj_set_attr(lsSquirrel, 
		ATTR0_SQUARE | ATTR0_AFF | ATTR0_AFF_DBL, // square shape, affine enabled, double size
		ATTR1_SIZE_64 | ATTR1_AFF_ID(0), // 64x64 size, affine transformation ID 0
		0); // priority of the sprite to 0
	OBJ_AFFINE *lsSquirrel_aff = &obj_aff_buffer[0]; // pointer to the first affine in the object affine buffer
	obj_aff_identity(lsSquirrel_aff); // Initializes the affine transformation as an identity matrix
	FIXED scale = (1<<8)/2; // scale by a factor of 2
	obj_aff_scale(lsSquirrel_aff, scale, scale); // apply scale factor
	obj_set_pos(lsSquirrel, 240, 33); // set position
	obj_aff_copy(obj_aff_mem, obj_aff_buffer, 1); // copy to object affine memory
	oam_copy(oam_mem, obj_buffer, 1); // copy to object attribute memory

}

void loading_screen_display()
{
	REG_DISPCNT |= DCNT_BG0; // enable background 0

	// this is a trick for "sleep"
	for(int i = 0; i < 200; i++){
		vid_vsync(); // wait for VBlank
	}

	REG_DISPCNT ^= DCNT_BG0; // disable background 0
	REG_DISPCNT |= DCNT_BG2; // enable background 2

	for(int i = 240; i>0; i= i-2){

		vid_vsync(); // wait for VBlank

		obj_set_pos(&obj_buffer[0], i, 33); // set squirrel position
		oam_copy(oam_mem, obj_buffer, 1); // copy to object attribute memory
	}

	REG_DISPCNT |= DCNT_BG1; // enable background 1

	// const u8 notes[12]= {NOTE_CIS, NOTE_D, NOTE_DIS, NOTE_E, NOTE_CIS, NOTE_D, NOTE_DIS, NOTE_E, NOTE_DIS, NOTE_D, NOTE_CIS, NOTE_C};
    // const u8 lens[12]= { 2, 2, 2, 4, 2, 2, 2, 4, 2, 2, 2, 4 };

	// play music until key press
	while(!key_was_down(KEY_START)){

		for(int i=0; i< 12 ; i++)
		{
			key_poll();
			if(key_is_down(KEY_START)){
				break;
			}
			// note_play(notes[i], -1);
			// VBlankIntrDelay(8*lens[i]);
		}
		
	}

	// change palette color from green to black
	// this prevents a weird flash of green before
	// starting the actual game
	pal_bg_bank[0][0] = CLR_BLACK;
	pal_bg_bank[1][0] = CLR_BLACK;
	pal_bg_bank[2][0] = CLR_BLACK;
	REG_DISPCNT ^= DCNT_BG1; // disable background 1
	REG_DISPCNT ^= DCNT_BG2; // disable background 2
	REG_DISPCNT ^= DCNT_OBJ; // disable objects
	
}

void window_init()
{
    
	/*
	 * Window 0 will display the minimap and text
	 * Window 1 will display everything except background 2
	 * This will allow for updating the building health
	 * and score at the same time but only showing one.
	 * We will enable/disable window 0 to show minimap
	 */
	
	
	// strcut for storing window borders { left, top, right, bottom }
	typedef struct tagRECT_U8 { u8 ll, tt, rr, bb; } ALIGN4 RECT_U8;

	// define window positions
	RECT_U8 win[2]= 
	{
		{ 48, 8,  193,  152 },  // win0: minimap display
		{ 0, 0 ,SCREEN_WIDTH, 24 } // win1: text at top of screen

	};
	
	// set positions
	REG_WIN0H= win[0].ll<<8 | win[0].rr;
    REG_WIN1H= win[1].ll<<8 | win[1].rr;
    REG_WIN0V= win[0].tt<<8 | win[0].bb;
    REG_WIN1V= win[1].tt<<8 | win[1].bb;

	REG_DISPCNT |= DCNT_WIN1; // enable window 1

	// define what goes in each window
	REG_WININ= WININ_BUILD( (WIN_BG0|WIN_BG2), (WIN_ALL) );
	REG_WINOUT= WINOUT_BUILD((WIN_OBJ|WIN_BG1), 0);
}
