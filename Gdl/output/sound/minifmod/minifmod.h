/******************************************************************************/
/* MINIFMOD.H                                                                 */
/* ----------------                                                           */
/* MiniFMOD public source code release.                                       */
/* This source is provided as-is.  Firelight Technologies will not support    */
/* or answer questions about the source provided.                             */
/* MiniFMOD Sourcecode is copyright (c) Firelight Technologies, 2000-2004.    */
/* MiniFMOD Sourcecode is in no way representative of FMOD 3 source.          */
/* Firelight Technologies is a registered company name.                       */
/* This source must not be redistributed without this notice.                 */
/******************************************************************************/

// some add made by rov, you can say it's minifmod 1.8 ..

//==========================================================================================
// MINIFMOD Main header file. Copyright (c), Firelight Technologies, 2000-2004.
// Based on FMOD, copyright (c), Firelight Technologies, 2000-2004.
//==========================================================================================

#ifndef _MINIFMOD_H_
#define _MINIFMOD_H_

//===============================================================================================
//= DEFINITIONS
//===============================================================================================

// fmod defined types
typedef struct FMUSIC_MODULE FMUSIC_MODULE;

//===============================================================================================
//= FUNCTION PROTOTYPES
//===============================================================================================

#ifdef __cplusplus
extern "C" {
#endif

// ==================================
// Initialization / Global functions.
// ==================================
typedef void (*SAMPLELOADCALLBACK)(void *buff, int lenbytes, int numbits, int instno, int sampno);
typedef void (*FMUSIC_CALLBACK)(FMUSIC_MODULE *mod, unsigned char param);

// this must be called before FSOUND_Init!
void FSOUND_File_SetCallbacks(unsigned int	(*OpenCallback)(char *name),
                              void			(*CloseCallback)(unsigned int handle),
                              int			(*ReadCallback)(void *buffer, int size, unsigned int handle),
                              void			(*SeekCallback)(unsigned int handle, int pos, signed char mode),
                              int			(*TellCallback)(unsigned int handle)
							 );

void FSOUND_File_SetExtraCallbacks(	void (*userMix)(short*buffer, int size),
									void (*onVolumeEnd)(void),
									void (*onSongEnd)(void),
									void (*log)(const char * format, ...)
								  );


// =============================================================================================
// FMUSIC API
// =============================================================================================

// Song management / playback functions.
// =====================================

FMUSIC_MODULE * FMUSIC_LoadSong(char *data, SAMPLELOADCALLBACK sampleloadcallback);
signed char		FMUSIC_FreeSong(FMUSIC_MODULE *mod);
signed char		FMUSIC_PlaySong(FMUSIC_MODULE *mod);
signed char		FMUSIC_StopSong(FMUSIC_MODULE *mod);

// Runtime song information.
// =========================

int				FMUSIC_GetOrder(FMUSIC_MODULE *mod);
int				FMUSIC_GetRow(FMUSIC_MODULE *mod);
unsigned int	FMUSIC_GetTime(void);

// add by rov
int				FMUSIC_GetProgress(void); // return 0-100 song progress
void			FMUSIC_SetVolume(int volume); // set real volume
void			FMUSIC_SlideVolume(int way);
void			FMUSIC_fadeSound2Zero(int speed); // fade volume to 0, return when volume is null.
int				FMUSIC_updateVolume(int time); // launched by minifmod to fade volume to the wanted (internal, no call it yourself)
int				FMUSIC_setWantedVolume(int volume); // set value where real volume slide
void			FMUSIC_changeSong(FMUSIC_MODULE *newMod);
void			FMUSIC_setVolumeUpdateSpeed(int speed);
signed char		FMUSIC_FreeSongWithoutStop(FMUSIC_MODULE *mod);
void			FMUSIC_MemInit(void); // init minifmod with the mem io callback

extern int volumeUpdateSpeed ;

extern FMUSIC_MODULE *lastmodplay ;

extern void(*aditionalMix)(short*buffer, int size); // callback to user own mix
extern void(*onNoVolume)(void);	// callback, launched when real volume reatched 0
extern void(*onSoundEnd)(void);	// user callback launched when song is finish..
extern void(*printlg)(const char * format, ...); // log..

// mix buffer info...
extern short*	curentOutBf;
extern int		curentOutSz;
extern short*	curentMixBf;
extern int		curentMixSz;

int retreve(char what);

char * FMUSIC_GetPointerToOutputBuffer(void); // will return you ptr to curent mix buffer

// rov end

typedef struct
{
  int length;
  int pos;
  void *data;
}
MEMFILE;

#ifdef __cplusplus
}
#endif

#endif
