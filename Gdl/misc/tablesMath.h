#ifndef	_TINY_TOOLS_TABLE_GENERATOR_H_
#define	_TINY_TOOLS_TABLE_GENERATOR_H_


/*
	--------------------------------------------------------------
	- Fichier auto généré par Tiny Tools : Math Tables Generator -
	--------------------------------------------------------------

	Fichier créé le :Jun 17 2003 à 15:18:10
*/


#define	PRECISION_UNIT		0x00010000
#define	PRECISION_SHIFT		16
#define	PRECISION_FACTOR	65536
#define	NB_TABLE_ELEMENT	512
#define	COS(a) tableCos[(int)a & 0x1FF]
#define	SIN(a) tableCos[((int)a + 128) & 0x1FF]
#define	TAN(a) tableTan[(int)a & 0x1FF]
extern	long	tableCos[512];
extern	long	tableTan[512];

#endif

