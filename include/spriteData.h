#ifndef SPRITESDATA
#define SPRITESDATA

// GENERAL DEFINES
#define palLen 32 // all palettes are this size

// PLAYER DEFINES
#define playerTilesLen 2048
extern const unsigned int playerTiles[512];
extern const unsigned int playerPal[8];

// APPLE DEFINES
#define appleTilesLen 32
extern const unsigned int appleTiles[8];
extern const unsigned int applePal[8];

// SQUIRREL
#define squirrelTilesLen 512
extern const unsigned int squirrelTiles[128];
extern const unsigned int squirrelPal[8];
extern const unsigned int squirrel2Pal[8];

// BUILDING DEFINES
#define buildingTilesLen 1024
extern const unsigned int lilly1Tiles[256];
extern const unsigned int lilly2Tiles[256];
extern const unsigned int sparks1Tiles[256];
extern const unsigned int sparks2Tiles[256];
extern const unsigned int baxter1Tiles[256];
extern const unsigned int baxter2Tiles[256];
extern const unsigned int goodrich1Tiles[256];
extern const unsigned int goodrich2Tiles[256];
extern const unsigned int hays1Tiles[256];
extern const unsigned int hays2Tiles[256];
extern const unsigned int detchon1Tiles[256];
extern const unsigned int detchon2Tiles[256];
extern const unsigned int chapel1Tiles[256];
extern const unsigned int chapel2Tiles[256];
extern const unsigned int center1Tiles[256];
extern const unsigned int center2Tiles[256];
extern const unsigned int buildingsPal[8];

// LOADING SCREEN SQUIRREL
#define loadingScreenSquirrelTilesLen 2048
#define loadingScreenSquirrelPalLen 32
extern const unsigned int loadingScreenSquirrelTiles[512];
extern const unsigned int loadingScreenSquirrelPal[8];


#endif /* SPRITESDATA */