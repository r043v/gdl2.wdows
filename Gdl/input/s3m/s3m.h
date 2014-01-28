
#ifndef _s3m_
#define _s3m_

#include "../../Gdl.h"

extern const char *adlibType[] ;
extern const char      *s3note[] ;

// power of pointer !

struct sample
{  u8  *offset, *data ;
   u8  *flag,   *volume, *packed, *name, *fname ;
   u8  *looped, *stereo, *in16bits ;
   u16 *hrz16 ;
   u32 *size, *check, *loopEnd, *loopStart, adData ; // adData for looped, stereo and in16bits
};

struct s3m
{ u8  *file ;
  u16 *orderNb, *sampleNb, *ptnNb, *flag, *version ;
  u8  *glbVol, *mstVol, *iSpeed, *iTempo, *stereo, *fchSetting, *chnNb, *order, **ptn ;
  struct chn ** chn ;
  struct sample ** sample ;
  u8  *songName, *tracker, *comment ;
  u8  *chnSwitch ;
  s8  *chnSetting ;
  u32 adData ; // adData for mstVol & stereo,  to keep pointer way
  void**freeList;
};

struct sample * loadSample(u8*sample, u8*s3m, u32 fileSize=0);
struct s3m * loadS3m(u8 *song,int fileSize=0);
void freeS3m(struct s3m*s3m);

void logPtn0Chn1(struct s3m*s3m);

const int	 periods[]  = { 1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907 };

#endif
