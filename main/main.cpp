
/* Gdl² - test */

#include "../Gdl/Gdl.h" // the lib..

  // include gfx
#include "./gfx/flashfont.h"
#include "./gfx/tileset.h"	// kobog tileset
//#include "./gfx/cursor.h"	// the yellow animated cursor
#include "./gfx/zeldaTiles.h"
#include "./gfx/zeldaTiles.map.h"
#include "./gfx/tile54.h"
#include "./gfx/tile10.h"
//#include "./gfx/przs.h"
//#include "./gfx/PRZYSTAN.map.h"

#include "./gfx/zzz.h"
#include "./gfx/sky.h"
#include "./gfx/sky.map.h"

#include "./array.h"		// the map array..
#include "./mymap.h"		// the map array..

#include "data.rar.h"

//#include "./kial_Tiles.h"

// uncrunch data...
void uncrunchData(void)
{	// uncrunch gfx from 4b gfm to 32b gfm
	unCrunchGfm(zzz,zzzFrmNb);
	unCrunchGfm(zeldaTiles,zeldaTilesFrmNb);
	unCrunchGfm(tile54,tile54FrmNb);
	unCrunchGfm(tile10,tile10FrmNb);
	unCrunchGfm(sky,skyFrmNb);
	//unCrunchGfm(tileset,tilesetFrmNb);
	unCrunchGfm(flashfont,flashfontFrmNb);
	//log("uncrunched");
}

void drawOutZone(outzone*out)
{	ligne(out->x,out->y,out->x+out->width,out->y,pixel);
	ligne(out->x,out->y,out->x,out->y+out->height,pixel);
	ligne(out->x+out->width,out->y,out->x+out->width,out->y+out->height,pixel);
	ligne(out->x,out->y+out->height,out->x+out->width,out->y+out->height,pixel);
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)

{	
	
	u32 size;
	u8 * as = loadFile("asm.s",&size);
	u32 asmOff=0, outOff=0;
	if(as)
	{	u8 * out = (u8*)malloc(102400);
		while(asmOff<size)
		{	while(asmOff<size && (as[asmOff]==' ' || as[asmOff]=='\n')) asmOff++;
			if(asmOff<size)
			{	const char * str = "\n\t\"\t";
				u32 s = strlen(str);
				memcpy(&out[outOff],str,s);
				outOff += s;
			}
			while(asmOff<size && as[asmOff]!='\n')
			{	static u32 cpy=1;
				if(as[asmOff] == '/' && as[asmOff+1] == '*') { asmOff+=2; cpy=0; } // comment start
				if(as[asmOff] == '*' && as[asmOff+1] == '/') { asmOff+=2; cpy=1; } // comment end
				if(cpy) out[outOff++] = as[asmOff];
				asmOff++;
			}
			asmOff++; outOff--;
			//memcpy(&out[outOff],,strlen("\t\\n\""));
			if(asmOff<size)
			{	const char * str = "\t\\n\"";
				u32 s = strlen(str);
				memcpy(&out[outOff],str,s);
				outOff += s;
			}
		};
		writeFile("out.c",out,outOff);
	}

	Gdl_init("[gdlMap]",640,480);   // init the framebuffer
	if(!Gdl_playSong("data.rar|song.xm")) Gdl_playSong(data,dataSize,"darkchip.xm");
	//setGdlfont(greenfont,greenfontFrmNb);
	uncrunchData();

	setGdlfont(flashfont);

	map myMap, myMap2, lapin, prz;

	outzone * out  = createOutzone(10,10,296,296);
	outzone * out2 = createOutzone(320,320,194,150);
	outzone * out3 = createOutzone(320,10,312,296);
	outzone * out4 = createOutzone(10,310,300,164);
	 
	lapin.set((u16*)skymap,sky,skyFrmNb,8,8,50,80,0,0,out,0);
	myMap2.set(maparray,zzz,zzzFrmNb,8,8,64,64,0,0,out2,0);
	myMap.set((u16*)zeldaTilesMap,zeldaTiles,zeldaTilesFrmNb,16,16,80,60,0,0,out3,0);
	prz.set((u16*)array,tileset,tilesetFrmNb,16,16,64,64,0,0,out4,0);

	anim * myanim  = setAnim(zzz,zzzFrmNb,100);
	//anim * myanim2 = setAnim(&tileset[4],8,100);
	anim * flowerAnm = setAnim(tile54,tile54FrmNb,200);
	anim * flowerAnm2= setAnim(tile10,tile10FrmNb,150);

	myMap2.setAnimatedTile(1,myanim);
	myMap.setAnimatedTile(54,flowerAnm);
	myMap.setAnimatedTile(10,flowerAnm2);

	u32 px = 320;
	int pxw = 1;

	u32 modSize = 0;
	u8 * sfx = loadFile("test.mod",&modSize);
	Gdl_setSfx(sfx,modSize);


	myMap.setScroll(150,150);


	while(1)
	{	clrScr(0);
		memset(pixel,0xff,WIDTH*4);

		static u32 hway=2, wway=8, hway2=2, wway2=8, hway3=2, wway3=8, hway4=2, wway4=8;
		u32 rtn = myMap.scroll(hway|wway,1);

		if(rtn)
		{	if(rtn&1) hway=2;
			if(rtn&2) hway=1;
			if(rtn&4) wway=8;
			if(rtn&8) wway=4;
		}

		rtn = myMap2.scroll(hway2|wway2,1);

		if(rtn)
		{	if(rtn&1) hway2=2;
			if(rtn&2) hway2=1;
			if(rtn&4) wway2=8;
			if(rtn&8) wway2=4;
		}

		rtn = lapin.scroll(hway3|wway3,1);

		if(rtn)
		{	if(rtn&1) hway3=2;
			if(rtn&2) hway3=1;
			if(rtn&4) wway3=8;
			if(rtn&8) wway3=4;
		}

		rtn = prz.scroll(hway4|wway4,2);

		if(rtn)
		{	if(rtn&1) hway4=2;
			if(rtn&2) hway4=1;
			if(rtn&4) wway4=8;
			if(rtn&8) wway4=4;
		}

		myMap2.moveOutZone(px,320);
		px += pxw;
		if(pxw<0 && px<=320) pxw =  1;
		if(pxw>0 && px>=440) pxw = -1;
		
		myMap.draw();
		myMap2.draw();
		lapin.draw();
		prz.draw();

		drawOutZone(out);
		drawOutZone(out2);
		drawOutZone(out3);
		drawOutZone(out4);

		if(keyPush(kleft))
		{	Gdl_playSfx(4,100);
		}

		u32 crttile = 0xffff;

		crttile = lapin.getScreenTile(mousex,mousey);
		if(crttile == 0xffff) crttile = myMap.getScreenTile(mousex,mousey);
		if(crttile == 0xffff) crttile = myMap2.getScreenTile(mousex,mousey);
		if(crttile == 0xffff) crttile = prz.getScreenTile(mousex,mousey);

		if(isMouseHere) if(crttile != 0xffff) prints(mousex+10,mousey+10,"%i",crttile);

		prints(10,20,"%i.%i",mousex,mousey);

		Gdl_flip();
	};

	Gdl_exit();
	return 0;
}

