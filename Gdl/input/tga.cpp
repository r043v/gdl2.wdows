
#ifndef _tga_
#define _tga_

#include "../Gdl.h"

u32 m_getw(char*p) { u8*pp = (u8*)p; return ( (*pp)+(pp[1]*256) ); }
int mLoadTGA(char *file, u8 *outpix, u32 *pal, u32 *clNb, u32 *w, u32 *h, int size)
{	char *fptr = file ;
	if(*(fptr + size - 7) != 'X') { /*printlr("! not a tga");*/	return 0 ; }	fptr += 18 ;
	u32 width	= m_getw(file+12);	if(w)	*w=width	;
	u32 height	= m_getw(file+14);	if(h)	*h=height	;
	u32 i = m_getw(file+5) ;	if(i>256) i=256 ; // cmlen
	if(clNb) *clNb = i;
	if(!outpix && !pal)	return 1 ;

	if(pal)
	{	if (*(file+1) != 1)	return 0 ;
		memset(pal, 0, 1024);	fptr += *file ;
		u32 i = m_getw(file+5) ;	if(i>256) i=256 ; // cmlen
		if(clNb) *clNb = i;
		register u32 r,g,b, color ;	u32 j=0 ;
		u8*p;
		if(i)	switch (*(file+7))	// cmdepth
        		{	case 15:
					case 16:
						//printlr("15 or 16b pal deep");
		                do{	color = m_getw(fptr); fptr+=2;
							//color = *fptr++ ; color += (*fptr++) * 256	;
							r = (color & 0x7C00) >> (10-3);
							g = (color & 0x03E0) >> (5-3);
							b = (color & 0x001F) << (3-0);
							pal[j] = (r<<16)|(g<<8)|b ;
						} while(++j<i) ;
		                break;
		            case 24:
						//printlr("24b pal deep");
						p = (u8*)fptr;
						do{	b = *p++;
							g = *p++;
							r = *p++;
							pal[j] = (r<<16)|(g<<8)|b ;
							printlr("color %i : %x %x %x : %x",j,r,g,b,pal[j]);
						} while(++j<i) ;
						fptr = (char*)p;
		                break;
		            case 32:
						//printlr("32b pal deep");
   		                p = (u8*)fptr;
						do{	b = *p++;
							g = *p++;
							r = *p++;	p++ ;
							pal[j] = (r<<16)|(g<<8)|b ;
						} while(++j<i) ;
						fptr = (char*)p;
		                break;
				};
    }

    if(outpix)
	{   if(*(file+2) != 1 && *(file+2) != 9) return 0 ; // imgtype
        if(*(file+16) != 8) return 0 ; // bpp

        fptr = file + 18 + *file ; // idsize
        if(*(file+1)) fptr += (*(file+7)+1)/8*m_getw(file+5); // cmapt cmdepth cmlen

        if(*(file+2) == 1) {     // Raw
        	int i = height ;
            while(i)	memcpy(outpix + (height - (i--) - 1)*width, fptr+=width, width) ;
        } else {                    // RLE
            long size;
            u8 *outp;

            for(u32 i = 0; i < height; i++) {
                size = width;
                outp = outpix + (height - i - 1)*width;
                while (size > 0) {
                    byte p;

                    p = *fptr++ ;
                    if (p & 0x80) {
                        byte v;
                        v = *fptr++ ;
                        p = p & 0x7F;
                        p += 1;
                        memset(outp, v, p);
                    } else {
                        p += 1;
                        memcpy(outp,fptr,p) ;	fptr+=p ;
                    }
                    outp += p;
                    size -= p;
                }
            }
        }
    }

    return 1 ;
}

#endif
