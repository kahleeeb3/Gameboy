#include <tonc.h> // GBA library
#include "objData.h" // obj data and attr
#include "bgData.h" // bg data and attr
#include "animations.h" // animating bg and obj

INLINE u8 any_key_pressed()
{   return (~REG_KEYINPUT & KEY_MASK); }

void playGame()
{

  key_poll();

  if(key_tri_vert() | key_tri_horz())
    walk(player);
}

int main()
{

  // create obj
  oam_init(obj_attr_buf, NUM_OBJ);
  player_init();
  oam_copy(oam_mem, obj_attr_buf, NUM_OBJ);
  REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D;

  // create background
  QSC_init();
  REG_DISPCNT |= DCNT_MODE0 | DCNT_BG0;

  // enable isr switchboard and VBlank interrupt
  irq_init(NULL);
  irq_add(II_VBLANK, NULL);

  while(1)
  {
    playGame();
  }
}