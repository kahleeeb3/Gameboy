#ifndef BG_DATA
#define BG_DATA

#include <tonc.h>

#define QSC_CBB 0
#define QSC_SBB 31
#define QSC_PRIO 1
#define QSCTilesLen 2336
#define QSCMapLen 2048
#define QSCPalLen 32

extern const unsigned short QSCTiles[1168];
extern const unsigned short QSCMap[1024];
extern const unsigned short QSCPal[16];
void QSC_init();

#endif
