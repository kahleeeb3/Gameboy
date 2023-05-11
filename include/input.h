#include <tonc.h>

#define PLAYER_SPEED 5

#define PLAYER_FRAME1 0
#define PLAYER_FRAME2 16
#define PLAYER_FRAME3 32
#define PLAYER_FRAME4 48


typedef struct{
	OBJ_ATTR *mem_location;
	int size;
    int threw;
	int start_tile;
	int pal_bank;
	int curr_frame;
	int max_frame;
	int dir_facing;
	int x_pos;
	int y_pos;
}SpriteStruct;

int input(SpriteStruct *player, OBJ_ATTR *obj_buffer);