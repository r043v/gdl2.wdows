
#include "stdio.h"
#include "string.h"
#include "minifmod.h"

unsigned int memopen(char *name) { return((unsigned int)name); }

void memclose(unsigned int handle){}

int memread(void *buffer, int size, unsigned int handle)
{ MEMFILE *memfile = (MEMFILE *)handle;

  if (memfile->pos + size >= memfile->length)
    size = memfile->length - memfile->pos;

  memcpy(buffer, (char *)memfile->data+memfile->pos, size);
  memfile->pos += size;

  return(size);
}

void memseek(unsigned int handle, int pos, signed char mode)
{ MEMFILE *memfile = (MEMFILE *)handle;

  if (mode == SEEK_SET)
    memfile->pos = pos;
  else if (mode == SEEK_CUR)
    memfile->pos += pos;
  else if (mode == SEEK_END)
    memfile->pos = memfile->length + pos;

  if (memfile->pos > memfile->length)
    memfile->pos = memfile->length;
}

int memtell(unsigned int handle)
{ MEMFILE *memfile = (MEMFILE *)handle;
  return(memfile->pos);
}

void FMUSIC_MemInit(void)
{  FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
}