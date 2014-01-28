
#include "s3m.h"

//const char *adlibType[] = { "","","amel","abd","asnare","atom","acym","ahihat" } ;
//const char    *s3note[] = { "C-","C#","D-","D#","E-","F-","F#","G-","G#","A-","A#","B-" } ;

      u8 emptyraw[32*5] = { 255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                            255,0,255,255,0,255,0,255,255,0,
                          } ;

struct sample * loadSample(u8*sample, u8*s3m, u32 fileSize)
{ u8 *s = sample ; //log("enter load spl");
//	if( ((int)s)%4 ) log("sample not aligned.");
  u32 *is = (u32*)(s+16) ;
  if(*s > 1){ /*log("*s > 1 : bad sample");*/ return 0 ; }
  struct sample *spl = (struct sample*)malloc(sizeof(struct sample)) ;
         memset(spl,0,sizeof(struct sample)) ;// addFreeEntry(spl,s3m.freeList) ;
		 if(*s > 1) { /*log("*** bad sample");*/ free(spl) ; return 0 ; }
  spl->offset    = s   ;
  spl->fname     = s+1 ;
  spl->name      = s+16*3  ;
  spl->volume    = s+16+12 ;
  spl->packed    = s+16+14 ;
  spl->flag      = s+16+15 ;
  spl->check     = (u32*)(s+16*3+28) ;
  if(*s != 1 || *(spl->check) != 0x53524353) { /*log("*bad sample check");*/ *(spl->flag) = 142 ; return spl ; } // empty/bad or unsuported sample
  spl->size      = is++ ;
  spl->loopStart = is++ ;
  spl->loopEnd   = is   ;
   { u8 flag = *(spl->flag) ;
     u8   *d = (u8*)&(spl->adData)  ;
     *d   = flag&1 ; // looped ?
     d[1] = flag&2 ; // stereo ?
     d[2] = flag&4 ; // 16 bit ?
     spl->looped   = d   ;
     spl->stereo   = ++d ;
     spl->in16bits = ++d ;
   }
   spl->hrz16 = (u16*)(s+32) ;

  { //u32 z = *(s+14) + ((*(s+13))<<8) + ((*(s+15))<<16) ;
	  u32 z = *(s+13) + ((*(s+14))<<8) + ((*(s+15))<<16) ;
	 
    z >>= 4 ;
    if(z > fileSize)
	{	//log("sample error, too big !!!") ;
		*(spl->flag) = 142 ;
	}
	else	spl->data = s3m + z ;
  }// log(" exit load spl");
  return spl ;
}

u8*emptyPattern = (u8*)0xffffffff ;

struct s3m * loadS3m(u8 *song, int fileSize)
{  //printlr("enter load s3m, song is %i ko size and is at 0x%X",fileSize>>10,song);
	
	struct s3m s3m, *out ; u8 *f = song ; int i ;
   s3m.file = s3m.songName = f ;
   f+=28 ; if(*f++ != 0x1a) return 0 ; // not an s3m
           if(*f != 16 && *f != 17) return 0 ;

		   s3m.freeList = createFreeList();

   f+=3 ;  s3m.orderNb  = (u16*)f ;
   f+=2 ;  s3m.sampleNb = (u16*)f ;
   f+=2 ;  s3m.ptnNb    = (u16*)f ;
   f+=2 ;  s3m.flag     = (u16*)f ;
   f+=2 ;  s3m.tracker  = (u8*)allocAndAddFree(42,s3m.freeList) ;
           { int tr4k3r = *(u16*)f;
             if(tr4k3r>>12 == 0x1) sprintf((char*)s3m.tracker,"ScreamTracker") ;
             else           sprintf((char*)s3m.tracker,"unknow (%x)",tr4k3r>>12);
             tr4k3r&=0xfff ;
             sprintf((char*)s3m.tracker,"%s v%x.%x",s3m.tracker,tr4k3r>>8,tr4k3r&0xff);
			 //printlr("song '%s' created with %s",s3m.songName,s3m.tracker);
           }
   f+=2 ;  s3m.version  = (u16*)f ;
   f+=2 ;  if(*(int*)f != 0x4d524353){ freeAllEntry(s3m.freeList); return 0; } // not an s3m
   f+=4 ;  s3m.glbVol   = f++ ;
           s3m.iSpeed   = f++ ;
           s3m.iTempo   = f++ ;
           { u8* p = (u8*)(&(s3m.adData)) ;
             p[0] = (u8)((*f)&0x7f) ;
             p[1] = (u8)((*f)>>7) ;
           }
		  /* 
			printlr("it's really an s3m .. fileVersion %i",*s3m.version);
			printlr("glb volume %i .. speed %i .. tempo %i",*s3m.glbVol,*s3m.iSpeed,*s3m.iTempo);
*/
		  // log();
		  // log("it's an s3m");

   f+=11;  //if(((int)f)%2) log("s3m.comment not 2 multiple");
			/*if(*(u16*)f) s3m.comment = s3m.file + (*(u16*)f)*16 ;  // <- fix here !
			else*/         s3m.comment = 0 ;
   f+=2 ;  s3m.fchSetting = f ;    // read channels
           s3m.chnSetting = (s8*)allocAndAddFree(32*4,s3m.freeList) ;
          //printf("\n 255>>7 %i, 128>>7 %i",255>>7,128>>7);
          s3m.chnSwitch = (u8*)allocAndAddFree(64,s3m.freeList) ;
          
		  // channels settings
		  {  u8*p = (u8*)(&(s3m.adData)) ; p+=2 ; s3m.chnNb = p ; *p=0 ;
             for(i=0;i<32;i++)
             {  int ch = *f++ ;
                 if(ch>>7) { (s3m.chnSetting)[i] = 127 ; (s3m.chnSetting)[i+32] = -1 ; (s3m.chnSwitch)[i] = 255 ; } // unactive or unabled channel
                  else
                   { s8 *s = s3m.chnSetting + i ;
                     if(ch<8) { *s = -1 ; *(s+32) = ch+1 ; } // left
                     else if(ch<16) { *s = 1 ; *(s+32) = ch-7 ; } // right
                          else if(ch<32) { *s = 0  ; *(s+32) = ch-15 ; } // adlib chn
                               else     { *s = 42 ; *(s+32) = -1 ; continue ; }    // error <- maybe can try a fix here !
                     (s3m.chnSwitch)[i] = (*p)++ ;
                     //sprintf(dbg,"\nchn %i -> %i",i,(s3m.chnSwitch)[i]);log(dbg);
                   }
             };
            //sprintf(dbg,"%i channels",*s3m.chnNb) ;log(dbg);
          }

/*			printlr("there is %i channels ..\nchannels setting",*s3m.chnNb);
			for(i=0;i<*s3m.chnNb;i++)
			{	if((s3m.chnSetting)[i]<0)
					printlr("[%i]->[%i] (left)",i,(s3m.chnSwitch)[i]);
				else 
				{	if((s3m.chnSetting)[i])
						printlr("[%i]->[%i] (right)",i,(s3m.chnSwitch)[i]);
					else printlr("[%i]->[%i] **error**)",i,(s3m.chnSwitch)[i]);
				}
			};
*/
//		log("create and fill order array");
		  // create and fill order array

//			printlr("there is %i patterns and %i order ..",*(s3m.ptnNb),*(s3m.orderNb));

           { u8 n=0 ; i = *(s3m.orderNb) ; char txt[256]; *txt=0;
             s3m.order = (u8*)allocAndAddFree(i+42,s3m.freeList);
             while(*f != 255 && i--)
             {   while(*f == 254)  f++ ;
                 /*f++ ; n++;*/s3m.order[n++] = *f++ ;
             };  if(*f == 255) { f++ ; --i ; } else --i ; if(i>0) f+=i ;
                 s3m.order[n] = 0xff ;
				 *s3m.orderNb = n ;
				// sprintf(dbg,"%i order .. pass 1",n);log(dbg);
/*				 sprintf(txt,"order list .. [%i",*(s3m.order));
				 for(u32 z=1;z<=n;z++)
					 sprintf(txt,"%s.%i",txt,s3m.order[z]);
				sprintf(txt,"%s]",txt);
				log(txt);*/
           }

//			printlr("finally there are %i orders..",*(s3m.orderNb));
//			printlr("there are %i samples, load them..",*s3m.sampleNb);
			
           //sprintf(dbg,"load samples (%i)",*s3m.sampleNb);log(dbg);
		   // load samples
           {  u16*p = (u16*)f ; u8 *off ; u8 valid=0 ;
              f += (*s3m.sampleNb)*2 ;
			  //if( ((int)f)&1 ) log("f not aligned");
			  //else log("f aligned");
              s3m.sample = (struct sample **)allocAndAddFree(4*(*s3m.sampleNb),s3m.freeList) ;
			 // log("samples alocated");
             for(i=0;i<*s3m.sampleNb;i++)
              { off = (*p++)*16 + s3m.file ;
			 //if( ((int)off)&1 ) log("off not aligned");
			 if( off - s3m.file < 0 || off - s3m.file > fileSize)
				{	//log("sample offset error");
					s3m.sample[i] = 0 ;
				} else {
					switch(*off)
					 { case 0 :
					   case 1 : s3m.sample[i] = loadSample(off, song,fileSize) ; addFreeEntry(s3m.sample[i],s3m.freeList); break ;
					   default: s3m.sample[i] = 0 ; break; /* adlib melody or drum, unsuported */
					 };
				}

				//if(!(s3m.sample[i])) printlr("\t%2i got an error.",i);
              };

/*			 log("\nsamples___");

            { struct sample *s ; u32 z=0 ;
              while(z < *s3m.sampleNb) { s = s3m.sample[z++] ;
                                         if(s) { if(*(s->flag) != 142) //    printf(" empty") ; else
										 {      char txt[256]; *txt=0;
                                                        sprintf(txt,"%2i\t| ",z) ;
                                                        if(*(s->packed))   sprintf(txt,"%s packed ",txt) ;
                                                        else               sprintf(txt,"%sunpacked",txt) ;
                                                        sprintf(txt,"%s | vol %i | sze %i\t",txt,*(s->volume),*(s->size)) ;
														//if(*(s->size)>9999) sprintf(txt,"%s\t",txt);
														if(*(s->size)<100)  sprintf(txt,"%s\t",txt);
                                                        if(*(s->in16bits)) sprintf(txt,"%s16bits",txt) ;
                                                        else               sprintf(txt,"%s 8bits",txt) ;
                                                        if(*(s->stereo))   sprintf(txt,"%s stereo",txt) ;
                                                        else               sprintf(txt,"%s  mono ",txt) ;

                                                        if(*(s->looped))
                                                             sprintf(txt,"%s | looped [%4i.%4i]",txt,*(s->loopStart),*(s->loopEnd)) ;
                                                        else sprintf(txt,"%s\t\t",txt) ;
                                                        sprintf(txt,"%s\t| %16s\t| %s",txt,s->fname,s->name) ;
														log(txt);
												} else printlr("%2i ************\t\t\t\t\t\t\t\t\t| %s",z,s->name);
                                               } else printlr("%2i is bad",z) ;
              };
            }
*/
	//	log("samples loaded");
		//	sprintf(dbg,");log(dbg);
		//	sprintf(dbg,"%i chn . %i ptn . %i spl . %i ord",*s3m.chnNb,*s3m.ptnNb,*s3m.sampleNb,*s3m.orderNb);log(dbg);

              s3m.ptn = (u8**)allocAndAddFree((*s3m.ptnNb)<<2,s3m.freeList) ;
              memset(s3m.ptn,0,4*(*s3m.ptnNb)) ;
              
			i=0 ; u8 nb;
			//log("** enter do");
			//log("get patterns");

//			printlr("\nnow get the %i patterns",*s3m.ptnNb);

			while(s3m.order[i] != 0xff)
			{   
				nb = s3m.order[i] ;

				//printlr("procced order %2i -> pattern %2i",i,nb);

				//sprintf(dbg,"order %i\tpattern %i",i,nb) ; log(dbg);
				//if(*s3m.ptnNb<=nb) log("**** order too big !!!");
				
/*				if(s3m.ptn[nb] == emptyPattern)
					printlr("\tpattern %i empty",nb);
*/				
				if(s3m.ptn[nb] == 0 && s3m.ptn[nb] != emptyPattern)
	            {	//printlr("\tload pattern %3i (from order %2i)",nb,i);
					off = (p[ s3m.order[i] ])*16 + song ;
					//sprintf(dbg,"  offset %x",(p[ s3m.order[i] ])*16); log(dbg);
					if(off == song || (fileSize && off-song >= fileSize)) // bad offset, skip pattern
					{	//sprintf(dbg,"bad offset %i in pattern %i",off,nb); log(dbg); 
						//s3m.ptn[nb] = 0; i++;//(u8*)0xffff ; i++;
						s3m.ptn[nb] = emptyPattern ; i++ ;
					} else {	s3m.ptn[nb] = (u8*)allocAndAddFree(64*((*s3m.chnNb))*5,s3m.freeList) ;
								u8 *ptr = off+2, *o = s3m.ptn[nb], *chn=o, v ;
								int nb=0,size = ((off[1])*256)+*off;
								memcpy(o,emptyraw,(*s3m.chnNb)*5);			   
								//sprintf(dbg,"offset %x size %i",off-song,size) ;log(dbg);
								//if(fileSize)if(fileSize < size){sprintf(dbg,"pattern offset is bigger than file !");log(dbg);}
								//
								//log("  start to read pattern data..");
								nb=0; u8*start = ptr;
								while(nb < 64)
								{	v = *ptr++ ;
									if(ptr-start > (*s3m.chnNb)*5*2)
									{ // is really a pattern ? presume, no
								//		sprintf(dbg,"is %i really a pattern ? presume, no",s3m.order[i]) ; log(dbg);
										//s3m.ptn[nb] = 0;
										s3m.ptn[s3m.order[i]] = emptyPattern ;
										break;
									}

									if(!v) { //sprintf(dbg,"end of raw %i .. size - (ptr-off) = %i",nb,size - (ptr-off));log(dbg);
											   if(nb++ < 63){ o+=(*s3m.chnNb)*5 ; memcpy(o,emptyraw,(*s3m.chnNb)*5); start=ptr; }
											   else          { int z = size - (ptr-off) ;
															   // in "Mailbox-on-NES" z= -2, what is the tracker ?
																//sprintf(dbg,"");
																start=ptr;
																/*if(z && z!=-2)
																{ sprintf(dbg,"error about %i in pattern %i .. real size %i",z,s3m.order[i],ptr-off) ; log(dbg);
																if(fileSize)	
																	if(fileSize < ptr-off)
																	{	sprintf(dbg,"pattern out of file :|") ; log(dbg); }
																}// else sprintf(dbg,"ok") ;
																*/
															 }
											}
									 else {	if((v&31) > *s3m.chnNb) // curent channel more than max !
											{	//printlr("\t ** error ** channel number upper than max ! : %i",v&31);
												// madtracker up channels number, here only ignore and skip
												if(v&32)  ptr+=2;
												if(v&64)  ptr++ ;
												if(v&128) ptr+=2;
											} else { // it's a good channel, fill info found
												chn = o + ((s3m.chnSwitch)[v&31])*5 ;
												//sprintf(dbg,"channel %i",chnb) ;log(dbg);
												if(v&32) { //sprintf(dbg,"note : %s-%i %x",note[(*ptr)&0x0f],(*ptr)>>4,ptr[1]) ;log(dbg);
														   *chn = *ptr++ ;  // copy note
														   *(chn+1) = *ptr++ ; // and sample nb
														 }
												if(v&64) { //printf(" volume %x",*ptr) ;
														   *(chn+2) = *ptr++ ; // copy volume value
														 }
												if(v&128){ //printf(" command %x info %x",*ptr,ptr[1]) ;
														   *(chn+3) = *ptr++ ;  // copy command nb
														   *(chn+4) = *ptr++ ;  // and command arg
														 }
											}
										 }
								}; // while end
								//log("data get.");
							  ++i ;
					} // else end
				} else { ++i ; } 
           };//while(s3m.order[i] < 99) ;
			//log("** do end");
			//sprintf(dbg,"out of read patterns.") ;log(dbg);

			//logPtn0Chn1(&s3m);

		/* recheck song pattern list to delete unloaded and/or bad patterns */
//			log("recheck song pattern order list");
/*
		u8*orderPtr = s3m.order ;
		u8*orderPt2 = s3m.order ;

		while(*orderPt2 != 255)// && orderPt2<s3m.order+*s3m.orderNb)
		{	if(*orderPt2 <= *s3m.ptnNb)
				if(s3m.ptn[*orderPt2] && s3m.ptn[*orderPt2]!=emptyPattern)
					*orderPtr++ = *orderPt2 ;
			orderPt2++;
		};	*orderPtr = 255 ;
		
		if(orderPtr!=orderPt2)
		{	sprintf(dbg,"skip %i bad order",orderPt2-orderPtr); log(dbg); }


		*dbg=0; int e=0;

		//for(int e=0;e<*s3m.orderNb;e++)
		while(s3m.order[e] < 254)
			{	sprintf(dbg,"%s%i ",dbg,s3m.order[e]) ;
				//if(s3m.order[e] >= 254) { *s3m.orderNb=e; break;}
				e++;
			};*s3m.orderNb=e;
		sprintf(dbg,"%s%i ",dbg,s3m.order[e]) ;
		log(dbg);
		sprintf(dbg,"finally there are %i orders",*s3m.orderNb) ;log(dbg);
*/
		   }
//system("debug.log");
   out = (struct s3m *)malloc(sizeof(struct s3m)) ;
   s3m.mstVol = (u8*)(&(out->adData)) ;
   s3m.stereo = (s3m.mstVol)+1 ;
   memcpy(out,&s3m,sizeof(struct s3m)) ;
//sprintf(dbg,"%i channels",*out->chnNb) ;log(dbg);
   return out ;
}

void freeS3m(struct s3m*s3m)
{	freeAllEntry(s3m->freeList);
	free(s3m);
}
/*
void logPtn0Chn1(struct s3m*s3m)
{	log("\n channel 1 of pattern 0");
	u8 * ptn = s3m->ptn[s3m->order[0]] ;
	for(int n=0;n<64;n++)
	{	u8 * nt = ptn + (*s3m->chnNb)*5*n + 5 ;
		u32 note = (*nt)&0x0f;
		if(note > 11)	printlr("~~~ %i",nt[1]);
		else printlr("%s%i %i",s3note[note],(*nt)>>4,nt[1]);
	};
}*/

//sprintf(dbg,"note : %s-%i %x",note[(*ptr)&0x0f],(*ptr)>>4,ptr[1]) ;log(dbg);
