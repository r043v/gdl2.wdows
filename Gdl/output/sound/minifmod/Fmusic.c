/******************************************************************************/
/* FMUSIC.C                                                                   */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Technologies will not support    */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) Firelight Technologies, 2000-2004.    */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Technologies is a registered company.                            */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

#include "minifmod.h"

#include "mixer_fpu_ramp.h"
#include "music.h"
#include "music_formatxm.h"
#include "sound.h"
#include "system_file.h"
#include "system_memory.h"

FMUSIC_MODULE *		FMUSIC_PlayingSong = NULL;
FMUSIC_CHANNEL		FMUSIC_Channel[32];		// channel array for this song
FMUSIC_TIMMEINFO *	FMUSIC_TimeInfo;
FSOUND_SAMPLE		FMUSIC_DummySample;
// initialization taken out due to size.. should be ok.
/* =
{
//	0,								// index
	NULL,							// buff

	0,								// length
	0,								// loopstart
	0,								// looplen
	0,								// default volume
	0,								// finetune value from -128 to 127 

	44100,							// default frequency
	128,							// default pan

	8,								// bits
	FSOUND_LOOP_OFF,				// loop mode


	TRUE,							// music owned
	0,								// sample global volume (scalar)
	0,								// relative note 
	8363,							// finetuning adjustment to make for music samples.. relative to 8363hz
	0,								// sample loop start
	0,								// sample loop length
	0,								// vibrato speed 0-64 
	0,								// vibrato depth 0-64 
	0,								// vibrato type 0=sine, 1=rampdown, 2=square, 3=random
	0,								// vibrato rate 0-64 (like sweep?) 
};
*/

FSOUND_CHANNEL			FMUSIC_DummyChannel;
FMUSIC_INSTRUMENT		FMUSIC_DummyInstrument;

//= PRIVATE FUNCTIONS ==============================================================================


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/

void FMUSIC_SetBPM(FMUSIC_MODULE *module, int bpm)
{
	float hz;

	module->bpm = bpm;

	hz = (float)bpm * 2.0f / 5.0f;
	
	// number of samples
	module->mixer_samplespertick = (int)((float)FSOUND_MixRate * (1000.0f / hz) / 1000.0f);	
}


//= API FUNCTIONS ==============================================================================

/*
[API]
[
	[DESCRIPTION]
	To load a module with a given filename.  FMUSIC Supports loading of 
	- .MOD (protracker/fasttracker modules)
	- .S3M (screamtracker 3 modules)
	- .XM  (fasttracker 2 modules)
	- .IT  (impulse tracker modules)

	[PARAMETERS]
	'name'		Filename of module to load.
 
	[RETURN_VALUE]
	On success, a pointer to a FMUSIC_MODULE handle is returned.
	On failure, NULL is returned.

	[REMARKS]
	This function autodetects the format type, regardless of filename.

	[SEE_ALSO]
	FMUSIC_FreeSong
]
*/

//#include "../gdl/gdl.h"

FMUSIC_MODULE * FMUSIC_LoadSong(signed char *name, SAMPLELOADCALLBACK sampleloadcallback)
{   FMUSIC_MODULE		*mod;
    signed char			retcode=FALSE;
    FSOUND_FILE_HANDLE	*fp;

//if(printlg) printlg("enter load song..");

    // create a mod instance
    mod = FSOUND_Memory_Calloc(sizeof(FMUSIC_MODULE));
//if(printlg) printlg("mem allocated..");
	if(!FSOUND_File_Open) return 0;
    fp = FSOUND_File_Open(name, 0, 0);
    if (!fp)
    {  return NULL;
    }
//if(printlg) printlg("file open");
    mod->samplecallback = sampleloadcallback;
//if(printlg) printlg("now really load the xm..");
    // try opening all as all formats until correct loader is found
	retcode = FMUSIC_LoadXM(mod, fp);
//if(printlg) printlg("loaded :) .. %i",retcode);
    FSOUND_File_Close(fp);
//if(printlg) printlg("file closed");
    if (!retcode)
    {   FMUSIC_FreeSong(mod);
        return NULL;
    }
//if(printlg) printlg("all ok");
    return mod;
}


/*
[API]
[
	[DESCRIPTION]
	Frees memory allocated for a song and removes it from the FMUSIC system.

	[PARAMETERS]
	'mod'		Pointer to the song to be freed.
 
	[RETURN_VALUE]
	void

	[REMARKS]

	[SEE_ALSO]
	FMUSIC_LoadSong
]
*/

signed char FMUSIC_FreeSongWithoutStop(FMUSIC_MODULE *mod)
{
	int count;

	if (!mod) 
		return FALSE;

	// free samples
	if (mod->instrument)
	{
		for (count=0; count<(int)mod->numinsts; count++) 
		{
			int count2;

			FMUSIC_INSTRUMENT	*iptr = &mod->instrument[count];
			for (count2=0; count2<iptr->numsamples; count2++) 
			{
				if (iptr->sample[count2])
				{
					FSOUND_SAMPLE *sptr = iptr->sample[count2];
					FSOUND_Memory_Free(sptr->buff);
					FSOUND_Memory_Free(sptr);
				}
			}


		}
	}

	// free instruments
	if (mod->instrument)
    {
		FSOUND_Memory_Free(mod->instrument);
    }

	// free patterns
	if (mod->pattern)
	{
		for (count=0; count<mod->numpatternsmem; count++)
        {
			if (mod->pattern[count].data) 
            {
				FSOUND_Memory_Free(mod->pattern[count].data);
            }
        }

		if (mod->pattern) 
        {
			FSOUND_Memory_Free(mod->pattern);
        }
	}

	// free song
	FSOUND_Memory_Free(mod);

	return TRUE;
}

signed char FMUSIC_FreeSong(FMUSIC_MODULE *mod)
{
	int count;

	if (!mod) 
		return FALSE;

	BLOCK_ON_SOFTWAREUPDATE();

	FMUSIC_StopSong(mod);

	// free samples
	if (mod->instrument)
	{
		for (count=0; count<(int)mod->numinsts; count++) 
		{
			int count2;

			FMUSIC_INSTRUMENT	*iptr = &mod->instrument[count];
			for (count2=0; count2<iptr->numsamples; count2++) 
			{
				if (iptr->sample[count2])
				{
					FSOUND_SAMPLE *sptr = iptr->sample[count2];
					FSOUND_Memory_Free(sptr->buff);
					FSOUND_Memory_Free(sptr);
				}
			}


		}
	}

	// free instruments
	if (mod->instrument)
    {
		FSOUND_Memory_Free(mod->instrument);
    }

	// free patterns
	if (mod->pattern)
	{
		for (count=0; count<mod->numpatternsmem; count++)
        {
			if (mod->pattern[count].data) 
            {
				FSOUND_Memory_Free(mod->pattern[count].data);
            }
        }

		if (mod->pattern) 
        {
			FSOUND_Memory_Free(mod->pattern);
        }
	}

	// free song
	FSOUND_Memory_Free(mod);

	return TRUE;
}


/*
[API]
[
	[DESCRIPTION]
	Starts a song playing.

	[PARAMETERS]
	'mod'		Pointer to the song to be played.
 
	[RETURN_VALUE]
	TRUE		song succeeded playing
	FALSE		song failed playing

	[REMARKS]

	[SEE_ALSO]
	FMUSIC_StopSong
]
*/

signed char FMUSIC_PlaySong(FMUSIC_MODULE *mod)
{
	int				count;
	FMUSIC_CHANNEL	*cptr;
	int				totalblocks; 

	if (!mod) 
    {
		return FALSE;
    }

	lastmodplay = mod ;
	volumeUpdateSpeed = 30 ;
	FMUSIC_StopSong(mod);

	if (!FSOUND_File_OpenCallback || !FSOUND_File_CloseCallback || !FSOUND_File_ReadCallback || !FSOUND_File_SeekCallback || !FSOUND_File_TellCallback)
    {
		return FALSE;
    }

	// ========================================================================================================
	// INITIALIZE SOFTWARE MIXER 
	// ========================================================================================================

	FSOUND_OOMixRate    = 1.0f / (float)FSOUND_MixRate;
	FSOUND_BlockSize    = ((FSOUND_MixRate * FSOUND_LATENCY / 1000) + 3) & 0xFFFFFFFC;	// Number of *samples*
	FSOUND_BufferSize   = FSOUND_BlockSize * (FSOUND_BufferSizeMs / FSOUND_LATENCY);	// make it perfectly divisible by granularity
	FSOUND_BufferSize <<= 1;	// double buffer

	mix_volumerampsteps      = FSOUND_MixRate * FSOUND_VOLUMERAMP_STEPS / 44100;
	mix_1overvolumerampsteps = 1.0f / mix_volumerampsteps;
    totalblocks              = FSOUND_BufferSize / FSOUND_BlockSize;

	//=======================================================================================
	// ALLOC GLOBAL CHANNEL POOL
	//=======================================================================================
	memset(FSOUND_Channel, 0, sizeof(FSOUND_CHANNEL) * 256);

	// ========================================================================================================
	// SET UP CHANNELS
	// ========================================================================================================

	for (count=0; count < 256; count++)
	{
		FSOUND_Channel[count].index = count;
		FSOUND_Channel[count].speedhi = 1;
	}

	mod->globalvolume       = mod->defaultglobalvolume;
	mod->globalWantedVolume = mod->globalvolume ;
 	mod->speed              = (int)mod->defaultspeed;
	mod->row                = 0;
	mod->order              = 0;
	mod->nextorder          = -1;
	mod->nextrow            = -1;
	mod->mixer_samplesleft  = 0;
	mod->tick               = 0;
	mod->patterndelay       = 0;
	mod->time_ms            = 0;

	FMUSIC_SetBPM(mod, mod->defaultbpm);

	memset(FMUSIC_Channel, 0, mod->numchannels * sizeof(FMUSIC_CHANNEL));
//	memset(FSOUND_Channel, 0, 256 * sizeof(FSOUND_CHANNEL));

	for (count=0; count < mod->numchannels; count++)
	{
		cptr = &FMUSIC_Channel[count];
		cptr->cptr = &FSOUND_Channel[count];
	}

	FMUSIC_PlayingSong = mod;

	FMUSIC_TimeInfo = FSOUND_Memory_Calloc(sizeof(FMUSIC_TIMMEINFO) * totalblocks);

	// ========================================================================================================
	// PREPARE THE OUTPUT
	// ========================================================================================================
	{
		WAVEFORMATEX	pcmwf;
		UINT			hr;

		// ========================================================================================================
		// INITIALIZE WAVEOUT
		// ========================================================================================================
		pcmwf.wFormatTag		= WAVE_FORMAT_PCM; 
		pcmwf.nChannels			= 2;
		pcmwf.wBitsPerSample	= 16; 
		pcmwf.nBlockAlign		= pcmwf.nChannels * pcmwf.wBitsPerSample / 8;
		pcmwf.nSamplesPerSec	= FSOUND_MixRate;
		pcmwf.nAvgBytesPerSec	= pcmwf.nSamplesPerSec * pcmwf.nBlockAlign; 
		pcmwf.cbSize			= 0;

		hr = waveOutOpen(&FSOUND_WaveOutHandle, WAVE_MAPPER, &pcmwf, 0, 0, 0);

		if (hr) 
        {
			return FALSE;
        }
	}

	{
		WAVEHDR	*wavehdr;
		int	length = 0;

		// CREATE AND START LOOPING WAVEOUT BLOCK
		wavehdr = &FSOUND_MixBlock.wavehdr;

		length = FSOUND_BufferSize;
		length <<= 2;	// 16bits

		FSOUND_MixBlock.data = FSOUND_Memory_Calloc(length);
		
		wavehdr->dwFlags			= WHDR_BEGINLOOP | WHDR_ENDLOOP;
		wavehdr->lpData				= (LPSTR)FSOUND_MixBlock.data;
		wavehdr->dwBufferLength		= length;
		wavehdr->dwBytesRecorded	= 0;
		wavehdr->dwUser				= 0;
		wavehdr->dwLoops			= -1;
		waveOutPrepareHeader(FSOUND_WaveOutHandle, wavehdr, sizeof(WAVEHDR));
	}

	// ========================================================================================================
	// ALLOCATE MIXBUFFER
	// ========================================================================================================

	FSOUND_MixBufferMem = (signed char *)FSOUND_Memory_Calloc((FSOUND_BufferSize << 3) + 256);
	FSOUND_MixBuffer = (signed char *)(((unsigned int)FSOUND_MixBufferMem + 15) & 0xFFFFFFF0);

	// ========================================================================================================
	// PREFILL THE MIXER BUFFER 
	// ========================================================================================================

	do 
	{
		FSOUND_Software_Fill();
	} while (FSOUND_Software_FillBlock);

	// ========================================================================================================
	// START THE OUTPUT
	// ========================================================================================================

	waveOutWrite(FSOUND_WaveOutHandle, &FSOUND_MixBlock.wavehdr, sizeof(WAVEHDR));

	{
		DWORD	FSOUND_dwThreadId;

		// ========================================================================================================
		// CREATE THREADS / TIMERS (last)
		// ========================================================================================================
		FSOUND_Software_Exit = FALSE;

		FSOUND_Software_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FSOUND_Software_DoubleBufferThread, 0,0, &FSOUND_dwThreadId);

		SetThreadPriority(FSOUND_Software_hThread, THREAD_PRIORITY_TIME_CRITICAL);	// THREAD_PRIORITY_HIGHEST);
	}
	return TRUE;
}


/*
[API]
[
	[DESCRIPTION]
	Stops a song from playing.

	[PARAMETERS]
	'mod'		Pointer to the song to be stopped.
 
	[RETURN_VALUE]
	void

	[REMARKS]

	[SEE_ALSO]
	FMUSIC_PlaySong
]
*/
signed char FMUSIC_StopSong(FMUSIC_MODULE *mod)
{
	if (!mod) 
    {
		return FALSE;
    }

	// Kill the thread
	FSOUND_Software_Exit = TRUE;

	// wait until callback has settled down and exited
	BLOCK_ON_SOFTWAREUPDATE();

	if (FSOUND_Software_hThread) 
	{
		while (!FSOUND_Software_ThreadFinished) 
        {
			Sleep(10);
        }
		FSOUND_Software_hThread = NULL;
	}

	// remove the output mixbuffer
	if (FSOUND_MixBufferMem)
    {
		FSOUND_Memory_Free(FSOUND_MixBufferMem);
        FSOUND_MixBufferMem = 0;
    }

    if (FSOUND_MixBlock.wavehdr.lpData)
    {
    	waveOutUnprepareHeader(FSOUND_WaveOutHandle, &FSOUND_MixBlock.wavehdr, sizeof(WAVEHDR));
	    FSOUND_MixBlock.wavehdr.dwFlags &= ~WHDR_PREPARED;
        
        FSOUND_Memory_Free(FSOUND_MixBlock.wavehdr.lpData);
        FSOUND_MixBlock.wavehdr.lpData = 0;
    }

	FMUSIC_PlayingSong = NULL;

	if (FMUSIC_TimeInfo)
    {
		FSOUND_Memory_Free(FMUSIC_TimeInfo);
        FMUSIC_TimeInfo = 0;
    }

	// ========================================================================================================
	// SHUT DOWN OUTPUT DRIVER 
	// ========================================================================================================
	waveOutReset(FSOUND_WaveOutHandle);

	waveOutClose(FSOUND_WaveOutHandle);

	FSOUND_Software_FillBlock = 0;
    FSOUND_Software_RealBlock = 0;

	return TRUE;
}

//= INFORMATION FUNCTIONS ======================================================================

/*
[API]
[
	[DESCRIPTION]
	Returns the song's current order number

	[PARAMETERS]
	'mod'		Pointer to the song to retrieve current order number from.
 
	[RETURN_VALUE]
	The song's current order number.
	On failure, 0 is returned.

	[REMARKS]

	[SEE_ALSO]
	FMUSIC_GetPattern
]
*/
int FMUSIC_GetOrder(FMUSIC_MODULE *mod)
{
	if (!mod || !FMUSIC_TimeInfo)
    {
		return 0;
    }

	return FMUSIC_TimeInfo[FSOUND_Software_RealBlock].order;
}


/*
[API]
[
	[DESCRIPTION]
	Returns the song's current row number.

	[PARAMETERS]
 	'mod'		Pointer to the song to retrieve current row from.

	[RETURN_VALUE]
	On success, the song's current row number is returned.
	On failure, 0 is returned.

	[REMARKS]

	[SEE_ALSO]
]
*/
int FMUSIC_GetRow(FMUSIC_MODULE *mod)
{	if (!mod || !FMUSIC_TimeInfo) 
    {
		return 0;
    }

	return FMUSIC_TimeInfo[FSOUND_Software_RealBlock].row;
}

//**** add by rov
// will return progress of the song -> 0-100 value
int FMUSIC_GetProgress(void)
{	FMUSIC_MODULE *mod = lastmodplay ;
	static FMUSIC_MODULE *last=0 ;
	static float percentPerPattern, percentPerRaw ;
	static int	 numOrder=0, numRaw=0 ;
	static int lastProgress = 0 ;
	int progress ;
	if(!mod) return 0;
	if(mod != last) { lastProgress = 0 ; last=mod; }
	
	if(numOrder != mod->numorders || numRaw != mod->pattern->rows)
	{	percentPerPattern = 100 ;
		percentPerPattern /= mod->numorders ;
		numOrder = mod->numorders ;
		percentPerRaw = (float)(percentPerPattern/64);//>>6);// /numRaw) ;
	}
	
	progress = (int)(percentPerPattern*(FMUSIC_TimeInfo[FSOUND_Software_RealBlock].order));
	progress += (int)((FMUSIC_TimeInfo[FSOUND_Software_RealBlock].row) * percentPerRaw) ;
	
	if(onSoundEnd) // on song finish callback
	{	if(		(lastProgress > progress || progress >= 98)
			&& FMUSIC_TimeInfo[FSOUND_Software_RealBlock].ms > 90000) // 90 second minimum of playtime for a song
		{	if(onNoVolume)
			{	volumeUpdateSpeed = 24 ;
				FMUSIC_setWantedVolume(0);
				lastProgress = 0 ;
			} else onSoundEnd();
		}
	  lastProgress = progress ;
	}

	return progress ;
}

int retreve(char what)
{	if(!lastmodplay) return 0 ;
	if(what == 'o') return lastmodplay->order ;
	return lastmodplay->numorders ;
}

void FMUSIC_fadeSound2Zero(int speed) // launch it at windows close to fade before really quit :)
{	FMUSIC_MODULE *mod = lastmodplay ;
	if(!mod) return ;
	onNoVolume = 0 ;
	volumeUpdateSpeed = speed ;
	FMUSIC_setWantedVolume(0);
	while(mod->globalvolume) // take care it's blocking
	{	Sleep(200);
	};
}

void FMUSIC_changeSong(FMUSIC_MODULE *newMod)
{	int count ;

	memset(FSOUND_Channel, 0, sizeof(FSOUND_CHANNEL) * 256);
	for (count=0; count < 256; count++)
	{
		FSOUND_Channel[count].index = count;
		FSOUND_Channel[count].speedhi = 1;
	}

	newMod->globalWantedVolume = 64 ;
	newMod->globalvolume = 1;
	newMod->speed              = (int)newMod->defaultspeed;
	newMod->row                = 0;
	newMod->order              = 0;
	newMod->nextorder          = -1;
	newMod->nextrow            = -1;
	newMod->mixer_samplesleft  = 0;
	newMod->tick               = 0;
	newMod->patterndelay       = 0;
	newMod->time_ms            = 0;

	FMUSIC_SetBPM(newMod, newMod->defaultbpm);

	memset(FMUSIC_Channel, 0, newMod->numchannels * sizeof(FMUSIC_CHANNEL));
	//memset(FSOUND_Channel, 0, 256 * sizeof(FSOUND_CHANNEL));

	for (count=0; count < newMod->numchannels; count++)
	{	FMUSIC_CHANNEL * cptr ;
		cptr = &FMUSIC_Channel[count];
		cptr->cptr = &FSOUND_Channel[count];
	}

	FMUSIC_PlayingSong = newMod;
	lastmodplay = newMod;
}

void FMUSIC_setVolumeUpdateSpeed(int speed)
{ volumeUpdateSpeed = speed ;
}

//#include "../tinyPtc/Gdl/Gdl.hpp"

int FMUSIC_updateVolume(int time)
{	FMUSIC_MODULE *mod = lastmodplay ;
	static int tck = 0 ;
	int tick = GetTickCount();
	if(!mod || tick - tck < time) return 0;
	if(mod->globalWantedVolume == mod->globalvolume) return 0;
	tck = tick;
	if(mod->globalWantedVolume<mod->globalvolume) // slide down
		{	if(mod->globalvolume) mod->globalvolume--;
			if(!mod->globalvolume) if(onNoVolume) { /*onNoVolume();*/ return 1 ; }
		}
	else	if(mod->globalvolume<mod->globalWantedVolume) mod->globalvolume++;
	return 0;
}

int FMUSIC_setWantedVolume(int volume)
{	FMUSIC_MODULE *mod = lastmodplay ;
	int tmp ;
	if(!mod) return 64 ;
	tmp = mod->globalWantedVolume ;
	mod->globalWantedVolume = volume ;
	return tmp ;
}

void FMUSIC_SetVolume(int volume)
{	FMUSIC_MODULE *mod = lastmodplay ;
	if(!mod) return ;
	mod->globalvolume = volume ;
}

// make a volume fade, to max or min ..
void FMUSIC_SlideVolume(int way)
{	FMUSIC_MODULE *mod = lastmodplay ;
	if(!mod) return ;
	if(!way){	if(mod->globalWantedVolume)		mod->globalWantedVolume--;	}
	else		if(mod->globalWantedVolume<128)	mod->globalWantedVolume++;
}


/*void FMUSIC_GetRaw(int way)
{	FMUSIC_MODULE *mod = lastmodplay ;
	if(!mod) return ;
	
	FMUSIC_PATTERN * p ;
	p = &mod->pattern[mod->orderlist[FMUSIC_TimeInfo[FSOUND_Software_RealBlock].order]];

}*/



//*** rov end

/*
[API]
[
	[DESCRIPTION]
	Returns the time in milliseconds since the song was started.  This is useful for
	synchronizing purposes becuase it will be exactly the same every time, and it is 
	reliably retriggered upon starting the song.  Trying to synchronize using other 
	windows timers can lead to varying results, and inexact performance.  This fixes that
	problem by actually using the number of samples sent to the soundcard as a reference.

	[PARAMETERS]
	'mod'		Pointer to the song to get time from.
 
	[RETURN_VALUE]
	On success, the time played in milliseconds is returned.
	On failure, 0 is returned.

	[REMARKS]

	[SEE_ALSO]
]
*/
unsigned int FMUSIC_GetTime(void)
{	if (!lastmodplay || !FMUSIC_TimeInfo) return 0;
	return FMUSIC_TimeInfo[FSOUND_Software_RealBlock].ms;
}
