
#ifndef _pcx_
#define _pcx_

#include "../Gdl.h"

u32 mpcxSize(u8*pcx,u32*x,u32*y)
{	u32 out = 0;
	u16*size = (u16*)out;
	u16*file = (u16*)pcx;
	*size++ = file[4];
	*size   = file[5];
	if(x) *x = file[4];
	if(y) *y = file[5];
	return out;
}

void*mpcxLoad(u8*pcx,u32 format)
{	u16*file = (u16*)pcx;
	u32 size = (file[4])*(file[5]);
	u32 palType = pcx[59];
	u8*pal = &pcx[16];
	u8*array8 = (u8*)malloc(size);
	u32*pal32  = (u32*)malloc(256*4);
	u8*bf = &pcx[128];

	u32 cpt=0;
    	do {	u32 color = *bf++;
    		if((color&0xC0) == 0xC0) // rle flag
        	{ u32 nb = color&0x3F;	 // number of repetition
            	  color = *bf++;
            	  while(nb--) array8[cpt++]=color;
        	} else array8[cpt++]=color;
    	} while(cpt < size);
	if(palType == 12) pal = bf; // 256 color
	u32 nbColor = 16+248*(palType == 12);

	for(u32 n=0;n<nbColor;n++) // extract palete
	{	u32 r = *bf++;
		u32 g = *bf++;
		u32 b = *bf++;
		pal32[n] = (r<<16)|(g<<8)|b ;
	};

	switch(format)
	{	case 8: {u8*out = (u8*)malloc(4+4+nbColor*4+size);
			 u32*p = (u32*)out;
			 p[0] = ((file[4])<<16)|(file[5]);
			 p[1] = nbColor;
			 memcpy(out+8,pal32,4*nbColor);			free(pal32);
			 memcpy(out+8+4*nbColor,array8,size);	free(array8);
			 return out;
			}
		break;

		case 32:{u32*out = (u32*)malloc(size*4);
			 for(n=0;n<size;n++)
			  out[n] = pal32[array8[n]];
			 free(array8); free(pal32);
			 return out;
			}
		break;
	};

	free(array8); free(pal32); return 0;
}

#endif
