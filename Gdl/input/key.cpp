
#ifndef _gdlkey_
#define _gdlkey_

#include "../Gdl.h"

u8 *keyArray = 0 ;
u8 *lastKey = 0 ;

void inikey(void)
{	if(!keyArray)
		{	keyArray = (u8*)malloc(keyArraySize*2);
			lastKey = keyArray+keyArraySize;
		}
	memset(keyArray,0,512);
}

u32 keyUp(u32 key)
{	if(key>255) { memset(lastKey,0xff,256); return 0; }
	u32 up=0;
	up = (keyArray[key] && lastKey[key]==0);
	lastKey[key] = keyArray[key] ;
	return up;
}

u32 keyPush(u32 key)
{	if(key>255) { memset(lastKey,0xff,256); return 0; }
	return keyArray[key];
}

#endif
