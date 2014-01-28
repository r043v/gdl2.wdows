
#define u32 unsigned int
#define s8  signed char
#define u8  unsigned char

// by a change in minifmod, the xm lib will lauch this code after his mix
// it will add and mix all samples played with addSample

void extraMix(short*buffer, int size);

void addModSample(u8*spl,
				  u32 chn,
				  u32 freq,
				  u32 size,
				  u32 volume,
				  u32 looped,
				  u32 loopStart,
				  u32 loopEnd);

extern u32 isMixing;

// will stop sound
void clearMix(void);

extern u32 volumeOfSpl ; // sample volume, will not interfer with song volume
#define maxChannels 32 /* how many sample we can play at the same time ? */
