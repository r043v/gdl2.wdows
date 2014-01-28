// transparent blit routine for 16 & 32b buffer | Gdl²
// (C) 2k4/2k6 by r043v, under GPL

	/* screen buffer  */
		extern clDeep *pixel ;

/* routines */

	/*** data2Gfm */
		clDeep * data2Gfm(unsigned char *data);
	/*
		will convert an agf (a 4b gfm) to a 16 or 32b gfm	*/

		void unCrunchGfm(clDeep ** gfm, u32 frmNb);
		// uncrunch some 4b gfm

	/*** drawGfm */
		void drawGfm(clDeep *Gfm, int x, int y);
	/*
		draw an entirely clipped picture at specified screen coordonate
		 x and y are position in pixel from up left of the screen,
			coordonate also indicate up left of the picture blitted
		 picture must be a blitable gfm (16b or 32b)				*/


	/*** udrawGfm */
		void udrawGfm(clDeep *Gfm,clDeep *scr);
	/*
		draw a frame at specified screen adress,
		 to compute adress from classic coordonate use xy2scr macro
		 coordonate is pixel position from up left of screen, it's also up left sprite position
		 take care, this routine is not clipped for width !
		 picture must be a 16b or 32b gfm
		 note : drawGfm will use udrawGfm if the picture don't require any width clipping	*/

			// 2x zoomed udrawGfm
		void zdrawGfm(clDeep *Gfm,clDeep *scr);


			// directly draw a 4b Gfm use it if you need to work on picture palette (16 color)
		void udraw4bGfm(u8*Gfm,clDeep*scr,clDeep*mypal=0);

	/*** Gfm2array */
		clDeep* Gfm2array(clDeep *Gfm);
	/*
		convert a gfm into an raw array, 2 args
		. first is pointer to the gfm, a clDeep*
		. second is transparent color, a clDeep, default is pink */
	/*


		pixel/pixel colide routine
									*/
	void	logCldArray(u8 *Ar);
	u8*		Gfm2cldArray(clDeep *Gfm);
	int		colide(u8*s1,int x1,int y1,u8*s2,int x2,int y2);

// blit zone definition routines ..

// delimate current blit zone
void   setBlitLimit(u32 x, u32 y, u32 x1, u32 x2);
void setBlitLimit(clDeep *start, clDeep *end, u32 widthStart, u32 widthEnd);

// save or load a blit zone
void  saveBlitLimit(void); // save current blit limit
void  loadBlitLimit(void); // retreve saved blit limit

void internalLoadBlitLimit(void);
void internalSaveBlitLimit(void);

// maximize blit zone
void fullBlitLimit(void); // define blit limit as full screen

//void rdrawGfm(clDeep *Gfm, int x, int y, int angle);

/* macros */

u16 pc2gpColor(u32 color);
	
	/*** xy2scr .. will convert screen coordonate to screen address */
	#define xy2scr(x,y)		(&pixel[(x)+(y)*WIDTH])

    #ifdef use32b
    	/*** getGfmSize .. will retreve gfm size x and/or y */
    	#define getGfmSize(x,y,gfm)	if(x)*(x)=(((gfm)[2])>>16);if(y)*(y)=(((gfm)[2])&0xffff)
    #else
        #define getGfmSize(x,y,gfm)	if(x)*(x)=*Gfm;if(y)*(y)=gfm[1]
    #endif

