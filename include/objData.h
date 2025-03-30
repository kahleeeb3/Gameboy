#ifndef OBJ_DATA
#define OBJ_DATA

#include <tonc.h>

typedef struct
{
  OBJ_ATTR *obj_attr; // attr location
  u16 t0; // starting tile
  u8 fp; // previous frame
} OBJ;

#define NUM_OBJ 128
extern OBJ_ATTR obj_attr_buf[NUM_OBJ];
extern OBJ obj_buf[NUM_OBJ];
extern OBJ *player;

#define PLAYER_TILE       1
#define PLAYER_PAL        0
#define PLAYER_PRIO       0
#define PLAYER_WIDTH      16
#define PLAYER_HEIGHT     32
#define PLAYER_X          (SCREEN_WIDTH-PLAYER_WIDTH)/2
#define PLAYER_Y          (SCREEN_HEIGHT-PLAYER_HEIGHT)/2
#define playerTilesLen    2048
#define playerPalLen      32

extern const unsigned short playerTiles[1024];
extern const unsigned short playerPal[16];
void player_init();

#endif