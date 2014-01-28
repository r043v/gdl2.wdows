//
// TinyPTC by Gaffer
// www.gaffer.org/tinyptc
//

#include "../../../../config.h"

#ifdef usePtc
#include "tinyptc.h"

#ifdef keepOldSize // to keep windowed size
	int oldW, oldH;
#endif

char *wTitle ;
int   wHeight;
int   wWidth ;
int  *widthAdr;
int  *heightAdr;

HWND wnd;

unsigned int inFullScreen ;

u32 wmousex, wmousey ;	/* window mouse pos */
extern u32 mousex, mousey ;	/* buffer mouse pos */
u32 wsizex, wsizey ;	/* stretched window size */


/*
    extern int main(void);
#ifdef __PTC_WINMAIN_CRT__
    void WinMainCRTStartup(void)
#else
    void MainCRTStartup(void)
#endif
    {	main();
    }
	*/
/*
u32* srcptr;
u32* dstptr;
void _mmx_memcpy(void*dst,void*src,u32 size)
{    u32 s = size>>6;
     srcptr = (u32*)src;
     dstptr = (u32*)dst;
     while(--s)
     {  __asm("movq %mm0, _srcptr");
        __asm("movq %mm1, _srcptr+8");
        __asm("movq %mm2, _srcptr+16");
        __asm("movq %mm3, _srcptr+24");
        __asm("movq %mm4, _srcptr+32");
        __asm("movq %mm5, _srcptr+40");
        __asm("movq %mm6, _srcptr+48");
        __asm("movq %mm7, _srcptr+56");

        __asm("movq _dstptr,mm0");
        __asm("movq _dstptr+8,mm1");
        __asm("movq _dstptr+16,mm2");
        __asm("movq _dstptr+24,mm3");
        __asm("movq _dstptr+32,mm4");
        __asm("movq _dstptr+40,mm5");
        __asm("movq _dstptr+48,mm6");
        __asm("movq _dstptr+56,mm7");         
        
        srcptr += 16;
        dstptr += 16;       
     };
}
*/
#endif

