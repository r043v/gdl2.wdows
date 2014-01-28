#ifndef _gdlmisc_
#define _gdlmisc_

#include "../Gdl.h"

u16 pc2gpColor(u32 color)
{      register u32 r,g,b;
       r = (color>>19)&0x1f;
       g = (color>>11)&0x1f;
       b = (color>> 3)&0x1f;
       return (r<<11)|(g<<6)|(b<<1)|1;  
}

// count frame per second .. only launch this routine one time per frame !
u32 countFps(void)
{  	static u32 tm=0, fps=0, fps_count=0 ;
	u32 time = GetTickCount() ;
  	fps++ ;  if(tm + 500 < time) { fps_count=fps<<1; fps=0; tm=time; }
    return fps_count ;
}

u32 computeDec(u32 value)
{	u32 v=1, dec=0;
	while(v<value) { v<<=1; dec++; }
	return dec ;
}

u32 divide(u32 divadeWhat, u32 byHowMany)
{	if(!byHowMany) return 0;
	u32 nb=0;
	while(divadeWhat>=byHowMany)
		{	divadeWhat-=byHowMany;
			nb++;
		};
	return nb;
}

void showCursor(u32 x, u32 y,u32 x1, u32 y1, u32 size, u32 size1, int color, int color1,u32 time)
{	u32 sx = abs(x1-x);
	u32 sy = abs(y1-y);

	static u32 counter = 0;	// <127 for colored, >127 for transparent
	u32 way = 0 ;		// 0 line, 1 colon
	static u32 lastTime=0;
	u32 pixelValue = 0; // 1 for transparent
	u32 tsize = size+size1;

	if(tick-lastTime > time)
	{	if(++counter >= tsize)
		{	pixelValue = 0;
			counter=0;
		} else if(counter >= size) pixelValue = 1;
		lastTime = tick;
	}

	u32 c = counter ;

	clDeep * startScreen = pixel + x+y*WIDTH ;
	clDeep * screen = startScreen ;
	do { 	u32 w = way;
			c++;
			if(c >= size) pixelValue=1 ;
			if(c >= tsize) { c=0; pixelValue=0; }
			
			if(pixelValue)	*screen = color1;
			else			*screen = color;
		
			switch(w) // and jump into screen
			{	case 0 : // right
					if(++screen > startScreen+sx)
					{	screen--;
						screen+=WIDTH;
						way=1;
					}
					break;
				case 1 : // down
					screen += WIDTH;
					if(screen > startScreen+sy*WIDTH)
					{	screen-=WIDTH;
						screen--;
						way=2;
					}
					break;
				case 2 : // left
					if(--screen < startScreen+(sy*WIDTH)-WIDTH)
					{	screen++;
						screen-=WIDTH;
						way=3;
					}
					break;
				case 3 : // up
					screen-=WIDTH;
					if(screen < startScreen)
						return ;
						way=3;
					break;
			};
	} while(screen > startScreen);
}

// ------------------ //

/* this code is not mine,
 just get the Bresenham line algorithm and change it to directly poke into the framebuffer,
 also change it to blit a colored line instead of a single color
 yet also add clipping
*/

#define myabs(n) if(n<0)n=-n

void ligne(int xi,int yi,int xf,int yf, clDeep *line) {
clDeep *p = pixel + xi + yi*WIDTH ;
int dx,dy,i,xinc,yinc,cumul,x,y ;
x = xi ;
y = yi ;
dx = xf - xi ;
dy = yf - yi ;
xinc = ( dx > 0 ) ? 1 : -1 ;
yinc = ( dy > 0 ) ? 1 : -1 ;
dx = abs(dx) ;
dy = abs(dy) ;
*p = *line++ ;
if ( dx > dy ) {
  cumul = dx>>1 ;
  for ( i = 1 ; i <= dx ; i++ ) {
    p += xinc ;
    cumul += dy ;
    if (cumul >= dx) {
      cumul -= dx ;
      p += yinc*WIDTH ;
    }
    *p = *line++ ;     } }
  else {
  cumul = dy>>1 ;
  for ( i = 1 ; i <= dy ; i++ ) {
    p += yinc*WIDTH ;
    cumul += dx ;
    if ( cumul >= dy ) {
         cumul -= dy ;
         p += xinc ;
       }
    *p = *line++ ;
     } }
}



void _ligne(int xi,int yi,int xf,int yf, clDeep *line) {
clDeep *p = pixel + xi + yi*WIDTH ;
int dcx, xclip=0 ;
if(xf<0){ xclip=1 ;
          dcx=xi ;
} else if(xf > (int)(WIDTH-1)){
          xclip=1 ;
          dcx = (WIDTH-1)-xi ;
       }
int dx,dy,i,xinc,yinc,cumul,x,y ;
x = xi ;
y = yi ;
dx = xf - xi ;
dy = yf - yi ;
xinc = ( dx > 0 ) ? 1 : -1 ;
yinc = ( dy > 0 ) ? 1 : -1 ;
myabs(dx);
myabs(dy);
*p = *line++ ;
if ( dx > dy ) {
  cumul = dx>>1 ;
  for ( i = 1 ; i <= dx ; i++ ) {
    p += xinc ;  if(xclip) if(!dcx--) return ;
    cumul += dy ;
    if (cumul >= dx) {
      cumul -= dx ;
      p += yinc*WIDTH ;
    }
    *p = *line++ ;     } }
  else {
  cumul = dy>>1 ;
  for ( i = 1 ; i <= dy ; i++ ) {
    p += yinc*WIDTH ;
    cumul += dx ;
    if ( cumul >= dy ) {
         cumul -= dy ;
         p += xinc ; if(xclip) if(!dcx--) return ;
       }
    *p = *line++ ;
     } }
}

#ifdef includeMathTable

	#include "./tablesMath.h"
	// show a colored line from a point, an angle and a size, is clipped but start point must be in screen
	void aline(int x, int y, int angle, int size, clDeep *line)
	{  int xx, yy, za = 128-angle ; // 90°-angle -> (512/360)*90 - angle -> 128-angle
	   xx = x + ((size*COS(angle))>>16) ;
	   yy = y + ((size*SIN(angle))>>16) ;
	   if(yy > ((int)HEIGHT)-1){ xx += (TAN(za)*(yy-(HEIGHT-1)))>>16 ; yy=HEIGHT-1 ; }
		else if(yy < 0) { xx -= (TAN(za)*(-yy))>>16 ; yy=0 ; }
	   ligne(x,y,xx,yy,line) ;
	}

u32 getRotatePos(int x, int y, int angle, int size)
{  int xx, yy ; // 90°-angle -> (512/360)*90 - angle -> 128-angle
   xx = x + ((size*COS(angle))>>16) ;
   yy = y + ((size*SIN(angle))>>16) ;
   return (xx<<16)|(yy&0xffff);
}

void savelineMove(int x,int y,int xx,int yy,int*mx,int*my)
{	clDeep *p = pixel + x + y*WIDTH ;
	int dx,dy,i,xinc,yinc,cumul;
	dx = xx - x ;
	dy = yy - y ;
	xinc = ( dx > 0 ) ? 1 : -1 ;
	yinc = ( dy > 0 ) ? 1 : -1 ;
	myabs(dx) ;
	myabs(dy) ;

	printlr("line from %i.%i to %i.%i dx %i dy %i",x,y,xx,yy,dx,dy);
	
	if(dx>dy)
	{	cumul = dx>>1 ;
		for (i=1 ; i<=dx ; i++)
		{	  *mx++ = xinc ;
			  *my++ = 0;
				//printlr("x move_ %i",xinc);
			  cumul += dy ;
			  if (cumul >= dx)
				{	cumul -= dx ;
					*my++ = yinc*WIDTH ;
					*mx++ = 0;
					//printlr("y move_ %i",yinc);
				}
		}
	}
	  else
	{ cumul = dy>>1 ;
	  for (i=1 ; i<=dy ; i++)
	  {	*my++ = yinc;
	    *mx++ = 0;
		//printlr("y move %i",yinc);
		cumul += dx ;
		if(cumul >= dy)
		{ cumul -= dy ;
		  *mx++ = xinc ;
		  *my++ = 0;
		  //printlr("x move %i",xinc);
		}
	  }
	}

	  *mx++ = *my++ = 0xffff;
}



#endif


void logAsciiTable(void)
{ log("\n\n.. asciiTable ..\n\n");
  for(u32 x=0;x<256;x++)
	printlr("%i\t[%c]",x,x);
}

/*{ u32 lineSize = 16; u32 n=0, x=0;
  log("\n\n.. asciiTable ..\n\n");
  for(x=0;x<lineSize;x++)
  { if(x<10) printl("  %i",x);
     else    printl(" %i",x);
  }
  log("\n");
  while(n<256)
  { for(x=0;x<lineSize;x++) printl("  %c",(n+x)-127);
	log("\n");
	n+=lineSize;	
  };
}*/

#endif
