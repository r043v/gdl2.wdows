
#include "map.h"
/*
map::map(void)
{	reset();
}

map::map(u16*array,clDeep**tileset,u32 tileNumber,u32 tileSizex,u32 tileSizey,u32 sizex,u32 sizey,u32 scrollx,u32 scrolly)
{	reset();
	this->set(array,tileset,tileNumber,tileSizex,tileSizey,sizex,sizey,scrollx,scrolly);
}

map::~map(void)
{	if(array) free(array);
	if(isAnimate) free(isAnimate);
	array=0;	isAnimate=0;
}

u32 doesResolutionChange(void)
{	static u32 oldHeight = 0 ;
	if((u32)HEIGHT == oldHeight)
		return 0 ;
	oldHeight = HEIGHT ;
		return 1 ;
}

#define recomputeIfScreenChange {if(doesResolutionChange())resolutionJustChange(WIDTH,HEIGHT);}

void map::reset(void)
{	this->array = 0;
	this->tileset = 0;
	this->isAnimate = 0;
	this->scrollx = 0;
	this->scrolly = 0;
	this->maxScrollx = 0;
	this->maxScrolly = 0;
	this->tileNumber = 0;
	this->tileSizex = 0;
	this->tileSizey = 0;
	this->sizex = 0;
	this->sizey = 0;
}

void map::setAnimatedTile(u32 tile,struct anim ** Anim)
{	if(!tile || !Anim || tile >= tileNumber) return ;
	//printlr("set tile %i with anim 0x%x",tile,Anim);
	if(!isAnimate)
	{	u32 size = tileNumber*4;
		isAnimate = (u32*)malloc(size);
		memset(isAnimate,0,size);
	}
		isAnimate[tile]=1;
		tileset[tile]=(clDeep*)Anim;
}

void map::setNewArray(u16*newArray)
{	memcpy(this->array,newArray,(this->sizex * this->sizey)<<1);
}
	
u16* map::getUsedArray(void)
{	return this->array;
}

u32 map::set(u16*array,clDeep**tileset,u32 tileNumber,u32 tileSizex,u32 tileSizey,u32 sizex,u32 sizey,u32 scrollx,u32 scrolly)
{	u32 size = sizex*sizey ;
	if(!array || !tileset || !size) return 0 ;

	// copy the array (so we'll can modify it)
	this->array	= (u16*)malloc(size<<1);
	memcpy(this->array,array,size<<1);

	this->tileset = tileset	;
	this->tileNumber = tileNumber ;
	this->tileSizex = tileSizex ;
	this->tileSizey = tileSizey ;
	this->scrollx = scrollx ;
	this->scrolly = scrolly ;
	this->sizex = sizex;
	this->sizey = sizey;

	// precompute some usefull value
	xTileDec		= computeDec(tileSizex); // tile size is multiple of 2, so can use >> or << instead of / or *
	yTileDec		= computeDec(tileSizey);

	resolutionJustChange(WIDTH,HEIGHT);

	xMapStartDrawPos = scrollx>>xTileDec ;	// first tile drawed in screen
	xMapStartDrawDec = (xMapStartDrawPos<<xTileDec)-scrollx;

	yMapStartDrawPos = scrolly>>yTileDec ;	// drawed pos of first tile
	yMapStartDrawDec = (yMapStartDrawPos<<yTileDec)-scrolly;

	return 1;
}

void map::scroll(u32 up,u32 down,u32 left,u32 right)
{	if(!tileset || !array) return ;
	if(down)	{	if(scrolly+down<maxScrolly) scrolly += down ;
					 else scrolly = maxScrolly ;
				}
	else if(up)
			{	if(scrolly > up) scrolly -= up ;
						else scrolly = 0 ;
			}

	if(right)	{	if(scrollx+right<maxScrollx) scrollx += right ;
					 else scrollx = maxScrollx ;
				}
	else if(left)
			{	if(scrollx > left) scrollx -= left ;
						else scrollx = 0 ;
			}
	
	xMapStartDrawPos = scrollx>>xTileDec ;	// first tile drawed in screen
	xMapStartDrawDec = (xMapStartDrawPos<<xTileDec)-scrollx;

	yMapStartDrawPos = scrolly>>yTileDec ;	// drawed pos of first tile
	yMapStartDrawDec = (yMapStartDrawPos<<yTileDec)-scrolly;
}

void map::drawWithTile(u32 tile, u32 scrollx, u32 scrolly)
{	recomputeIfScreenChange
	u32 pos, pos2 ;
	u32 x,y;
	if(tile >= tileNumber) return;
	clDeep*tl = tileset[tile] ;

		// draw all border with drawGfm
	pos = xMapStartDrawDec ;
	for(x=0;x<nbTileDrawedx;x++)
	{	drawGfm(tl,pos,yMapStartDrawDec);
		pos += tileSizex;
	};
	
	pos = yMapStartDrawDec + tileSizey ;
	for(y=1;y<nbTileDrawedy-1;y++)
	{	drawGfm(tl,xMapStartDrawDec,pos);
		pos += tileSizey;
	};

	 pos = xMapStartDrawDec+((nbTileDrawedx-1)<<xTileDec) ;
	pos2 = yMapStartDrawDec+tileSizey;
	for(y=1;y<nbTileDrawedy-1;y++)
	{	drawGfm(tl,pos,pos2);
		pos2 += tileSizey ;
	};

	 pos = yMapStartDrawDec+((nbTileDrawedy-1)<<yTileDec) ;
	pos2 = xMapStartDrawDec;
	for(x=0;x<nbTileDrawedx;x++)
	{	drawGfm(tl,pos2,pos);
		pos2 += tileSizex;
	};
		// and fill center with udrawGfm
	clDeep* screen = xy2scr((xMapStartDrawDec+tileSizex),(yMapStartDrawDec+tileSizey)) ;
		for(y=1;y<nbTileDrawedy-1;y++)
		{	clDeep*s = screen ; screen += (tileSizey*WIDTH);
			for(x=1;x<nbTileDrawedx-1;x++)
			{	udrawGfm(tl,s);
				s += tileSizex ;
			}
		}

	if(this->scrollx >= tileSizex) this->scrollx=0;
	if(this->scrolly >= tileSizey) this->scrolly=0;
	scroll(0,scrolly,0,scrollx);
}

void map::setScroll(u32 x, u32 y)
{	scrollx = x ;	if(scrollx>maxScrollx) scrollx = maxScrollx;
	scrolly = y ;	if(scrolly>maxScrolly) scrolly = maxScrolly;
	scroll(0,0,0,0);
}

u32 map::getMaxy(void)
{	return maxScrolly ;
}

void map::draw(void)
{	if(!isAnimate) { drawUnAnimated(); return ; }
	recomputeIfScreenChange
	u32 pos, pos2 ;	u16*array = this->array, *array2;
	u32 tile,x,y ;
     // draw all screen border with drawGfm

		// up & down line
	array += (yMapStartDrawPos*sizex+xMapStartDrawPos) ;
	array2 = array + (nbTileDrawedy-1)*sizex ;
	pos = xMapStartDrawDec ;
	pos2 = yMapStartDrawDec+((nbTileDrawedy-1)<<yTileDec);
	for(x=0;x<nbTileDrawedx;x++)
	{	tile = *array++ ;
		if(tile && tile < tileNumber)
			{	if(!(isAnimate[tile]))	drawGfm(tileset[tile],pos,yMapStartDrawDec);
					else	playAnim((struct anim**)tileset[tile],pos,yMapStartDrawDec);
			}
		tile = *array2++ ;
		if(tile && tile < tileNumber)
			{	if(!(isAnimate[tile]))	drawGfm(tileset[tile],pos,pos2);
					else	playAnim((struct anim**)tileset[tile],pos,pos2);
			}
		pos += tileSizex;
	};

		// left & right colon
	array  = (this->array) + xMapStartDrawPos + (yMapStartDrawPos+1)*sizex ;
	array2 = array + (nbTileDrawedy-1);
	pos = yMapStartDrawDec+tileSizey; // for left
	pos2 = xMapStartDrawDec+((nbTileDrawedx-1)<<xTileDec); // and right
	for(y=1;y<nbTileDrawedy-1;y++)
	{	tile = *array ;
		if(tile && tile < tileNumber)
			{	if(!(isAnimate[tile]))	drawGfm(tileset[tile],xMapStartDrawDec,pos);
					else	playAnim((struct anim**)(tileset[tile]),xMapStartDrawDec,pos);
			}
		tile = *array2 ;
		if(tile && tile < tileNumber)
			{	if(!(isAnimate[tile]))	drawGfm(tileset[tile],pos2,pos);
					else	playAnim((struct anim**)(tileset[tile]),pos2,pos);
			}
		pos  += tileSizey;
		array += sizex ;
		array2 += sizex;
	};

 // and fill center with udrawGfm
	array  = (this->array) + xMapStartDrawPos + 1 + (yMapStartDrawPos+1)*sizex ;
	array2 = array;
	clDeep* screen = xy2scr((xMapStartDrawDec+tileSizex),(yMapStartDrawDec+tileSizey)) ;
		for(y=1;y<nbTileDrawedy-1;y++)
		{	clDeep*s = screen ; screen += (tileSizey*WIDTH);
			for(x=1;x<nbTileDrawedx-1;x++)
			{	tile = *array++ ;
				if(tile && tile < tileNumber)
				{	if(!(isAnimate[tile]))	udrawGfm(tileset[tile],s);
						else	playAnim((struct anim**)(tileset[tile]),s);
				}	s += tileSizex ;
			};	array2 += sizex; array = array2;
		}
}

void map::drawUnAnimated(void)
{	u32 pos, pos2, x, y;	u16*array = this->array, *array2;
	recomputeIfScreenChange
 // draw all screen border with drawGfm

		// up & down line
	array += (yMapStartDrawPos*sizex+xMapStartDrawPos) ;
	array2 = array + (nbTileDrawedy-1)*sizex ;
	pos  = xMapStartDrawDec ;
	pos2 = yMapStartDrawDec+((nbTileDrawedy-1)<<yTileDec);
	for(x=0;x<nbTileDrawedx;x++)
	{	drawGfm(tileset[*array++],pos,yMapStartDrawDec);
		drawGfm(tileset[*array2++],pos,pos2);
		pos += tileSizex;
	};

		// left & right colon
	array  = (this->array) + xMapStartDrawPos + (yMapStartDrawPos+1)*sizex ;
	array2 = array + (nbTileDrawedy-1);
	pos = yMapStartDrawDec+tileSizey; // for left
	pos2 = xMapStartDrawDec+((nbTileDrawedx-1)<<xTileDec); // and right
	for(y=1;y<nbTileDrawedy-1;y++)
	{	drawGfm(tileset[*array],xMapStartDrawDec,pos);
		drawGfm(tileset[*array2],pos2,pos);
		pos  += tileSizey;
		array += sizex ;
		array2 += sizex;
	};

 // and fill center with udrawGfm
	array  = (this->array) + xMapStartDrawPos + 1 + (yMapStartDrawPos+1)*sizex ;
	array2 = array;
	clDeep* screen = xy2scr((xMapStartDrawDec+tileSizex),(yMapStartDrawDec+tileSizey)) ;
		for(y=1;y<nbTileDrawedy-1;y++)
		{	clDeep*s = screen ; screen += (tileSizey*WIDTH);
			for(x=1;x<nbTileDrawedx-1;x++)
			{	udrawGfm(tileset[*array++],s);
				s += tileSizex ;
			};	array2 += sizex; array = array2;
		}
	
}

u32 map::getScrollx(void)
{	return scrollx ;
}

u32 map::getScrolly(void)
{	return scrolly ;
}

void map::getScroll(u32*x,u32*y)
{	if(x)*x=scrollx;
	if(y)*y=scrolly;
}

u32 map::getMapTile(u32 x, u32 y)
{	x >>= this->xTileDec ;
	y >>= this->yTileDec ;
	return array[y*sizex+x];
}

void map::setMapTile(u32 tile, u32 x, u32 y)
{	if(tile >= this->tileNumber) return ;
	x >>= this->xTileDec ;
	y >>= this->yTileDec ;
	array[y*sizex+x] = tile ;
}

u32 map::getScreenTile(u32 x, u32 y)
{	return getMapTile(x+scrollx,y+scrolly);
}

void map::setScreenTile(u32 tile, u32 x, u32 y)
{	if(tile >= this->tileNumber) return ;
	setMapTile(tile,x+scrollx,y+scrolly);
}

void map::resolutionJustChange(u32 w,u32 h)
{	nbTileDrawedx	=  (w+tileSizex)>>xTileDec; // tile drawed by screen
	if(nbTileDrawedx<<xTileDec != w+tileSizex) nbTileDrawedx++;

	nbTileDrawedy	=  (h+tileSizey)>>yTileDec;
	if(nbTileDrawedy<<yTileDec != h+tileSizey) nbTileDrawedy++;

	maxScrollx		= ((sizex<<xTileDec)-w)-1; // max scroll value
	maxScrolly		= ((sizey<<yTileDec)-h)-1;
}
 */