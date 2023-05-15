#include <tonc.h>
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy


#ifndef MAP
#define MAP

#define backgroundTilesLen 13984

#define backgroundMapLen 8192

#define backgroundPalLen 32


typedef struct{
	int x_pos;
	int y_pos;
} Map;

Map *map_init();

#endif // MAP