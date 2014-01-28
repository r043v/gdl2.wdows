
#ifndef _GdlMap_
#define _GdlMap_

#include "../../../Gdl.h"

#define arDeep unsigned short
#define arDeepDec 1

struct outzone
{	u32 height,width;
	u32 x,y;
	clDeep *start, *end;
};

class map
{ public:
	outzone * out;
	u32 scrollx, scrolly;
	u32 tileSizex, tileSizey;
	u32 xTileDec, yTileDec;
	u32 sizeInTilex, sizeInTiley;
	u32 sizeInPixelx, sizeInPixely;
	u32 firstTileBlitx, firstTileBlity;
	u32 uncutDrawx, uncutDrawy;
	u32 pixelMorex, pixelMorey;
	u32 currentDecx, currentDecy;
	u32 maxScrollx, maxScrolly;

	clDeep **tileset;
	anim ** Animate;
	u32 tileNumber;
	arDeep *array;
	
	u32 pixelessx, pixelessy;
	u32 morex, morey;
	u32 tiledrawx ,tiledrawy;

	u32  getMapTile(u32 x, u32 y);
	void setMapTile(u32 tile, u32 x, u32 y);
	u32  getOutZoneTile(u32 x, u32 y);
	void setOutZoneTile(u32 tile, u32 x, u32 y);
	u32  getScreenTile(u32 x, u32 y);
	void setScreenTile(u32 tile, u32 x, u32 y);

	u32  setScroll(u32 x, u32 y);
	u32  scroll(u32 way, u32 pawa);
	void setOutZone(outzone*out);
	void moveOutZone(u32 x, u32 y);

	void setAnimatedTile(u32 tile,anim * Anim);
	void draw(void);
	u32 set(arDeep*array,clDeep**tileset,u32 tileNumber,u32 tileSizex,u32 tileSizey,u32 sizex,u32 sizey,u32 scrollx,u32 scrolly, outzone*out, u32 copyArray);
};

outzone*createOutzone(u32 x, u32 y, u32 sx, u32 sy);
#endif
