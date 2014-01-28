
#ifndef _gdlConfig_
#define _gdlConfig_

	/************** configuration ***/
   
		/* used framebuffer .. default is tinyPtc under  win32 and rlyeh's one under gp2x */
			//#define useSdl
    
		/* power used by app */
			//#define startAppWithFullPower
			#define sleepWhenNoFocus

		/* do you need rar2 support ? 6ko */
			#define useUnrarlib
		
		/* sound lib to use		minifmod 8ko */
			#define useMiniFmod
			//#definee useBass
		
		/* do you plain to use sfx ? */
			#define iWantSfx
			#define useModSfx
			//#define useS3mSfx

		/* do you need sin/cos table ? 4ko */
			//#define includeMathTable

		/* do you need load external picture support ? */
			//#define useTga
			//#define usePcx

		/* do you need gfm creation at runtime ? */
			//#define enableSetGfm

		/* force 32b deep in sdl ? */
			//#define use32bIfPossible

		/* show error message ? */
			//#define showMeError

	/************* configuration end ***/

	#include "./ini.h"

#endif
