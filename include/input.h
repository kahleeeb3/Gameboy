#include <tonc.h>

#define PLAYER_SPEED 5

#define MAX_APPLES 5
#define APPLE_SPEED 7

#define MAX_SQUIRRELS 8
#define SQUIRREL_SPEED 1
#define SQUIRREL_HIT_BOX 4

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


typedef struct{
	OBJ_ATTR *mem_location; // location in object mem
	int thrown; // if the apple has been thrown
	int x_pos; // x location
	int y_pos; // y location
	int direction; // apple moving left or right
	int index; // which apple is this
} Apple;


typedef struct{
	OBJ_ATTR *mem_location; // location in object mem
	int hit; // if the squirrel has been hit
	int x_pos; // x location
	int y_pos; // y location
	int x_dir; // x direction headed
	int y_dir; // y direction headed
	int index; // which squirrel in the list
} Squirrel;

int input(SpriteStruct *player, OBJ_ATTR *obj_buffer);