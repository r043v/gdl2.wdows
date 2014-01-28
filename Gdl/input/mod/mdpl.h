
/* mdpl, my own mod parser
	mdpl is © 2005/2006 by r043v/dph
  */

#ifndef _mdpl_
#define _mdpl_


struct note
{ short sample ;
  short	effect ;
  short	period ;
  const char *text ;
}; // 10 byte per note

struct ptn
{ int*	 dta ; // infile data
  note** raw ; // (note[rawPos])[chn] ;
  unsigned int loaded ;
};

struct mod
{ u8 *file, *name, *ptnData ;
  u8 *song, *songPos   ;
  struct spl *samples ;
  struct ptn *patterns;
  unsigned int splNb, chnNb, ptnNb, sngLenth, restart, maxPtn ;
  unsigned int crtPtn, crtRaw, speed, lastTick ;
  void ** memAlocated ;
};

struct spl
{ u8 *msg ;
  s8 *data ;
  unsigned int size, finetune, volume ;
  int rptStart, rptEnd, looped ; // rptStart -> -1 for no repeat
};

int finetune2C4(int finetune);

int getOctave(int freq);
int getNoteValue(int freq, int octave=0xff);
const char *getNoteTxt(int freq, int octave=0xff);

void showPtn(struct mod * mod, int pt=0);

void loadPtns(struct mod * mod);
void loadSpls(struct mod * mod);

void freeMod(struct mod * mod);

struct mod *loadMod(u8 *mod);

#endif
