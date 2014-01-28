
#ifndef _gdlSound_
#define _gdlSound_

#include "../../Gdl.h"

FMUSIC_MODULE *gzk=0;

struct mod * mymod = 0;
u8 soundWasInit=0;

u32 Gdl_iniSound(void)
{  
	#ifdef useMiniFmod
		FMUSIC_MemInit();
		#ifdef iWantSfx
			FSOUND_File_SetExtraCallbacks(&extraMix,0,0,0);
		#endif
		soundWasInit = 1;
		return 1;
	#endif

	#ifdef useBass
		return 0;
	#endif

	return 0;
}

u32 Gdl_playSong(u8* song,u32 size)
{	if(!song || !size) return 0;
	if(!soundWasInit) Gdl_iniSound();
	#ifdef useMiniFmod
		MEMFILE mzk ;
		mzk.pos=0 ;
		mzk.length=size;
		mzk.data=(char*)song;
		gzk = FMUSIC_LoadSong((char*)&mzk,0);
		if(gzk)
		{	FMUSIC_PlaySong(gzk);
			return 1;
		}
	#endif
	return 0;
}

#ifdef useUnrarlib
	u32 Gdl_playSong(u8*rar,u32 rarSize,const char * songPath)
	{	unsigned long size;
		char * file = 0;

		MemoryFile	datarar;
		datarar.data	= rar ;
		datarar.offset	= 0 ;
		datarar.size	= rarSize;

		if(urarlib_get(&file,&size,(char*)songPath,&datarar,0))
			return Gdl_playSong((u8*)file,size);
		#ifdef showMeError
			sprintf(dbg,"cannot uncompress %s from memory rar 0x%x",songPath,rar);
			showMsg(dbg,"error");
		#endif
		return 0;
	}
#endif

u32 Gdl_playSong(const char * songPath)
{	if(!songPath) return 0;
	u32 size;
	#ifdef useUnrarlib
		char*path = (char*)songPath;
		while(*path && *path != '.') path++;
		if(*path)
		{	if(path[1] && (path[1] == 'r' || path[1] == 'R'))
			 if(path[2] && (path[2] == 'a' || path[2] == 'A'))
			  if(path[3] && (path[3] == 'r' || path[3] == 'R')) // .rar found in path
				{	// it's an external rar, now see if we want a specific file..
					char * rpath = (char*)malloc(512);
					memcpy(rpath,songPath,(path-songPath)+4);
					
					if(path[4] == '|')
					{	// extract file path
						char * fpath = &path[5];
						char * file = 0;
						unsigned long size;
						
						//sprintf(z,"rar %s\nfile %s",rpath,fpath);
						//showMsg(z,"zzz");
						if(urarlib_get(&file,&size,fpath,rpath,0))
						{	free(rpath);
							return Gdl_playSong((u8*)file,size);
						}
						#ifdef showMeError
							char * z = rpath+256;
							sprintf(z,"cannot uncompress %s from %s",fpath,rpath);
							showMsg(z,"error");
						#endif
						free(rpath); return 0;
					}
						// not any file specified, see into the rar for a single file..

						free(rpath); return 0;
				}
		}

	#endif
	u8* song = loadFile(songPath,&size);
	if(song) return Gdl_playSong(song,size);
	 else return 0;
}

void Gdl_stopSong(u32 method)
{	FMUSIC_StopSong(gzk);
	FMUSIC_FreeSong(gzk);
}

void Gdl_fadeAndStopSong(u32 fadeSpeed)
{	if(gzk)	{	FMUSIC_fadeSound2Zero(fadeSpeed);
				FMUSIC_StopSong(gzk);
				FMUSIC_FreeSong(gzk);
			}
}

/*
void playARandomSong(void)
{	if(currentRAR->data && currentRAR->size)
	{	static int lastPlayed = 0xffff ;
		curentMixBf=0;
		ArchiveList_struct *list,*plop;
		u32 songNb = urarlib_list(currentRAR,(ArchiveList_struct*)&plop);
		
		if(!readLastSong || lastPlayed == 0xffff)
		{	int tmp = lastPlayed ;
			lastPlayed = currentSong ;
			do	{ currentSong = rand()%songNb ;
				} while(lastPlayed == currentSong || tmp == currentSong);
		} else	{	int tmp = currentSong ;
					currentSong = lastPlayed ;
					lastPlayed = tmp ;	
				}

		list = plop ;
		for(int n=0;n<currentSong;n++)
				list = list->next ;
		u8*newZik ;
		if(deadSong) { newZik = empty ; dataSize = emptySize; }
		else	{	if(!urarlib_get(&newZik,&dataSize,(list->item).Name,currentRAR,"r043v"))
						newZik=0;
					else strcpy(sngName,(list->item).Name) ;
				}
		
		if(newZik)	{	
			zk[0].pos=0 ; zk[0].length=dataSize; zk[0].data=newZik;
			mod[1] = mod[0];
			mod[0] = FMUSIC_LoadSong((char*)&(zk[0]), NULL);			
			FMUSIC_changeSong(mod[0]);
			if(mod[1]) FMUSIC_FreeSongWithoutStop(mod[1]);
			mod[1] = 0 ;
			if(*mzIk) free(*mzIk);
			*mzIk = newZik; *mzIkSze = dataSize;
			char * name = (char*)*mzIk ;
			name += 16 ;
			memcpy(sngName,name,21);
			sngName[21]=0;
			sprintf(sngString+sngStringOffset,"%s",sngName);
			sngStringPos = WIDTH ;
			FMUSIC_setVolumeUpdateSpeed(10);
			readLastSong=0;
		} else	{	if(readLastSong) return ; // to prevent infinite loop if last song is bad..
					readLastSong = 1 ;
					playARandomSong();
					return ;
				}
		urarlib_freelist(plop);
	}
}*/

#ifdef iWantSfx

	u32 Gdl_setSfx(u8* sfxSong,u32 size)
	{	if(!sfxSong || !size) return 0;
		#ifdef useModSfx
			mymod = loadMod(sfxSong);
			if(!mymod) return 0;
		#endif

		#ifdef useS3mSfx

		#endif
		
		return 1;
	}

	void Gdl_playSfx(u32 splNb, u32 freq, u32 volume, u32 chn)
	{
	  if(!mymod) return;
	  if(!mymod->samples) return;
	  if(splNb >= mymod->splNb) return;
	  if(!freq) return;
	  if(!volume) return;
	  spl *sample = &(mymod->samples[splNb]);
	  if(!sample) return;
	  if(!sample->data) return;
	  freq = 3546895/freq; //(8287*69434*period)>>16;
  
	  addModSample(	(u8*)sample->data,
					chn,
					freq,
					sample->size,
					volume,
					sample->looped,
					sample->rptStart,
					sample->rptEnd
				  );
	}

#endif

#endif
