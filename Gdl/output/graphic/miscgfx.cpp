
#ifndef _gdlMiscGfx_
#define _gdlMiscGfx_

#include "../../Gdl.h"

void clrScr(clDeep color)
{	for(clDeep *scr = scrStart;scr <= scrEnd; scr++) *scr = color;
}

#endif