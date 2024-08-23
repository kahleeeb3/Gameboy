#include "bgData.h"

void QSC_init()
{  
  memcpy32(pal_bg_bank, QSCPal, QSCPalLen/4);
  memcpy32(tile_mem, QSCTiles, QSCTilesLen/4);
  memcpy32(&se_mem[QSC_SBB], QSCMap, QSCMapLen/4);

  REG_BG0CNT = BG_CBB(QSC_CBB) | BG_SBB(QSC_SBB) | BG_4BPP | BG_REG_32x32 | BG_PRIO(QSC_PRIO);
}

// TILES
const unsigned short QSCTiles[1168] __attribute__((aligned(4)))=
{
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0xBBBB,0xBBBB,0xBBBB,0xBBBB,0xDDDD,0xDDDD,0xDDDD,0xDDDD,
	0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,
	0xBBBB,0xBBBB,0xBBBB,0xBBBB,0xDDDD,0xDDDD,0xDDDD,0xDDDD,
	0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0x3339,0x3333,
	0xBBBB,0xBBBB,0xBBBB,0xBBBB,0xDDDD,0xDDDD,0xDDDD,0xDDDD,
	0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0x3333,0x3333,

	0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,
	0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,
	0xEEE9,0xEEEE,0xEEE9,0xEEEE,0xEEE9,0xEEEE,0xEEE9,0xEEEE,
	0xEEE9,0xEEEE,0xEEE9,0xEEEE,0xEEE9,0xEEEE,0xEEE9,0xEEEE,
	0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,
	0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,
	0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0xDDDD,0x5555,0x5555,
	0x5555,0x5555,0xBBBB,0xBBBB,0xDDDD,0xDDDD,0xDDDD,0xDDDD,

	0x4449,0x4444,0x4449,0x4444,0x4449,0x4444,0x4339,0x4444,
	0x3339,0x3333,0x3339,0x3333,0x4339,0x4444,0x4449,0x4444,
	0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
	0x3333,0xC333,0x3333,0xD333,0x4444,0xD444,0x4444,0xD444,
	0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
	0xCDDD,0x3333,0xDCDC,0x3333,0xDDCD,0x4444,0xDCDC,0x4444,
	0xDDDD,0xDDDD,0xDDDD,0xDDDD,0x1DDD,0x1111,0x9155,0x9999,
	0x9155,0x9999,0x91BB,0x9999,0x91DD,0x9999,0x91DD,0x9999,

	0xDDDD,0xDDDD,0xDDDD,0xDDDD,0x1111,0x1111,0x9999,0x9999,
	0x9999,0x9999,0x9999,0x9999,0x9999,0x9999,0x9999,0x9999,
	0xDDDD,0xDDDD,0xDDDD,0xDDDD,0x1111,0xDD11,0x9999,0x5199,
	0x9999,0x5199,0x9999,0xB199,0x9999,0xD199,0x9999,0xD199,
	0xDDDD,0xDDDD,0xDDDD,0xDDDD,0x9999,0x9999,0x1111,0x1111,
	0xAAAA,0xAAAA,0xAAAA,0xAAAA,0x3333,0x3333,0x3333,0x3333,
	0x4449,0x4444,0x4449,0x4444,0x4449,0x4444,0x4449,0x4444,
	0x4449,0x4444,0x4449,0x4444,0x4444,0x4444,0x4444,0x4444,

	0x4444,0xC444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
	0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
	0xCDDD,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
	0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
	0x91DD,0x9999,0x91DD,0x9999,0x9199,0x9999,0x9111,0x9999,
	0x91AA,0x9999,0x91AA,0x9999,0x9133,0x9999,0x9133,0x9999,
	0x9999,0x9999,0x9999,0x9999,0x9999,0x9999,0x9999,0x9999,
	0x9999,0x9999,0x9999,0x9999,0x9999,0x9999,0x9999,0x9999,

	0x9999,0xD199,0x9999,0xD199,0x9999,0x9199,0x9999,0x1199,
	0x9999,0xA199,0x9999,0xA199,0x9999,0x3199,0x9999,0x3199,
	0xFFDD,0xFFFF,0xCC8D,0xCCCC,0xCC89,0xCCCC,0xC811,0xCCCC,
	0xC8AA,0xCFCF,0xC8AA,0xFCFC,0xC833,0xCFCF,0xC833,0xCCCC,
	0xFFFF,0xDDDF,0xCCCC,0xDDCC,0xCCCC,0x99CC,0xCCCC,0x111C,
	0xCFCF,0xAAAC,0xCCFC,0xAAAC,0xCFCF,0x333C,0xCCCC,0x333C,
	0x91EE,0x9999,0x91EE,0x9999,0x91EE,0x9999,0x91EE,0x9999,
	0x71EE,0x7777,0x71EE,0x7777,0x1BEE,0x1111,0x1BEE,0x1779,

	0x9999,0x9999,0x9999,0x9999,0x9999,0x9999,0x9999,0x9999,
	0x7777,0x7777,0x7777,0x7777,0x1111,0x1111,0x1111,0x1111,
	0x9999,0xE199,0x9999,0xE199,0x9999,0xE199,0x9999,0xE199,
	0x7777,0xE177,0x7777,0xE177,0x1111,0xEB11,0x7911,0xEB17,
	0xFC8E,0xFCFC,0xCC8E,0xCFCF,0xFC8E,0xFCFC,0xCC8E,0xCCCC,
	0x88EE,0x8888,0xDBEE,0xEEEB,0x3BEE,0xEEEB,0xBBEE,0xEEEB,
	0xFCFC,0xEECC,0xCFCF,0xEECC,0xFCFC,0xEECC,0xCCCC,0xEECC,
	0x8888,0xEEE8,0xDBEE,0xEEEB,0x3BEE,0xEEEB,0xBBEE,0xEEEB,

	0x1BEE,0x1779,0xBEEE,0xB111,0xBEEE,0xB111,0xEEEE,0xEEEE,
	0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,
	0xBBBB,0xBBBB,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,
	0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,
	0x791B,0xEB17,0x11BE,0xEEB1,0x11BE,0xEEB1,0xEEEE,0xEEEE,
	0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,
	0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xECCE,0xEEEE,0xCCCF,0xEEEE,
	0xCCCF,0xEEEE,0xCCCF,0xEEEE,0xCCCF,0xCCCE,0xCCCF,0xCCCC,

	0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,
	0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xECCC,0xEEEE,0xCCCC,0xEEEE,
	0xEEEE,0xEEEE,0xBBBE,0xBBBB,0x2221,0x2222,0x6221,0x6666,
	0x6221,0x6666,0x6221,0x6666,0x6221,0x6666,0x6221,0x6666,
	0xEEEE,0xEEEE,0xBBBB,0xBBBB,0x2222,0x2222,0x6666,0x6666,
	0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,
	0xEEEE,0xEEEE,0xBBBB,0xEBBB,0x2222,0xB222,0x6666,0xB226,
	0x6666,0xB226,0x6666,0xB226,0x6666,0xB226,0x6666,0xB226,

	0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
	0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
	0xF444,0xFFFF,0xCC44,0xCCCC,0xCC44,0xCCCC,0xC444,0xCCCC,
	0xC444,0xFCFC,0xC444,0xCFCC,0xC444,0xFCFC,0xC444,0xCCCC,
	0xFFFF,0x44FF,0xCCCC,0x48CC,0xCCCC,0x48CC,0xCCCC,0x448C,
	0xFCFC,0x448C,0xCFCF,0x448C,0xFCFC,0x448C,0xCCCC,0x448C,
	0xCCCF,0xCFCC,0xCCCF,0xFCFC,0x8CCF,0xCFCC,0x8884,0xFCFC,
	0x8C44,0xCFCC,0x8444,0xFCFC,0x8444,0xCFCC,0x4444,0xCCC8,

	0xCFCF,0x444C,0xCCFC,0x444C,0xCFCF,0x444C,0xCCFC,0x444C,
	0xCFCF,0x444C,0xCCFC,0x444C,0xCFCF,0x444C,0xCCCC,0x444B,
	0x6221,0x6666,0x6221,0x6666,0x6221,0x6666,0x6221,0x6666,
	0x6221,0x6666,0x6221,0x6666,0x6221,0x6666,0x6221,0x6666,
	0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,
	0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,
	0x6666,0xB226,0x6666,0xB226,0x6666,0xB226,0x6666,0xB226,
	0x6666,0xB226,0x6666,0xB226,0x6666,0xB226,0x6666,0xB226,

	0xCC44,0xCFCF,0xCC44,0xFCFC,0xCC44,0xCFCF,0xCC44,0xCCCC,
	0x8444,0x8888,0xB444,0x44BD,0xB444,0x44B3,0xB444,0x44BB,
	0xCFCF,0x48CF,0xFCFC,0x48CC,0xCFCF,0x48CF,0xCCCC,0x48CC,
	0x8888,0x4488,0xB444,0x44BD,0xB444,0x44B3,0xB444,0x44BB,
	0x4444,0x8884,0xDB44,0x444B,0x3DB4,0x44BD,0x3DB4,0x44BD,
	0xDB44,0x444B,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
	0x4888,0x444B,0xB444,0x44BD,0xDB44,0x4BD3,0xDB44,0x4BD3,
	0xB444,0x44BD,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,

	0x6221,0x6666,0x6221,0x6666,0x2221,0x2222,0x1111,0x1111,
	0xAAA4,0xAAAA,0xD1A4,0x444B,0xD1A4,0x444B,0xBA44,0x4444,
	0x6666,0x6666,0x6666,0x6666,0x2222,0x2222,0x1111,0x1111,
	0xAAAA,0xAAAA,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
	0x6666,0xB226,0x6666,0xB226,0x2222,0xB222,0x1111,0xB111,
	0xAAAA,0x4BBA,0xA444,0x4BD1,0xA444,0x4BD1,0x4444,0x44BA,
	0xCEEE,0xFCFC,0xCEEE,0xCFCC,0xCEEE,0xFCFC,0xCEEE,0xCFCC,
	0xCEEE,0xFCFC,0xCEEE,0xCFCC,0xCEEE,0xFCFC,0xBEEE,0xCCCC,

	0xCCFC,0xFCCC,0xCFCF,0xFCCC,0xCCFC,0xFCC8,0xCFCF,0xE888,
	0xCCFC,0xEEC8,0xCFCF,0xEEE8,0xCCFC,0xEEE8,0x8CCC,0xEEEE,
	0xFEEE,0xFFFF,0xCCEE,0xCCCC,0xCCEE,0xCCCC,0xCEEE,0xCCCC,
	0xCEEE,0xFCFC,0xCEEE,0xCFCC,0xCEEE,0xFCFC,0xCEEE,0xCCCC,
	0xFFFF,0xEEFF,0xCCCC,0xE8CC,0xCCCC,0xE8CC,0xCCCC,0xEE8C,
	0xFCFC,0xEE8C,0xCFCF,0xEE8C,0xFCFC,0xEE8C,0xCCCC,0xEE8C,
	0x4444,0x4444,0xBBB4,0xBBBB,0x222B,0x2222,0x622B,0x6666,
	0x622B,0x6666,0x622B,0x6666,0x622B,0x6666,0x622B,0x6666,

	0x4444,0x4444,0xBBBB,0xBBBB,0x2222,0x2222,0x6666,0x6666,
	0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,
	0x4444,0x4444,0xBBBB,0x4BBB,0x2222,0x1222,0x6666,0x1226,
	0x6666,0x1226,0x6666,0x1226,0x6666,0x1226,0x6666,0x1226,
	0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
	0x4444,0x4444,0x4444,0x4444,0x4444,0xCCC4,0x4444,0xCCCC,
	0x4444,0x4444,0x4444,0x4444,0x4444,0x4CC4,0x4444,0xFCCC,
	0x4444,0xFCCC,0x4444,0xFCCC,0x4CCC,0xFCCC,0xCCCC,0xFCCC,

	0xFFEE,0xFFFF,0xCC8E,0xCCCC,0xCC8E,0xCCCC,0xC8EE,0xCCCC,
	0xC8EE,0xFCFC,0xC8EE,0xCFCF,0xC88E,0xFCFC,0xC8C8,0xCFCF,
	0xFFFF,0xEEEF,0xCCCC,0xEECC,0xCCCC,0xEECC,0xCCCC,0xEEEC,
	0xCCFC,0xEEEC,0xCFCF,0xEEEC,0xCCFC,0xEE8C,0xCFCF,0xE8CC,
	0x622B,0x6666,0x622B,0x6666,0x222B,0x2222,0x111B,0x1111,
	0xABBE,0xAAAA,0x1DBE,0xEEEA,0x1DBE,0xEEEA,0xABEE,0xEEEE,
	0x6666,0x6666,0x6666,0x6666,0x2222,0x2222,0x1111,0x1111,
	0xAAAA,0xAAAA,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,

	0x6666,0x1226,0x6666,0x1226,0x2222,0x1222,0x1111,0x1111,
	0xAAAA,0xEAAA,0xBEEE,0xEA1D,0xBEEE,0xEA1D,0xEEEE,0xEEAB,
	0xBEEE,0x888E,0xDBEE,0xEEEB,0x3DBE,0xEEBD,0x3DBE,0xEEBD,
	0xDBEE,0xEEEB,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,
	0xE888,0xEEEE,0xBEEE,0xEEBD,0xDBEE,0xEBD3,0xDBEE,0xEBD3,
	0xBEEE,0xEEBD,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,0xEEEE,
	0xC8C8,0xCCCC,0xCC88,0xCCCC,0xCC8E,0xCCCC,0x888E,0x8888,
	0xDBEE,0xEEEB,0x3BEE,0xEEEB,0xBBEE,0xEEEB,0xEEEE,0xEEEE,

	0xCCCC,0xE8CC,0xCCCC,0xE8CC,0xCCCC,0xEECC,0x8888,0xEE88,
	0xDBEE,0xEEEB,0x3BEE,0xEEEB,0xBBEE,0xEEEB,0xEEEE,0xEEEE,
	0xF444,0xFFFF,0xCC44,0xCCCC,0xCC44,0xCCCC,0xC444,0xCCCC,
	0xC444,0xCFCC,0xC444,0xFCFC,0xC844,0xCFCC,0xCC84,0xFCFC,
	0xFFFF,0x44FF,0xCCCC,0x48CC,0xCCCC,0x48CC,0xCCCC,0x448C,
	0xCFCF,0x448C,0xFCFC,0x448C,0xCFCF,0x488C,0xFCFC,0x8C8C,
	0xCC84,0xCCCC,0xCC84,0xCCCC,0xCC44,0xCCCC,0x8844,0x8888,
	0xB444,0x44BD,0xB444,0x44B3,0xB444,0x44BB,0x4444,0x4444,

	0xCCCC,0x8C8C,0xCCCC,0x88CC,0xCCCC,0x48CC,0x8888,0x4888,
	0xB444,0x44BD,0xB444,0x44B3,0xB444,0x44BB,0x4444,0x4444,
};

// MAPS
const unsigned short QSCMap[1024] __attribute__((aligned(4)))=
{
	0x0001,0x0001,0x0002,0x0003,0x0003,0x0402,0x0001,0x0001,
	0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,
	0x0001,0x0001,0x0001,0x0001,0x0001,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0004,0x0004,0x0005,0x0006,0x0006,0x0405,0x0004,0x0004,
	0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,
	0x0004,0x0004,0x0004,0x0004,0x0004,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0007,0x0007,0x0008,0x0009,0x000A,0x0408,0x0007,0x000B,
	0x000C,0x000C,0x000C,0x000D,0x0007,0x0007,0x0007,0x0007,
	0x0007,0x0007,0x0007,0x0007,0x0007,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x000E,0x000E,0x000F,0x0010,0x0011,0x040F,0x000E,0x0012,
	0x0013,0x0013,0x0013,0x0014,0x000E,0x000E,0x000E,0x000E,
	0x000E,0x0015,0x0016,0x000E,0x000E,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0017,
	0x0018,0x0018,0x0018,0x0019,0x0006,0x0006,0x0006,0x0006,
	0x0006,0x001A,0x001B,0x0006,0x0006,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x001C,
	0x001D,0x001D,0x001D,0x001E,0x0006,0x001F,0x0020,0x0021,
	0x0022,0x0022,0x0022,0x0022,0x0023,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0024,0x0024,0x0024,0x0025,0x0026,0x0024,0x0024,0x0024,
	0x0024,0x0024,0x0024,0x0024,0x0024,0x0027,0x0028,0x0029,
	0x002A,0x002A,0x002A,0x002A,0x002B,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0024,0x0024,0x0024,0x002C,0x002D,0x0024,0x0024,0x0024,
	0x0024,0x0024,0x0024,0x0024,0x0024,0x002E,0x002F,0x0030,
	0x0031,0x0031,0x0031,0x0031,0x0032,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0006,0x0423,0x0022,0x0022,0x0022,0x0022,0x0421,0x0420,
	0x041F,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,
	0x0006,0x0006,0x0006,0x0006,0x0006,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0006,0x042B,0x002A,0x002A,0x002A,0x002A,0x0429,0x0033,
	0x0034,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,
	0x0006,0x0006,0x0006,0x0006,0x0006,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0024,0x0432,0x0031,0x0031,0x0031,0x0031,0x0430,0x042F,
	0x042E,0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,
	0x0024,0x0024,0x0024,0x0024,0x0024,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,
	0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,
	0x0024,0x0426,0x0425,0x0024,0x0024,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,
	0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,
	0x0006,0x001A,0x001B,0x0006,0x0006,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0006,0x0006,0x0006,0x0035,0x0036,0x0006,0x0006,0x0006,
	0x0006,0x0006,0x0006,0x0006,0x0006,0x001F,0x0020,0x0021,
	0x0022,0x0022,0x0022,0x0022,0x0023,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0024,0x0024,0x0024,0x002C,0x002D,0x0024,0x0024,0x0024,
	0x0024,0x0024,0x0024,0x0024,0x0024,0x0027,0x0028,0x0029,
	0x002A,0x002A,0x002A,0x002A,0x002B,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0024,0x0037,0x0038,0x0038,0x0038,0x0038,0x0039,0x003A,
	0x003B,0x0024,0x0024,0x0024,0x0024,0x002E,0x002F,0x0030,
	0x0031,0x0031,0x0031,0x0031,0x0032,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0006,0x042B,0x002A,0x002A,0x002A,0x002A,0x0429,0x0033,
	0x0034,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,
	0x0006,0x003C,0x003D,0x0006,0x0006,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0006,0x003E,0x003F,0x003F,0x003F,0x003F,0x0040,0x0041,
	0x0042,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,
	0x0006,0x0043,0x0044,0x0006,0x0006,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0024,0x0024,0x0024,0x0045,0x0046,0x0024,0x0024,0x0024,
	0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,
	0x0024,0x0024,0x0024,0x0024,0x0024,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0024,0x0024,0x0024,0x0047,0x0048,0x0024,0x0024,0x0024,
	0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,0x0024,
	0x0024,0x0024,0x0024,0x0024,0x0024,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,
	0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,
	0x0006,0x0006,0x0006,0x0006,0x0006,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,

	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
};

// PALETTES
const unsigned short QSCPal[16] __attribute__((aligned(4)))=
{
	0x0000,0x31AE,0x5B18,0x14A6,0x635B,0x4A73,0x77BD,0x21B2,
	0x0C6B,0x3A35,0x2129,0x3DEF,0x18D0,0x4EB5,0x5719,0x2976,
};
