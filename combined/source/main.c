#include <tonc.h> // GBA library
#include "player.h" // for sprite data
#include "background.h"

// player defines
#define PLAYER_TILE_ID    1
#define PLAYER_PAL_BANK   0
#define PLAYER_PRIO       0
#define PLAYER_WIDTH      16
#define PLAYER_HEIGHT     32
#define PLAYER_X          (SCREEN_WIDTH-PLAYER_WIDTH)/2
#define PLAYER_Y          (SCREEN_HEIGHT-PLAYER_HEIGHT)/2

#define NUM_SPRITES       1


OBJ_ATTR obj_attr_buf[NUM_SPRITES];  // create a set of obj attr structs
OBJ_ATTR *player = &obj_attr_buf[0]; // pointer to start of obj buffer

int map_x = 0, map_y = 0, prevFrame=0;

int three_frame_flip(int prevFrame, int frame1, int frame2, int frame3)
{
  if(on_frame(frame1))
  {
    int newFrame;
    if(prevFrame == frame2) newFrame = frame3;
    else newFrame = frame2;
    set_frame(newFrame);
    return frame1;
  }

  else if (on_frame(frame2))
  {
    set_frame(frame1);
    return frame2;
  }

  else if (on_frame(frame3))
  {
    set_frame(frame1);
    return frame3;
  }

  else
  {
    set_frame(frame3);
    return frame1;
  }
}

int walkRight(int prevFrame)
{
  player->attr1 &= ~ATTR1_HFLIP; // ensure not flipped
  return three_frame_flip(prevFrame,5,6,7);
}

int walkLeft(int prevFrame)
{
  player->attr1 |= ATTR1_HFLIP; // flip sprite
  return three_frame_flip(prevFrame,5,6,7);
}

int walkDown(int prevFrame){
  player->attr1 &= ~ATTR1_HFLIP; // ensure not flipped
  return three_frame_flip(prevFrame,0,1,2);
}

int walkUp(int prevFrame){
  if(on_frame(4))
  {
    player->attr1 ^= ATTR1_HFLIP;
    set_frame(3);
  }
  else set_frame(4);
  return 3;
}

void walkAnim(int facingDir, int x, int y){
  
  int frameCount = 8;
  if(facingDir) frameCount = 16;
  int eighthFrame = frameCount/8;
  int halfFrame = frameCount/2;
  
  for(int i = 0; i<frameCount; i++)
  {
    if(i%eighthFrame == 0 && facingDir)
    {
      if(x) map_x+=x;
      if(y) map_y-=y;
    }

    if(i%halfFrame == 0){
      if(x==1) prevFrame=walkRight(prevFrame);
      if(x==-1) prevFrame=walkLeft(prevFrame);
      if(y==1) prevFrame=walkUp(prevFrame);
      if(y==-1) prevFrame=walkDown(prevFrame);
      oam_copy(oam_mem, obj_attr_buf, NUM_SPRITES);
    }

    REG_BG0HOFS = map_x;
    REG_BG0VOFS = map_y;
    VBlankIntrWait();
  }
}

void walk(){
    
  // moving vertical
  if(key_tri_vert())
  {

    // move up
    if(key_is_down(KEY_UP))
    {
      
      if( on_frame(3) )
      {
        walkAnim(1, 0, 1);
      }

      else
      {
        walkAnim(0, 0, 1);
      }

    }

    // move down
    else
    {
      if (on_frame(0) && sprite_flipped() == 0)
      {
        walkAnim(1, 0, -1);
      }
      else
      {
        walkAnim(0, 0, -1);
      }
    }


  }

  // moving horizontal
  else
  {
    // move right
    if(key_is_down(KEY_RIGHT))
    {

      if( on_frame(5) && sprite_flipped() == 0 )
      {
        walkAnim(1, 1, 0);
      }

      else
      {
        walkAnim(0, 1, 0);
      }

    }

    //move left
    else
    {

      if( on_frame(5) && sprite_flipped() )
      {
        walkAnim(1, -1, 0);
      }

      else
      {
        walkAnim(0, -1, 0);
      }
      
    }

  }
  
}

void playGame(){

  VBlankIntrWait(); // vsync
  key_poll();

  if(key_tri_horz() | key_tri_vert())
  {
    walk();
  }

  oam_copy(oam_mem, obj_attr_buf, NUM_SPRITES);

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
  REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D;
}

void background_init(){
  
  memcpy32(pal_bg_bank, backgroundPal, backgroundPalLen/4);
  memcpy32(tile_mem, backgroundTiles, backgroundTilesLen/4);
  memcpy32(&se_mem[31], backgroundMap, backgroundMapLen/4);

  REG_BG0CNT = BG_CBB(0) | BG_SBB(31) | BG_4BPP | BG_REG_32x32 | BG_PRIO(2);
  REG_DISPCNT |= DCNT_MODE0 | DCNT_BG0;
}

int main()
{

  player_init();
  background_init();

  // enable isr switchboard and VBlank interrupt
  irq_init(NULL);
  irq_add(II_VBLANK, NULL);

  while(1)
  {
    

    playGame();
    

  }
}