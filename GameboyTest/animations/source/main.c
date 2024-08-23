#include <tonc.h> // GBA library
#include "player.h" // for sprite data
#include <string.h> // for memcpy

#define PLAYER_TILE_ID    1
#define PLAYER_PAL_BANK   0
#define PLAYER_PRIO       0
#define NUM_SPRITES       1
#define PLAYER_WIDTH      16
#define PLAYER_HEIGHT     24
#define PLAYER_X          (SCREEN_WIDTH-PLAYER_WIDTH)/2
#define PLAYER_Y          (SCREEN_HEIGHT-PLAYER_HEIGHT)/2


OBJ_ATTR obj_attr_buf[NUM_SPRITES];  // create a set of obj attr structs
OBJ_ATTR *player = &obj_attr_buf[0]; // pointer to start of obj buffer

void walkHorz(){

  // isolate tile num (first 10 bits)
  switch (player->attr2 & 0x3FF)
  {

  case PLAYER_TILE_ID + 8*5:
    if(player->attr2 & ATTR2_PRIO(PLAYER_PRIO+1)){ // if prio was increased
      player->attr2 &= 0xf3ff; // clear prio
      player->attr2 |= ATTR2_PRIO(PLAYER_PRIO); // decrease prio
      player->attr2 &= 0xfc00; // clear tile num
      player->attr2 |= PLAYER_TILE_ID + 8*7;
    }
    else{
      player->attr2 &= 0xf3ff; // clear prio
      player->attr2 |= ATTR2_PRIO(PLAYER_PRIO+1); // increase prio
      player->attr2 &= 0xfc00; // clear tile num
      player->attr2 |= PLAYER_TILE_ID + 8*6;
    }
    break;

  default:
    player->attr2 &= 0xfc00; // clear first 10 bits
    player->attr2 |= PLAYER_TILE_ID+8*5;
  }

}

void walkRight(){
  player->attr1 &= ~ATTR1_HFLIP; // remove hflip from walkLeft()
  walkHorz();
}

void walkLeft(){
  player->attr1 |= ATTR1_HFLIP;
  walkHorz();
}

void walkDown()
{
  player->attr1 &= ~ATTR1_HFLIP; // remove hflip from walkUp()
  // isolate tile num (first 10 bits)
  switch (player->attr2 & 0x3FF)
  {

  case PLAYER_TILE_ID:
    if(player->attr2 & ATTR2_PRIO(PLAYER_PRIO+1)){ // if prio was increased
      player->attr2 &= 0xf3ff; // clear prio
      player->attr2 |= ATTR2_PRIO(PLAYER_PRIO); // decrease prio
      player->attr2 &= 0xfc00; // clear tile num
      player->attr2 |= PLAYER_TILE_ID + 16;
    }
    else{
      player->attr2 &= 0xf3ff; // clear prio
      player->attr2 |= ATTR2_PRIO(PLAYER_PRIO+1); // increase prio
      player->attr2 &= 0xfc00; // clear tile num
      player->attr2 |= PLAYER_TILE_ID + 8;
    }
    break;

  default:
    player->attr2 &= 0xfc00; // clear first 10 bits
    player->attr2 |= PLAYER_TILE_ID;
  }
}

void walkUp(){
  // isolate tile num (first 10 bits)
  switch (player->attr2 & 0x3FF)
  {
  case PLAYER_TILE_ID + 8*3:
    player->attr2 &= 0xfc00; // clear first 10 bits
    player->attr2 |= PLAYER_TILE_ID + 8*4;
    break;
  default:
    player->attr1 ^= ATTR1_HFLIP;
    player->attr2 &= 0xfc00; // clear first 10 bits
    player->attr2 |= PLAYER_TILE_ID + 8*3;
  }
}

void playGame(){

  key_poll();

  if(key_held(KEY_RIGHT)){
    walkRight();
  }

  if(key_held(KEY_LEFT)){
    walkLeft();
  }

  if(key_held(KEY_UP)){
    walkUp();
  }

  if(key_held(KEY_DOWN)){
    walkDown();
  }

}

void player_init(){
  memcpy32(&tile_mem[4][PLAYER_TILE_ID], playerTiles, playerTilesLen/4); // copy tiles
  memcpy32(pal_obj_mem+16*PLAYER_PAL_BANK, playerPal, playerPalLen/4); // copy pal

  oam_init(obj_attr_buf, NUM_SPRITES); // initialize sprite attr
  
  obj_set_attr(
    player,
    ATTR0_4BPP|ATTR0_TALL,
    ATTR1_SIZE_16x32,
    ATTR2_PALBANK(PLAYER_PAL_BANK)|ATTR2_PRIO(PLAYER_PRIO)|PLAYER_TILE_ID
  );

  obj_set_pos(player, PLAYER_X, PLAYER_Y); // set the objects position
}

int main()
{

  player_init();
  REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D;

  // enable isr switchboard and VBlank interrupt
  irq_init(NULL);
  irq_add(II_VBLANK, NULL);

  while(1)
  {
    for(int i=0; i<5; i++){
      VBlankIntrWait(); // vsync
    }

    playGame();
    oam_copy(oam_mem, obj_attr_buf, NUM_SPRITES); // copy sprite data to sys mem

  }
}