#ifndef _gdlfree_
#define _gdlfree_

#include "../Gdl.h"

void * allocAndAddFree(u32 allocSize,void **list)
{	void * data = malloc(allocSize);
	addFreeEntry(data,list);
	return data ;
}

void ** defaultFreeList = 0 ;

void freeAllEntry(void **list)
{	if(!list) list = defaultFreeList ;
	int	nbEntry = *(int*)list ;
    while(nbEntry) if(list[nbEntry]) free(list[nbEntry--]); else nbEntry--;
	free(list); list=0;
}

void ** createFreeList(u32 maxEntry)
{	void ** list = (void**)malloc(maxEntry*4);
	*(int*)list = 0 ; // set entry number to zero
	return list ;
}

void addFreeEntry(void * add, void**list) // if no list specified, use the default
{	static void **freeList = 0 ;
	int nbEntry ;
	if(!list)
	{ if(!defaultFreeList) defaultFreeList = createFreeList(1024); // 1024 entry max
	  list = defaultFreeList ;
	}

	nbEntry = *(int*)list ;
   if(add) // add an entry
   {	list[++nbEntry] = add ;
		*(int*)list = nbEntry ;
   }
}

#endif
