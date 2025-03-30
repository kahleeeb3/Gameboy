#ifndef OBJ_ANIM
#define OBJ_ANIM

#include <tonc.h>
#include "objData.h"

// add useful stuff missing from tonc
INLINE void obj_set_tile(OBJ_ATTR *obj, u16 tile_id)
{  BFN_SET(obj->attr2, tile_id, ATTR2_ID);  }

INLINE u16 obj_get_tile(OBJ_ATTR *obj)
{  return BFN_GET2(obj->attr2, ATTR2_ID);  }

INLINE u8 obj_cmp_tile(OBJ_ATTR *obj, u16 tile_id)
{  return BFN_CMP(obj->attr2, tile_id, ATTR2_ID);  }

INLINE u8 obj_is_hflip(OBJ_ATTR *obj)
{  return BFN_CMP(obj->attr1, ATTR1_HFLIP, ATTR1_FLIP);  }

INLINE u8 obj_is_vflip(OBJ_ATTR *obj)
{  return BFN_CMP(obj->attr1, ATTR1_VFLIP, ATTR1_FLIP);  }
// ==================================

#define OBJ_FRAME(n, f0) f0+(8*n)

void walk(OBJ *obj);

#endif