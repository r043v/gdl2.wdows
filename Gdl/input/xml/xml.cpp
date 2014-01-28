
#include "xml.h"

u32	xml::loadXmlFile(char * path)
{	xmlFile = (char*)loadFile(path,&fileSize);
	wasloadfromDisc=1; offset=xmlFile;
	return !(xmlFile==0);
}

u32 xml::memLoadXml(char * xml, u32 fileSize)
{	xmlFile = xml; this->fileSize = fileSize ;
	wasloadfromDisc=0; offset=xmlFile;
	return !(xmlFile==0 || fileSize==0);
}

void xml::freeClass(void)
{	if(wasloadfromDisc && xmlFile) free(xmlFile);
	if(wordPath) free(wordPath);
}

xml::xml(char * fileOrPath,u32 memsize)
{	xml();
	wordPath = (char*)malloc(2048);
	workBf = &(wordPath[1024]);
	*wordPath = *workBf = 0 ;
	xmlFile = 0 ;
	if(isAPath(fileOrPath))	loadXmlFile(fileOrPath);
		else				memLoadXml(fileOrPath,memsize);
}

xml::xml(void)
{	wordPath = (char*)malloc(2048);
	workBf = &(wordPath[1024]);
	*wordPath = *workBf = 0 ;
	xmlFile = 0 ;
}

xml::~xml(void)
{	freeClass();
}

char * xml::getCurrentWordPath(void)
{	return wordPath ;
}

void xml::scanAllFile	(	void	(*onAWord)(char *word),
							void	(*onAnArgument)(char *arg,char*value)
						)
{	u32 type ;
	if(!xmlFile) return ;
	do{ type = jumpToNext();
		switch(type)
			{	case 0 : /*printlr("\n..eof ..");*/ break;
				case 1 ://	printlr("<%s>",workBf);
							if(onAWord) onAWord(workBf);	
				break;
				case 2 : u32 n = 0 ;
						 while(workBf[n++]);
						 char * p = &(workBf[n]);
						 //printlr("argument find : [%s][%s]",p,workBf);
						 if(onAnArgument) onAnArgument(p,workBf);
				break;
			};
	} while(type);
}

u32 xml::jumpToNext(void)
{	char * w = offset; u32 n=0;
	char * fileEnd = (xml::xmlFile) + (xml::fileSize) ;
	while(1)
		switch(*w)
		{	 // start of a balise
			case '<' :
				//printlr("start of balise.");
				switch(w[1])
				{	case '/' : // in fact it was a balise end
					case '\\':
						//printlr("in fact it's a end");
						// search if we skip a value..
						{	char * p = w-1 ;
							while(*p != '=' && *p != '>' && *p != ' ' && *p != '\n') p-- ;
							if(*p == '>') // we was skip this value !
							{	//printlr("we skip value");
								p++ ; u32 n=0;
								while(*p != '>') { workBf[n++]=*p++; workBf[n]=0; } w++; // set value
								p = &(w[2]); n++ ; char * z = &(workBf[n]);
								while(*p != '>') { workBf[n++]=*p++; workBf[n]=0; } w++; // and arg name
								offset=w;
								//printlr("arg : %s.%s",workBf,z);
								{	n = strlen(wordPath);
									char * p = wordPath + n ;
									while(p > wordPath && *p != '\\') p-- ; *p=0 ;
									return 2; // return find an argument !
								}
							}
						}
						{	n = strlen(wordPath);
							char * p = wordPath + n ;
							while(p > wordPath && *p != '\\') p-- ; *p=0 ; w++; offset=w;
						}
					break;

					default: // default: it's really a start, it's the balise name
						//printlr("it's a start");
						w++; n=0; 
						
						while(*w != ' ' && *w != '>')
							{	//printl("%c",*w);
								workBf[n] = *w;
								n++ ; w++ ;
								workBf[n]=0;
							};

						//printlr("");
						w++; offset=w;
						//printlr("name %s",workBf);
						//sprintf(wordPath,"%s\\%s",wordPath,workBf);
						return 1 ;
					break;
				};

			break;

			 // end of a balise
			case '/':
			case '\\':
				//printlr("end of balise");
				switch(w[1])
				{	case '>' : // last balise end
						{	w+=2; offset=w;
							n = strlen(wordPath);
							char * p = wordPath + n ;
							while(p > wordPath && *p != '\\') p-- ; *p=0 ;
						}
					break;
					default:
						while(*w++ != '>'); offset=w;
						{	n = strlen(wordPath);
							char * p = wordPath + n ;
							while(p > wordPath && *p != '\\') p-- ; *p=0 ;
						}
					break;
				};
			break;
			
			 // value of an argument
			case '=' :
			//	printlr("\nargument find");
				{	char * p = w+1 ; n=0;
					while(*p != '>' && *p != ' ' && *p != '/' && *p != '\\') { workBf[n++]=*p++; workBf[n]=0; }
					offset = p ; w = p ;
					while(*--p != '='); while(*--p != ' '); p++; n++; char * nm = &(workBf[n]);
					while(*p != '=') { workBf[n++]=*p++; workBf[n]=0; }
					//printlr("  name %s value %s",nm,workBf);
					return 2; // return find an argument !
				}
			break;
			 // space or text is skipped
			default : offset = w ; /*printl("%c",*w);*/ w++; if(offset>=fileEnd) return 0; break;
		}
}

