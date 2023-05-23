#include "helper.h"


int get_center(Sprite *sprite)
{
	return (sprite->size/2) - 1;
}

int get_rel_x_pos(Sprite *sprite)
{
	// x value for center of screen
	int screen_x_center = SCREEN_WIDTH/2 - 1;

	// x value for center of player
	int sprite_x_center = get_center(sprite);

	// relative center for the sprite
	int rel_x_center = (screen_x_center) - (sprite_x_center);

	// if sprite is left of the center of the screen
	if ( sprite->x_pos < rel_x_center)
		return -1;
	// if sprite is right of the center of the screen
	else if ( sprite->x_pos > rel_x_center)
		return 1;

	// player is center
	else
		return 0;
}

int get_rel_y_pos(Sprite *sprite)
{
	// y value for center of screen
	int screen_y_center = SCREEN_HEIGHT/2 - 1;

	// y value for center of player
	int sprite_y_center = get_center(sprite);

	// relative center for the sprite
	int rel_y_center = (screen_y_center) - (sprite_y_center);

	// if sprite is below of the center of the screen
	if ( sprite->y_pos > rel_y_center)
		return -1;
	// if sprite is above of the center of the screen
	else if ( sprite->y_pos < rel_y_center)
		return 1;

	// player is center
	else
		return 0;
}

int get_rel_diag_pos(Sprite *sprite)
{
	int screen_x_center = SCREEN_WIDTH/2 - 1;
	int screen_y_center = SCREEN_HEIGHT/2 - 1;

	int sprite_x_center = get_center(sprite);
	int sprite_y_center = get_center(sprite);

	int rel_x_center = (screen_x_center) - (sprite_x_center);
	int rel_y_center = (screen_y_center) - (sprite_y_center);

	// Sprite is NE
	if ( (sprite->x_pos > rel_x_center) && (sprite->y_pos < rel_y_center) )
		return 1; // in first quadrant

	// Sprite is NW
	else if ( (sprite->x_pos < rel_x_center) && (sprite->y_pos < rel_y_center) )
		return 2; // in second quadrant

	// Sprite is SW
	else if ( (sprite->x_pos < rel_x_center) && (sprite->y_pos > rel_y_center) )
		return 3; // in third quadrant

	// Sprite is SE
	else if ( (sprite->x_pos > rel_x_center) && (sprite->y_pos > rel_y_center) )
		return 4; // in third quadrant

	else
		return -1; // player is center

}

int collision(Sprite *s1, Sprite *s2, int hb)
{
    
	// center of sprite 1
	int x1 = s1->x_pos + (s1->size/2) - 1;
	int y1 = s1->y_pos + (s1->size/2) - 1;

	// center of sprite 2
	int x2 = 0;
	int y2 = 0;
	
	
	// center of sprite 2 (non-buildings)
	if(s2->size != -1){
		x2 = s2->x_pos + (s2->size/2) - 1;
		y2 = s2->y_pos + (s2->size/2) - 1;
	}

	// for buildings we need to do something different
	else{
		x2 = s2->x_pos + (64) - 1;
		y2 = s2->y_pos;
	}

    // if within x range
	if( (x1 >= (x2-hb) ) && (x1 <= (x2+hb) ) ) 
	{
		// if within y range
		if( (y1 >= (y2-hb) ) && (y1 <= (y2+hb) ) )
		{
			return 1; // hit success
		}
	}

    return 0; // hit fail

}

int off_screen(Sprite *sprite)
{
    
	int l = sprite->x_pos;
	int r = sprite->x_pos + sprite->size - 1;
	int t = sprite->y_pos;
	int b = sprite->y_pos + sprite->size - 1;
	
	if( (l > SCREEN_WIDTH) || (r < 0) ){
		return 1;
	}
	else if( (t > SCREEN_HEIGHT) || (b < 0) ){
		return 1;
	}
	else{
		return 0;
	}
}

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

int building_off_screen(Sprite *building)
{
	int bottom = building->y_pos + 64;
	int right = building->x_pos + 128;
	int top = building->y_pos;

	if( (top > SCREEN_HEIGHT) || (bottom < 0) || ( right < 0) ){
		return 1;
	}

	return 0;
}

void sound_init()
{
    // turn sound on
    REG_SNDSTAT= SSTAT_ENABLE;
    // snd1 on left/right ; both full volume
    REG_SNDDMGCNT = SDMG_BUILD_LR(SDMG_SQR1, 7);
    // DMG ratio to 100%
    REG_SNDDSCNT= SDS_DMG100;

    // no sweep
    REG_SND1SWEEP= SSW_OFF;
    // envelope: vol=12, decay, max step time (7) ; 50% duty
    REG_SND1CNT= SSQR_ENV_BUILD(12, 0, 7) | SSQR_DUTY1_2;
    REG_SND1FREQ= 0;
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

void note_play(int note, int octave)
{
    REG_SND1FREQ = SFREQ_RESET | SND_RATE(note, octave);
}