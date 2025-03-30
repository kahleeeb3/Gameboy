#include "animations.h"

void three_frame_flip(OBJ *obj, u8 f1, u8 f2, u8 f3)
{
  if( obj_cmp_tile(obj->obj_attr, OBJ_FRAME(f1, obj->t0)) )
  {
    if(obj->fp == f2)
    {
      obj_set_tile(obj->obj_attr, OBJ_FRAME(f3, obj->t0));
    }
    else
    {
      obj_set_tile(obj->obj_attr, OBJ_FRAME(f2, obj->t0));
    }
    obj->fp = f1;
  }
  
  else if ( obj_cmp_tile(obj->obj_attr, OBJ_FRAME(f2, obj->t0)) )
  {
    obj_set_tile(obj->obj_attr, OBJ_FRAME(f1, obj->t0));
    obj->fp = f2;
  }

  else if( obj_cmp_tile(obj->obj_attr, OBJ_FRAME(f3, obj->t0)) )
  {
    obj_set_tile(obj->obj_attr, OBJ_FRAME(f1, obj->t0));
    obj->fp = f3;
  }

  else
  {
    obj_set_tile(obj->obj_attr, OBJ_FRAME(f3, obj->t0));
    obj->fp = f1;
  }

}

void walk_right(OBJ *obj)
{
  obj->obj_attr->attr1 &= ~ATTR1_HFLIP; // ensure not flipped
  three_frame_flip(obj, 5, 6, 7);
}

void walk_left(OBJ *obj)
{
  obj->obj_attr->attr1 |= ATTR1_HFLIP; // ensure flipped
  three_frame_flip(obj, 5, 6, 7);
}

void walk_down(OBJ *obj)
{
  obj->obj_attr->attr1 &= ~ATTR1_HFLIP; // ensure not flipped
  three_frame_flip(obj, 0, 1, 2);
}

void walk_up(OBJ *obj)
{

  if( obj_cmp_tile(obj->obj_attr, OBJ_FRAME(4, obj->t0)) )
  {
    obj->obj_attr->attr1 ^= ATTR1_HFLIP;
    obj_set_tile(obj->obj_attr, OBJ_FRAME(3, obj->t0));
    return;
  }
  obj_set_tile(obj->obj_attr, OBJ_FRAME(4, obj->t0));
  return;

}

int x = 0, y = 0;

void walk(OBJ *obj) 
{

  // determine if facing correct direction
  u8 facing_correct_dir = 0;
  if(key_is_down(KEY_UP) && obj_cmp_tile(obj->obj_attr, OBJ_FRAME(3, obj->t0))) facing_correct_dir = 1;
  else if (key_is_down(KEY_DOWN) && obj_cmp_tile(obj->obj_attr, OBJ_FRAME(0, obj->t0))) facing_correct_dir = 1;
  else if (key_is_down(KEY_LEFT) && obj_cmp_tile(obj->obj_attr, OBJ_FRAME(5, obj->t0)) && obj_is_hflip(obj->obj_attr)) facing_correct_dir = 1;
  else if (key_is_down(KEY_RIGHT) && obj_cmp_tile(obj->obj_attr, OBJ_FRAME(5, obj->t0)) && !obj_is_hflip(obj->obj_attr)) facing_correct_dir = 1;


  int frame, seg = 16;
  for(frame=0; frame<seg; frame++)
  {

    if(frame == seg/2+1 && !facing_correct_dir) break;

    if(!(frame%(seg/2)))
    {
      if(key_is_down(KEY_UP)) walk_up(obj);
      else if (key_is_down(KEY_DOWN)) walk_down(obj);
      else if (key_is_down(KEY_LEFT)) walk_left(obj);
      else if (key_is_down(KEY_RIGHT)) walk_right(obj);
    }
    if(!(frame%(seg/8)) && facing_correct_dir)
    {
      if(key_tri_vert())
      {
        y += key_tri_vert();
        REG_BG0VOFS = y;
      }
      else
      {
        x += key_tri_horz();
        REG_BG0HOFS = x;
      }
      
    }

    oam_copy(oam_mem, obj_attr_buf, NUM_OBJ);
    VBlankIntrWait(); // vsync
  }
}