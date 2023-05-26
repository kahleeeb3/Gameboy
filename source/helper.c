#include <tonc.h> // GBA library
#include "sprites.h"

// CALCULATIONS
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

int collision(Sprite *s1, Sprite *s2, int hbx, int hby)
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
	if( (x1 >= (x2-hbx) ) && (x1 <= (x2+hbx) ) ) 
	{
		// if within y range
		if( (y1 >= (y2-hby) ) && (y1 <= (y2+hby) ) )
		{
			return 1; // hit success
		}
	}

    return 0; // hit fail

}

// SOUNDS
void sound_init()
{
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);
    
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

void note_play(int note, int octave)
{
    REG_SND1FREQ = SFREQ_RESET | SND_RATE(note, octave);
}

// TIMERS
void timer_init()
{
	// Overflow every ~1 second:
    // 0x4000 ticks @ FREQ_1024
    REG_TM2D = -0x4000;          // 0x4000 ticks till overflow
    REG_TM2CNT = TM_FREQ_1024;   // we're using the 1024 cycle timer

    REG_TM2CNT |= TM_ENABLE;
    REG_TM3CNT = TM_ENABLE | TM_CASCADE;
}

u32 timer_reset()
{
	u32 time_elapsed = -1;
	REG_TM3CNT = 0;
	REG_TM3CNT = TM_ENABLE | TM_CASCADE;
	return time_elapsed;
}

u32 timer_update()
{
    return REG_TM3D;
}

int time_change(u32 time_elapsed)
{
    if (REG_TM3D != time_elapsed){
        return 1;
    }
    else{
        return 0;
    }
}