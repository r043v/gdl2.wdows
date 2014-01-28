// transparent blit routine for Gdl²
// (C) 2k4/2k6 by r043v, cannot be used for any commercial purpose

#include "../../../Gdl.h"

clDeep *scrStart,*scrEnd,*screen ;
int frmWidth, frmWidthStart, frmWidthEnd;

clDeep *saved_scrStart=0, *saved_scrEnd=0, *saved_screen=0 ;
u32 saved_frmWidth=0, saved_frmWidthStart=0, saved_frmWidthEnd=0;

clDeep *isaved_scrStart=0, *isaved_scrEnd=0, *isaved_screen=0 ;
u32 isaved_frmWidth=0, isaved_frmWidthStart=0, isaved_frmWidthEnd=0;

void fullBlitLimit(void)
{	scrStart  = pixel;
    scrEnd    = &pixel[WIDTH*HEIGHT-1];
    frmWidth  = WIDTH;
    frmWidthStart = 0;
    frmWidthEnd   = WIDTH;
}

void saveBlitLimit(void)
{   saved_scrStart  = scrStart;
    saved_scrEnd    = scrEnd;
    saved_frmWidth  = frmWidth;
    saved_frmWidthStart = frmWidthStart;
    saved_frmWidthEnd   = frmWidthEnd;
}

void loadBlitLimit(void)
{   //if(!saved_scrStart) resetBlitLimit();
    scrStart  = saved_scrStart;
    scrEnd    = saved_scrEnd;
    frmWidth  = saved_frmWidth;
    frmWidthStart = saved_frmWidthStart;
    frmWidthEnd   = saved_frmWidthEnd;
}

void internalSaveBlitLimit(void)
{   isaved_scrStart  = scrStart;
    isaved_scrEnd    = scrEnd;
    isaved_frmWidth  = frmWidth;
    isaved_frmWidthStart = frmWidthStart;
    isaved_frmWidthEnd   = frmWidthEnd;
}

void internalLoadBlitLimit(void)
{   //if(!saved_scrStart) resetBlitLimit();
    scrStart  = isaved_scrStart;
    scrEnd    = isaved_scrEnd;
    frmWidth  = isaved_frmWidth;
    frmWidthStart = isaved_frmWidthStart;
    frmWidthEnd   = isaved_frmWidthEnd;
}

void setBlitLimit(clDeep *start, clDeep *end, u32 widthStart, u32 widthEnd)
{      scrStart  = start;
       scrEnd    = end  ;
       frmWidth  = widthEnd-widthStart;
       frmWidthStart = widthStart;
       frmWidthEnd   = widthEnd;
}

void setBlitLimit(u32 x, u32 y, u32 x1, u32 y1)
{    u32 width = x1 - x;
     scrStart  = &pixel[x+y*WIDTH];
     scrEnd    = &pixel[x1+y1*WIDTH];
     frmWidth  = width;
     frmWidthStart = x;
     frmWidthEnd   = x1;
}

void drawGfm(clDeep *Gfm, int x, int y)
{    
     #ifdef use32b
      int sx = Gfm[2]>>16 ; u32 sy = Gfm[2]&65535 ;
     #else
      int sx = Gfm[0]; int sy = Gfm[1];
     #endif

     if(x >= frmWidthStart && x + sx < frmWidthEnd)
		{		udrawGfm(Gfm,&pixel[y*WIDTH+x]) ;
                return;
        }

     if(x < (frmWidthStart-sx) || x > frmWidthEnd) return ;// out of screen on x
     
     if(x >= frmWidthStart) // clip right only
     {   u32 max = frmWidthEnd-x ;  u32 lnb, sze ;
         clDeep *scr = &pixel[y*WIDTH+x] ;
         if(scr > scrEnd) return ; // out of screen at down
         
         u32 upClip = (scr + sy*WIDTH > scrEnd) ; // is clipped at down ?

         #ifdef use32b
          Gfm += 3;
         #else
          Gfm += 2;
         #endif
         
         if(scr < scrStart) // is clipped at up ?
         {  if(scr + sy*WIDTH < scrStart) return ; // out of screen at up
            do{ lnb = *Gfm++ ;
                while(lnb--) { Gfm++ ; Gfm += *Gfm++ ; };
                scr += WIDTH ; sy-- ;
            } while(scr < scrStart) ;
         }

         clDeep *screen = scr ; u32 c = 0 ; clDeep * lend ;
         while(c < sy)
         {  lnb = *Gfm++ ; lend = scr + max ;
            while(lnb--) { scr += *Gfm++ ; sze = *Gfm++ ;
                           if(scr + sze < lend) memcpy(scr,Gfm,sze<<clDeepDec) ;
                           else if(scr < lend)  memcpy(scr,Gfm,(lend-scr)<<clDeepDec) ;
                           Gfm += sze ;    scr+=sze ;
            };  scr = screen + WIDTH*(++c) ;
            if(upClip) if(scr > scrEnd) return ;
         };
     } else if(x+sx < frmWidthEnd) { // clip left only
         u32 lnb, sze ; clDeep *s = &pixel[y*WIDTH + frmWidthStart] ;

         if(s > scrEnd) return ; // out of screen at up
         u32 upClip = (s + sy*WIDTH > scrEnd) ; // is clipped at up ?

         #ifdef use32b
          Gfm += 3;
         #else
          Gfm += 2;
         #endif         

         if(s < scrStart) // is clipped at down ?
         {  if(s + sy*WIDTH < scrStart) return ; // out of screen at down
            do{ lnb = *Gfm++ ;
                while(lnb--) { Gfm++ ; Gfm += *Gfm++ ; };
                s += WIDTH ; sy-- ;
            } while(s < scrStart) ;
         }
         
         clDeep*scr = s+x ;
         scr -= frmWidthStart ;
         clDeep *screen = scr ;
         u32 c=0 ; u32 size ;
         
         while(c < sy)
         {  lnb = *Gfm++ ;
            while(lnb--) { scr += *Gfm++ ; sze = *Gfm++ ;
                           if(scr >= s)   memcpy(scr,Gfm,sze<<clDeepDec) ;
                           else if(scr + sze > s) { size = (scr + sze)-s ;
                                                    memcpy(s,Gfm+(sze-size),size<<clDeepDec) ;
                                                  }
                           Gfm += sze ;    scr+=sze ;
            };  scr = screen + WIDTH*(++c) ; s = scr+frmWidthStart ; s -= x ;
            if(upClip) if(s > scrEnd) return ;
         };
     } else return ;
}


void udrawGfm(clDeep*Gfm,clDeep*scr)
{
   #ifdef use32b
    Gfm+=2 ; u32 sy = (*Gfm++)&65535 ;
   #else 
    Gfm++; u32 sy = *Gfm++;
   #endif
    
    u32 lnb, sze ;
    
    if(scr > scrEnd) return ; // out of screen at up
    u32 upClip = (scr + sy*WIDTH > scrEnd) ; // is clipped at up ?

    if(scr < scrStart) // is clipped at down ?
    {  if(scr + sy*WIDTH < scrStart) return ; // out of screen at down
       do{    lnb = *Gfm++ ;
              while(lnb--) { Gfm++ ; Gfm += *Gfm++ ; };
              scr += WIDTH ; sy-- ;
       } while(scr < scrStart) ;
    }

    clDeep *screen = scr ;
    u32 c = 0 ;
    
    if(upClip)
     {  while(c < sy)
        {  lnb = *Gfm++ ;
           while(lnb--) { scr += *Gfm++ ; sze = *Gfm++ ;
                          while(sze>1)
                            { *scr++ = *Gfm++;
                              *scr++ = *Gfm++;
                              sze-=2;
                            };
                          if(sze) *scr++ = *Gfm++;
           };  scr = screen + WIDTH*(++c) ;
           if(scr > scrEnd) return ;
        };
     }
    else
     {  while(c < sy)
        {  lnb = *Gfm++ ;
           while(lnb--) { scr += *Gfm++ ; sze = *Gfm++ ;
                          
                          while(sze>1)
                            { *scr++ = *Gfm++;
                              *scr++ = *Gfm++;
                              sze-=2;
                            };
                          if(sze) *scr++ = *Gfm++;
           };  scr = screen + WIDTH*(++c) ;
        };
     }
} 

void udraw4bGfm(u8*Gfm,clDeep*scr,clDeep*mypal)
{

  Gfm+=3 ;  u32 clNum=*Gfm++ ; Gfm+=4 ;
  u32 sx   = *(u16*)Gfm ; Gfm+=2 ;
  u32 sy   = *(u16*)Gfm ; Gfm+=2 ;
  Gfm+=4 ;
  u32*pal;
  if(mypal) pal = mypal;
   else		pal = (u32*)Gfm;
  Gfm+=(4*clNum);

  u32 cnt=0, c, jump, size, p1, p2 ;

  //printf("\n\nout size : %i\n%i colors { %x",outSize,clNum,*pal) ;
  //for(int c=1;c<clNum;c++) printf(",%x",pal[c]) ; printf(" }\nsize : %i*%i",sx,sy) ;

	clDeep * s;

  while(cnt++ < sy)
  {    c = *Gfm++ ; //printf("\n* line %i, %i sublines",cnt,c) ;
       s = scr;
	   while(c--) { jump = *Gfm++ ;
					size = *Gfm++ ;
					scr+=jump;
                    while(size > 1) { p1 = (*Gfm)>>4 ; p2 = (*Gfm)&15 ;
                                      size-=2 ;
									  *scr++ = pal[p1] ; *scr++ = pal[p2] ;
                                      ++Gfm ;
                                    };
                    if(size!=0) { //printf(" + %x",*data);
									*scr++ = pal[*Gfm++] ;
								}
                  };
	   scr = s+WIDTH;
  };
  /*

   #ifdef use32b
    Gfm+=2 ; u32 sy = (*Gfm++)&65535 ;
   #else 
    Gfm++; u32 sy = *Gfm++;
   #endif
    
    u32 lnb, sze ;
    
    if(scr > scrEnd) return ; // out of screen at up
    u32 upClip = (scr + sy*WIDTH > scrEnd) ; // is clipped at up ?

    if(scr < scrStart) // is clipped at down ?
    {  if(scr + sy*WIDTH < scrStart) return ; // out of screen at down
       do{    lnb = *Gfm++ ;
              while(lnb--) { Gfm++ ; Gfm += *Gfm++ ; };
              scr += WIDTH ; sy-- ;
       } while(scr < scrStart) ;
    }

    clDeep *screen = scr ;
    u32 c = 0 ;
    
    if(upClip)
     {  while(c < sy)
        {  lnb = *Gfm++ ;
           while(lnb--) { scr += *Gfm++ ; sze = *Gfm++ ;
                          while(sze>1)
                            { *scr++ = *Gfm++;
                              *scr++ = *Gfm++;
                              sze-=2;
                            };
                          if(sze) *scr++ = *Gfm++;
           };  scr = screen + WIDTH*(++c) ;
           if(scr > scrEnd) return ;
        };
     }
    else
     {  while(c < sy)
        {  lnb = *Gfm++ ;
           while(lnb--) { scr += *Gfm++ ; sze = *Gfm++ ;
                          
                          while(sze>1)
                            { *scr++ = *Gfm++;
                              *scr++ = *Gfm++;
                              sze-=2;
                            };
                          if(sze) *scr++ = *Gfm++;
           };  scr = screen + WIDTH*(++c) ;
        };
     }*/
} 

#ifdef use16b

 // convert a 4b Gfm to a 16b Gfm
 
u16 * data2Gfm(u8*data) // convert a 4b Gfm to a 16b Gfm
{ data+=3 ;  u32 clNum=*data++ ; data+=4 ;
  u32 sx   = *(u16*)data ; data+=2 ;
  u32 sy   = *(u16*)data ; data+=2 ;
  u32 outSize = *(u32*)data ; data+=4 ;
  u16 pal[16];
  //outSize-=12;
   outSize>>=1; // 16b size is 12b and 2x smaller than 32b Gfm

  u16 *Gfm =  (u16*)malloc(outSize) ;

  {  int*pal32 = (int*)data;
     for(u32 n=0;n<clNum;n++)
           pal[n] = pc2gpColor(pal32[n]);
  }

  data+=(4*clNum);
  u16 *o = Gfm;
  u32 cnt=0, c, jump, size, p1, p2 ;

  *o++ = sx;
  *o++ = sy;

  while(cnt++ < sy)
  {    *o++ = c = *data++ ; //printf("\n* line %i, %i sublines",cnt,c) ;
       while(c--) { jump = *data++ ; size = *data++ ; *o++ = jump ; *o++ = size ;
                    while(size > 1) { p1 = (*data)>>4 ; p2 = (*data)&15 ;
                                      size-=2 ; *o++ = pal[p1] ; *o++ = pal[p2] ;
                                      ++data ;
                                    };
                    if(size) *o++ = pal[(*data++)] ;
                  };
  };  return Gfm ;
}

#else



clDeep * Gfm2array(clDeep *Gfm)
{ int sx = Gfm[2]>>16, sy = Gfm[2]&0xffff ;
  clDeep *array=(clDeep *)malloc(sx*sy*4) ; clDeep *o=array ;
  memset(array,0xFF,sx*sy*4) ;
  int cnt=0, c ; clDeep *gPtr = Gfm + 3 ;
  int size ;
  while(cnt<sy)
  {    c = *gPtr++ ;
       while(c--) { array += *gPtr++ ; size = *gPtr++ ;
                    memcpy(array,gPtr,size*4) ;
                    gPtr+=size ; array+=size ;
                  }; array = o + sx*(++cnt) ;
  }; return o ;
}

 // convert a 4b Gfm to a 32b Gfm
clDeep * data2Gfm(unsigned char *data)
{ //unsigned char *d = data ;
  data+=3 ; u32 clNum=*data++ ; data+=4 ;
  u32 sx   = *(short*)data ; data+=2 ;
  u32 sy   = *(short*)data ; data+=2 ;
  int outSize = *(int*)data ; data+=4 ;
  //printf("\n%i colors %i*%i out size %i",clNum,sx,sy,outSize) ;
  clDeep *Gfm =  (clDeep*)malloc(outSize) ;
  clDeep *pal =  (clDeep*)malloc(4*clNum) ;
  memcpy(pal,data,4*clNum) ; data+=(4*clNum) ;
  clDeep *o = Gfm ;
  u32 cnt=0, c, jump, size, p1, p2 ;

  //printf("\n\nout size : %i\n%i colors { %x",outSize,clNum,*pal) ;
  //for(int c=1;c<clNum;c++) printf(",%x",pal[c]) ; printf(" }\nsize : %i*%i",sx,sy) ;

  *o++ = 0x6d6647 ;              // put signature "Gfm\0"
  *o++ = outSize ;               // put Gfm object size
  *o++ = sx<<16 | (sy & 65535) ; // put frame size x and y

  while(cnt++ < sy)
  {    *o++ = c = *data++ ; //printf("\n* line %i, %i sublines",cnt,c) ;
       while(c--) { jump = *data++ ; size = *data++ ; *o++ = jump ; *o++ = size ;
                    //printf("\n jmp %i sze %i | ",jump,size) ;
                    if(jump > sx || size > sx) {
                      //printf("\njump or size error ... pos %i",data-d) ;
                      //return 0 ;
                    }

                    while(size > 1) { p1 = (*data)>>4 ; p2 = (*data)&15 ;
                                      //printf(",%x,%x",p1,p2) ;
                                      size-=2 ; *o++ = pal[p1] ; *o++ = pal[p2] ;

                                      if(p1 >= clNum || p2 >= clNum) {
                                       //printf("\ndata error, out of pal ! ... pos %i ... data %i | %x | %c",data-d,*data,*data,*data) ;
                                        //return 0 ;
                                      }

                                      ++data ;
                                    };
                    if(size!=0) { //printf(" + %x",*data);
									*o++ = pal[(*data++)] ;
								}
                  };
  };
  *o = 0x2a2a2a2a ;
  addFreeEntry(Gfm);
  return Gfm ;
}

#endif

/*void logCldArray(u8 *Ar)
{ int sx=*Ar++, sy=*Ar++;
  printl("\n%i*%i",sx,sy);
  for(int y=0;y<sy;y++)
  { printl("\n");
    for(int x=0;x<sx;x++)
     if(Ar[x+y*sx]) printl("%i",Ar[x+y*sx]) ;
      else printl(" ");
  };
}*/

void unCrunchGfm(clDeep ** gfm, u32 frmNb)
{	for(u32 c=0;c<frmNb;c++)
		gfm[c] = data2Gfm((u8*)(gfm[c]));
}

// do a box & a pixel by pixel test to detect colide (between 2 colide buffer, see a bit highter)
int colide(u8*s1,int x1,int y1,u8*s2,int x2,int y2)
{ int sx1=*s1++, sy1=*s1++ ;
  int sx2=*s2++, sy2=*s2++ ;

  if(x2<=x1+sx1 && x2+sx2>=x1)
   if(y2<=y1+sy1 && y2+sy2>=y1)
    {  int x,y ; // we are here if there is a box colide
       int spr1x=0,spr1y=0,spr2x=0,spr2y=0,szx,szy ;
        // compute the colide box
		if(x1 < x2) spr1x = x2 - x1 ;
		else if(x2 < x1) spr2x = x1 - x2 ;
			
		if(y1 < y2) spr1y = y2 - y1 ;
		else if(y2 < y1) spr2y = y1 - y2 ;
			
		if(x1 + sx1 >= x2 + sx2)
		{	if(x1 > x2) szx = (x2 + sx2) - x1 ;
			 else szx = sx2 ;
		} else if(x1 + sx1 < x2 + sx2) szx = (x1 + sx1) - x2 ;
			
		if(y1 + sy1 >= y2 + sy2)
		{	if(y1 > y2) szy = (y2 + sy2) - y1 ;
			 else szy = sy2 ;
		} else if(y1 + sy1 < y2 + sy2) szy = (y1 + sy1) - y2 ;

		if(szx > sx1) szx=sx1 ;
		if(szy > sy1) szy=sy1 ;

        u8*ps1 = s1+spr1x+spr1y*sx1 ;
        u8*ps2 = s2+spr2x+spr2y*sx2 ;

        // and do the pixel/pixel test
       	for(x=0;x<szx;x++)
       	 for(y=0;y<szy;y++)
		  if(ps1[x+y*sx1] && ps2[x+y*sx2]) return 1 ;
    }  return 0 ;
}

u8 *Gfm2cldArray(int *Gfm) // create a raw buffer used to compute colide (yet not a 1bit buffer..)
{ u32 sx = Gfm[2]>>16, sy = Gfm[2]&0xffff ;
  u8 *array=(u8*)malloc(sx*sy+2) ; u8 *o=array+2 ;
  *array++=sx; *array++=sy;  memset(o,0,sx*sy) ; // take care, sprite size must be < to 256 pixels!
  u32 cnt=0, c, size ; int *gPtr = Gfm + 3 ;
  while(cnt<sy)
  {    c = *gPtr++ ;
       while(c--) { array += *gPtr++ ; size = *gPtr++ ;
                    memset(array,1,size) ;
                    gPtr+=size ; array+=size ;
                  }; array = o + sx*(++cnt) ;
  };

  //logCldArray(o-2);
  return o-2 ;
}
