//
// TinyPTC by Gaffer
// www.gaffer.org/tinyptc
//

/******************

	take care this is an unofficial release !

  this release support callback, switch in fullscreen
	and some other thing..
		cannot be used in any commercial project!
			i will not be responsable for some bugs ect...
				use it at your own risk.
			(c) 2k5/2k6 by r043v
		for more info noferov@gmail.com

		*******************/

#ifdef usePtc

#ifndef __TINYPTC_WINDOWS_H
#define __TINYPTC_WINDOWS_H

#ifdef __cplusplus
extern "C" {
#endif

#define int32 unsigned int   
#define short16 unsigned short 
#define char8 unsigned char  
#define u32 unsigned int

// tinyptc api

int ptc_open(char *title,int *w,int *h);
int ptc_update(void *buffer);
void ptc_close(void);//int exitProcess);
void ptc_getMsg();
void getWdsSize(int*w,int*h);
void setInfo(char*title,int *h,int *w);
void showMsg(const char*msg,const char*title);
void ptc_iniMsgCallBack(u32 (*onMsg)(u32 msg, u32 arg1, u32 arg2));
void ptc_setAfterZoomCallBack(void (*onAfterZoom)(void*bf,u32 w,u32 h,u32 pitch));

// display target
#define __PTC_DDRAW__ 

// configuration

#define __PTC_CENTER_WINDOW__
#define __PTC_RESIZE_WINDOW__
#define __PTC_SYSTEM_MENU__
#define __PTC_ICON__ "IDI_MAIN"
#define __PTC_ALLOW_CLOSE__
#define __PTC_CLOSE_ON_ESCAPE__
#define __PTC_DISABLE_SCREENSAVER__

/* ------- add by ferov ---- */

//#define setx2zoom					/* enable it to directly get an x2 zoom at start */
#define allow2xZoom					/* enable it to turn on the ability to zoom at 2x */
//#define allow4xZoom				/* the same but for 4x (allow before for 2x) */
//#define startInFullScreen			/* does we start in full screen mode or windowed ? */
#define canAutoResizeInFullScreen	/* can the output be resized in full screen mode (if not 4/3) ? if not, stay in windowed */
#define keepOldSize					/* will keep windowed size while switching */
#define minimumFullScreenHeight	480	/* please use legal value, 0 to disable minimum size */
#define minimumFullScreenWidth	640

/* ------------------------- */

// converter configuration

#define __PTC_CONVERTER_32_TO_32_RGB888
#define __PTC_CONVERTER_32_TO_32_BGR888
#define __PTC_CONVERTER_32_TO_24_RGB888
#define __PTC_CONVERTER_32_TO_24_BGR888
#define __PTC_CONVERTER_32_TO_16_RGB565
#define __PTC_CONVERTER_32_TO_16_BGR565
#define __PTC_CONVERTER_32_TO_16_RGB555
#define __PTC_CONVERTER_32_TO_16_BGR555

// endian configuration
#define __PTC_LITTLE_ENDIAN__

// optimization
//#define __PTC_MMX__
#define __PTC_MEMCPY__

#define __PTC_WINMAIN_CRT__

extern u32 wmousex, wmousey ;	/* window mouse pos */
extern u32 mousex,  mousey ;	/* buffer mouse pos */
extern u32 wsizex,  wsizey ;	/* stretched window size */

#include "ddraw.h"
extern HWND wnd;

extern unsigned int inFullScreen ;

void setAppPower(unsigned int value);

#ifdef keepOldSize // to keep windowed size
	extern int oldW, oldH;
#endif

extern char *wTitle ;
extern int   wHeight;
extern int   wWidth ;
extern int  *widthAdr;
extern int  *heightAdr;

#ifdef __cplusplus
}
#endif

#endif

#endif
