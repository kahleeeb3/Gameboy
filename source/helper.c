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
    int x1 = s1->x_pos + (s1->size/2);
    int y1 = s1->y_pos + (s1->size/2);

    // center of sprite 2
    int x2 = s2->x_pos + (s2->size/2);
    int y2 = s2->y_pos + (s2->size/2);

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
    if( (sprite->x_pos > SCREEN_WIDTH) || (sprite->x_pos < 0) ){
        return 1; // sprite is off screen
    }
	else if ( (sprite->y_pos > SCREEN_HEIGHT) || (sprite->y_pos < 0) ){
        return 1; // sprite is off screen
	}
    else
    {
        return 0; // sprite is on screen
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

	REG_DISPCNT |= DCNT_BG0; // enbale background 0
	irq_init(NULL); // Initialize the interrupt system with no callback function
    irq_add(II_VBLANK, NULL); // Add a VBLANK interrupt with no callback function

	// Initialize the TTE library for displaying text in mode 4 (4bpp) with
    // 16x16 character cells using the Verdana 9 font with shading
	 tte_init_chr4c(
        0,                              // BG number.
        BG_CBB(2) | BG_SBB(10),         // BG control.
        0xF000,                         // Screen-entry base
        bytes2word(13, 15, 0, 0),       // Color attributes.
        CLR_WHITE,                      // Ink color
        &verdana9_b4Font,               // Verdana 9, with shade.
        (fnDrawg)chr4c_drawg_b4cts_fast // b4cts renderer, asm version
    );

	// set to be higher priority so sprite go behind

	tte_init_con(); // Initialize the console I/O for text output
}