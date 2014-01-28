
/* mdpl, my own mod parser
	mdpl is © 2005/2006 by r043v/dph
  */

#include "../../Gdl.h"
#include "./mdpl.h"

const char *noteName[14] = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B","???","   " } ;

// max freq for octave 0->5    C   C#    D   D#    E    F   F#    G    G#   A   A#  and < -> B
int maxFreqPerNote[2+5*11] = { 1616,1525,1440,1357,1281,1209,1141,1077,1017, 961, 907,
                                808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453,
                                404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226,
                                202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113,
                                101,  95,  90,  85,  80,  76,  71,  67,  64,  60,  57,
                                 48,   0
                             };

int c4freq[16] = { 8363,8413,8463,8529,8581,8651,8723,8757,7895,7941,7985,8046,8107,8169,8232,8280 };

int finetune2C4(int finetune)
{	if(finetune<0 || finetune>15) finetune=0;
	return c4freq[finetune];
}

char noteText[6*11*4];

void showPtn(struct mod * mod, int pt)
{ ptn  *p = &((mod->patterns)[pt]) ;  note *n ;
  if(p->loaded != 64) return ;
  printl("\n\n") ;
  for(int c=0;c<64;)
   { n = p->raw[c++] ;
     if(c<10) printl(" ") ;
     printl("%i - ",c) ;
     for(unsigned int r=0;r<mod->chnNb;r++)
     { if(n[r].text) printl(n[r].text) ;
       else          printl("   ") ;
       if(n[r].sample){ if(n[r].sample < 0x10) printl(" ") ;
                        printl("%x ",n[r].sample) ;
       } else printl("   ") ;
       //if(n[r].effect) printl("%3x",n[r].effect) ;
       //else            printl("   ") ;
	   printl("%i",n[r].period) ;
       printl(" | ",n[r].sample) ;
     };
     printl("\n") ;
   };
}

void loadPtns(struct mod * mod)
{ int max = mod->maxPtn, *ptr ;
	unsigned int c=0 ,m=0 ,cpt=0 ;
  ptn *p ; note *n, *nt ; s8 *splDta ;
  u8 *ptr8 ;
  //if(!mod->sngLenth){ printl("\nthis mod have song lenth of 0 :|\n") ; }

  //printl("\nsong lenth : %i",mod->sngLenth) ;
  for(;c<mod->sngLenth;c++)
     if(mod->song[c] > m && mod->song[c] < max) m = mod->song[c] ;

  mod->ptnNb = ++m ; //printl("\n%i patterns",m) ;
  mod->patterns = (struct ptn*)malloc(m*sizeof(struct ptn)) ;		addFreeEntry(mod->patterns,mod->memAlocated) ;
  n = (note*)malloc(mod->chnNb*sizeof(struct note)*64*mod->ptnNb) ; addFreeEntry(n,mod->memAlocated) ;
  for(c=0;c<m;c++)
  { p = &((mod->patterns)[c]) ;
    p->loaded = 0 ;
    p->dta = (int*)(mod->ptnData + (c*1024)) ;
    p->raw = (note**)malloc(64*4) ;
    ptr = (int*)(p->dta) ;
    //printl("\nload ptn %2i",c) ;
    nt = &(n[64*mod->chnNb*c]) ; // nt = n + 4*(p->loaded) + 256*c ; p->raw[(p->loaded)++] = nt ;
    while(p->loaded < 64)
    { p->raw[(p->loaded)++] = nt ;
      //printf("\n") ;
      for(cpt=0;cpt<mod->chnNb;cpt++)
        { if(*ptr)
          {	ptr8 = (u8*)ptr ;
			nt->sample	=	((ptr8[2])>>4) + (ptr8[0] & 0xF0)	;
			nt->period	=	((ptr8[0] & 0x0F)*256) + ptr8[1]	;
			nt->effect	=	((ptr8[2] & 0x0F)*256) + ptr8[3]	;
			nt->text	=	getNoteTxt(nt->period,0xff)         ;
		  } else { memset(nt,0,sizeof(struct note)); }
           nt++ ; ptr++ ;
    	};
    };
    //showPtn(mod,c);
  };  //mod->samples[0].data = (s8*)ptr ;
  c = cpt = 0 ; splDta = (s8*)ptr ;                 // after pattern data, there are sample data
  while(cpt<mod->splNb){ //mod->samples[c].data = 0 ;
                         if(mod->samples[c].size) { mod->samples[c].data = splDta ;
                                                    splDta += mod->samples[c].size;
                                                    cpt++ ;
                                                    //printl("\nsample %2x - %x %i - off %i",c,mod->samples[c].data,mod->samples[c].size,(int)(mod->samples[c].data)-(int)(mod->file));
                                                  }
                         c++ ;
                       }
}

void loadSpls(struct mod * mod)
{ u8*p = (mod->file)+20; unsigned int max = 31 ;
  struct spl *spl, *all ;
  u32    splSz[31], *sz = splSz, c=0 ;
  mod->splNb = 0 ;  mod->maxPtn=64 ;
  // get channels and samples number
  int chk = *(int*)((mod->file)+1080) ;
  mod->ptnData = mod->file + 1084 ; mod->chnNb=4 ;
  if((chk & 0x004B004D) == 0x004B004D) // M*K*  -> 31 samples, 4 channels
  { if(chk == 0x214B214D) mod->maxPtn=256 ; // M!K!, more than 64 patterns
  } else { if((chk & 0x00544C46) == 0x00544C46) // FLT*
           { mod->chnNb = ((chk&0xFF000000)>>24) - '0' ;
           } else { if((chk & 0x4E484300) == 0x4E484300) // *CHN
                    { mod->chnNb = (chk & 0x000000FF) - '0' ;
                    } else { if((chk & 0x48430000) == 0x48430000) // **CH
                             { mod->chnNb = (((chk & 0x000000FF)) - '0')*10 ;
                                mod->chnNb+= ((chk & 0x0000FF00)>>8) - '0' ;
                               // else is pattern data start, 4 channel, 15 samples
                             } else { max=15 ; mod->ptnData -= 4 ;
                                    }
                           }
                  }
         }

  all = (struct spl*)malloc(sizeof(struct spl)*max) ; addFreeEntry(all,mod->memAlocated) ;
  memset(all,0,sizeof(struct spl)*max) ;
  mod->samples = all ;

  char *z = (char*)&chk ;

  //printl("\"%c%c%c%c\"",*z,*(z+1),*(z+2),*(z+3)) ;
  //printl("\n%i channels, %i samples\n",mod->chnNb,max) ;

  while(c < max)
  { spl = &(all[c]) ;
	//printl("\n%22s",p);// spl=0 ; // show sample name or msg
	spl->msg = p;
    p+=22 ; *sz = ((*p)*256 + p[1])<<1 ;
    if(*sz){ 
             //printl(" :: %i ::",*sz) ;
			 spl->size = *sz ; p+=2 ;
             spl->finetune = (*p++)&0xf ;
             spl->volume = *p++ ;
             spl->rptStart = ((*p)*256 + p[1])<<1 ; p+=2;//((*p++)*256 + *p)<<1 ; //(*(u16*)p)<<1 ;
			 spl->rptEnd   = ((*p)*256 + p[1])<<1 ; p+=2;//((*p++)*256 + *p)<<1 ; //(*(u16*)p)<<1 ;
			 spl->looped = 0;
			 if(spl->rptEnd > 2)
				 spl->rptEnd += spl->rptStart ;
			 else spl->rptStart = -1 ; // no looped

             mod->splNb++ ;

//			 printl(" *") ;
/*
			 if((u32)spl->rptStart < *sz) { p+=2 ; spl->rptEnd = (*(u16*)p)<<1 ; p+=2 ;
                                       
										    if((u32)spl->rptStart + (u32)spl->rptEnd > *sz)
												spl->rptStart = -1 ;
											else printl("*") ;
                                     }
             else { spl->rptStart = -1 ; p+=4 ; }
*/		

			 spl->looped = spl->rptStart>=0;

			/*if(spl->looped)
				printl("[%i.%i]",spl->rptStart,spl->rptEnd);*/
    } else p+=8 ;
    sz++ ;
    c++  ;
  };
}

struct mod *loadMod(u8 *mod)
{ struct mod * md = (struct mod*)malloc(sizeof(struct mod)) ;
  memset(md,0,sizeof(struct mod)) ;
  md->memAlocated = createFreeList(31);
  md->file = mod ; md->name = mod ;  int max = 0 ;
  for(int cpt=0;cpt<20;cpt++) if(mod[cpt]) max=cpt ;

  if(max >= 20 || (mod[950]) > 128 || (mod[950]) == 0)
  {	  //printl("\nnot a mod !");
	freeMod(md);
	free(md);
		return 0;

  }

  cpt=max ;
  
  //printl(" %i\n",max) ;
  
  while(cpt<24 && !mod[++cpt]);// printl("-") ;
  if(cpt!=20)
  {	  //printl("\nnot a mod ! %i\n",cpt);
	freeMod(md);
	free(md);
		return 0;
  }

  md->sngLenth = mod[950] ; md->song = mod + 952 ;
  //printl("\nflag: %i\n",(int)mod[951]) ;
  loadSpls(md); // load samples info
  loadPtns(md);
  //showPtn(md) ;
  md->crtPtn = md->crtRaw = md->lastTick = 0 ;
  md->speed = 6 ;

  //printl("load finish.");

  return md ;
}

void freeMod(struct mod * mod)
{	if(!mod) return ;
	freeAllEntry(mod->memAlocated);
	free(mod);
}

/* code to get octave/note or text from a mod frequence */

int getOctave(int freq)
{       if(freq < 57)  return 5;
   else if(freq < 113) return 4;
   else if(freq < 226) return 3;
   else if(freq < 453) return 2;
   else if(freq < 907) return 1;
   return 0;
}

int getNoteValue(int freq, int octave)
{ if(!freq) return 13 ;
  if(octave>5) octave = getOctave(freq);
  int * p = &(maxFreqPerNote[octave*11]);
  for(int c=0;c<12;c++)
    if(freq > p[c]) return c ;
  return 12 ; // 12, unknow
}

const char *getNoteTxt(int freq, int octave)
{ static int first=0 ; if(!first++)memset(noteText,0,6*11*4);
  if(!freq) return noteName[13];
  if(octave>5) octave = getOctave(freq);
  int v = getNoteValue(freq,octave) ;
  char *txt = &(noteText[octave*11*4 + v*4]);
  if(!*txt)
  { const char *s=noteName[v], *o="%s%i" ;
    if(!s[1]) o="%s-%i"; sprintf(txt,o,s,octave) ;
  } return txt ;
}

