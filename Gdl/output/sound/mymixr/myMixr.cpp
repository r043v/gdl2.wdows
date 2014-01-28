
#include "myMixr.h"

u32 isMixing=0;

struct mysample
{	int looped,enable ;
	unsigned int zhratio, c4freq, volume, issigned ;
	u8 *data,*start,*end,*pos ;
	unsigned int playDiffered, maxPlayTime ;
};

int sampleNb2Mix=0 ;					// how many samples to mix ?
struct mysample spl2Mix[maxChannels];	// [you can say that's channels]

#include "windows.h"

void clearMix(void)
{	while(isMixing);
	sampleNb2Mix=0 ;
	memset(spl2Mix,0,maxChannels*sizeof(struct mysample));
}

void addModSample(u8*spl,u32 chn,u32 freq,u32 size,u32 volume,u32 looped,u32 loopStart,u32 loopEnd)
{	struct mysample *myspl ;

	if(chn==255)
		{	chn=0; myspl = &(spl2Mix[chn]);
			while(myspl->enable) myspl = &(spl2Mix[++chn]);
	} else 	myspl = &(spl2Mix[chn]);
	
	while(isMixing) Sleep(0); // wait curent mix end.
	
	if(myspl->enable) // unable sample
		{	if(sampleNb2Mix) sampleNb2Mix--;
			myspl->enable = 0 ;
		}

	myspl->zhratio = 88200/freq;

	u32 splStart=0, splEnd=size ;

	if(looped)
	{	splStart = loopStart;
		if(loopEnd<=splEnd) splEnd = loopEnd;
	}

	u8*data = spl ;
	myspl->issigned = 1 ;	// mod sample are signed.
	myspl->volume = volume ;		// from 0 to 64
	myspl->pos    = data ;
	myspl->start  = data + splStart ;
	myspl->end    = data + splEnd ;
	myspl->data   = data ;
	myspl->c4freq = 8287; // in fact it's c2 here.
	myspl->looped = looped ;
	myspl->playDiffered = 0 ;
	myspl->maxPlayTime = 0xffff ;

	while(isMixing) Sleep(0);
	myspl->enable = 1 ; sampleNb2Mix++;
}

#define spl2Bf(pos) (pos*(spl->zhratio))

u32 volumeOfSpl = 255 ; // value : 0 to 255

	// made an extra mix with the output buffer of minifmod
void extraMix(short*buffer, int size)
{	static u32 lastVolume=volumeOfSpl ;

	if(lastVolume != volumeOfSpl)
	{	if(lastVolume>volumeOfSpl)
		{	if(lastVolume-volumeOfSpl > 5) lastVolume-=5 ;
			else lastVolume--;
		} else {	if(volumeOfSpl-lastVolume > 5) lastVolume+=5 ;
					else lastVolume++;
		}
	}
	
	if(!lastVolume) return ;
	isMixing=1;
	struct mysample *spl ;
	u32 sampleMixed=sampleNb2Mix ;
	u32 splNb=0;
	
//	sprintf(ztext,"extra mix of %i samples | %i %i | %i",sampleNb2Mix,volumeOfSpl,lastVolume,(*spl2Mix).c4freq);
	
	//memset(buffer,0,size*2);

	while(sampleMixed)
	{	spl = &(spl2Mix[splNb++]);

		if(spl && spl->enable)
		{	short * bf = buffer ;
			short * bfEnd = bf ; bfEnd += size ;
			short * next = &(buffer[spl2Bf(1)]) ;
			int		value, bfValue ;
			int n=2;
		/*	
			if(spl->playDiffered) { spl->playDiffered--; goto nextSpl; }
			if(spl->maxPlayTime!=0xffff)
			{	if(!spl->maxPlayTime--)
				{	sampleNb2Mix--; spl->enable=0; goto nextSpl;
				}
			}
		*/	
					value = (signed char)*spl->pos;

					value <<= 8 ;			// set as 16bits
					//if(!spl->issigned) value -= 32767 ;		// convert to signed if needed

					if(spl->volume < 64) // apply sample volume
					{	value *= spl->volume ; // value are from 0 to 64
						value>>=6;
					}

					if(lastVolume!=255)	// apply global mixer volume
					{	value *= lastVolume; // volume go from 0 to 255
						value >>= 8;
					}

			while(bf < bfEnd)
			{	while(bf<next)
				{	bfValue = *bf ;
					if(bfValue)
						{	//bfValue >>= 1;
							bfValue += value ;
							bfValue >>= 1;
							if(bfValue < -32767) bfValue = -32767 ;
							  else
							if(bfValue > 32767) bfValue = 32767 ;
						} else bfValue = value ;

					*bf++ = bfValue ;
					*bf++ = bfValue ;
				};
					spl->pos++ ;
					if(spl->pos >= spl->end)
					{	spl->pos = spl->start;
						if(spl->looped == 0) { sampleNb2Mix--; spl->enable=0; goto nextSpl; }
					}

					value = (signed char)*spl->pos;

					value <<= 8 ;			// set as 16bits
					//if(!spl->issigned) value -= 32767 ;		// convert to signed
					
					if(spl->volume < 64) // sample volume
					{	value *= spl->volume ;
						value>>=6;
					}

					if(lastVolume!=255) // global sample volume
					{	value *= lastVolume; // volume go from 0 to 255
						value >>= 8;
					}

					next = &(buffer[spl2Bf(n++)]);
					if(next >= bfEnd) break ;// next = curentMixSz-1;
			};
				nextSpl:sampleMixed--;
		}
	}
	
	isMixing=0;
}

