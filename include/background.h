#include <tonc.h>
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy


#ifndef MAP
#define MAP

#define backgroundTilesLen 13920
#define backgroundMapLen 8192
#define backgroundPalLen 32

#define miniMapTilesLen 7168
extern const unsigned int miniMapTiles[1792];

#define miniMapMapLen 2048
extern const unsigned int miniMapMap[512];

#define miniMapPalLen 32
extern const unsigned int miniMapPal[8];


typedef struct{
	int x_pos;
	int y_pos;
} Map;

Map *map_init();

#endif // MAP