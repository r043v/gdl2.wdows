
#ifndef _xml_
#define _xml_

#include "../../Gdl.h"

class xml
{	private:
	 char *  xmlFile;
	  u32	 fileSize;
	  u32	 wasloadfromDisc;
	 char *  wordPath, *workBf;
	 char *	 offset ;
	 u32	 jumpToNext(void);

	public:
	 u32	memLoadXml(char * xml, u32 fileSize);
	 u32	loadXmlFile(char * path);

	 void	scanAllFile	(	void	(*onAWord)(char *word)=0,
					        void	(*onAnArgument)(char *arg,char*value)=0
            			);

	 char * getCurrentWordPath(void);

	 void	freeClass(void);

	  xml(void);
	  xml(char * fileOrPath, u32 memSize=0);
	 ~xml(void);
};

#endif
