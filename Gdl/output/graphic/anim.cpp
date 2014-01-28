
#ifndef _gdlAnim_
#define _gdlAnim_

#include "../../Gdl.h"

int Animate(struct anim **b) // check for time and animate if need, not draw
{   struct anim *a = *b ;
	if(a->lastTime + a->frmTime < tick)
	{	if(++(a->curentFrm) >= a->frmNumber)
        {    switch(a->animType)
             { case 0 : a->curentFrm = 0 ; break ; /* loop anim */
               case 1 : a->curentFrm = a->frmNumber-1 ; break ; /* stop at last frame */
             };
             if(a->onfinish) (a->onfinish)(b) ;
        } else if(a->onflip) (a->onflip)(b) ;
		a->lastTime = tick ; return 1 ;
	} else if(a->onplay) a->onplay(b) ;
    return 0 ;
}

void playAnim(struct anim **b,clDeep * screen)//, int way)
{	struct anim *a = *b ;    Animate(b) ;
	udrawGfm((a->Gfm)[a->curentFrm],screen) ;
}

void playAnim(struct anim **b, int x, int y, u32 way)
{   struct anim *a = *b ;    Animate(b) ;
	if(!way) drawGfm((a->Gfm)[a->curentFrm],x,y) ;
     //else   drawGfm((a->iGfm)[a->curentFrm],x,y) ;
}

void drawAnim(struct anim **b, int x, int y, u32 way)
{  	struct anim *a = *b ;
    if(!way) drawGfm((a->Gfm)[a->curentFrm],x,y) ;
     //else   drawGfm((a->iGfm)[a->curentFrm],x,y) ;
}

void drawFrm(struct anim **b, int x, int y, int frm, u32 way)
{   struct anim *a = *b ;
    if(!way) drawGfm((a->Gfm)[frm],x,y) ;
     //else   drawGfm((a->iGfm)[frm],x,y) ;
}

void ifreeAnim(struct anim *a)
{ if(!(a->iGfm)) return ;
  for(u32 c=0;c<a->frmNumber;c++) free((a->iGfm)[c]) ;
}

void mifreeAnim(struct anim **a, u32 nb)
{ for(u32 c=0;c<nb;c++) ifreeAnim(a[c]) ; }

struct anim * setAnim( clDeep **Gfm, u32 nb,
                       u32 frmTime, u32 animType,
                       void (*onfinish)(struct anim**),
                       void (*onflip)(struct anim**),
                       void (*onplay)(struct anim**)
                     )
{	struct anim *a = (struct anim *)malloc(sizeof(struct anim)) ;
	a->Gfm = Gfm ;	a->frmNumber = nb ; // a->iGfm = flipGfm(Gfm,nb) ;
	a->curentFrm = a->lastTime = 0 ;
    a->onplay=onplay ; a->onflip=onflip ; a->onfinish=onfinish ;
	a->frmTime = frmTime ; a->animType = animType ;
    return a ;
}

void resetAnim(struct anim **a)
{ (*a)->curentFrm=0 ; (*a)->lastTime = tick ; }

#endif
