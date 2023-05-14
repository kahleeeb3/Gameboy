
//{{BLOCK(background)

//======================================================================
//
//	background, 512x512@4, 
//	+ palette 16 entries, not compressed
//	+ 437 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x64 
//	Total size: 32 + 13984 + 8192 = 22208
//
//	Time-stamp: 2023-05-13, 19:35:36
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.6
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BACKGROUND_H
#define GRIT_BACKGROUND_H

#define backgroundTilesLen 13984
extern const unsigned short backgroundTiles[6992];

#define backgroundMapLen 8192
extern const unsigned short backgroundMap[4096];

#define backgroundPalLen 32
extern const unsigned short backgroundPal[16];

#endif // GRIT_BACKGROUND_H

//}}BLOCK(background)
