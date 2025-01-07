#include <iocslib.h>

#include "algofnt.h"
#include "makefnt.h"

const fontPicData algoFontData[8]=
{
	{0xeb9f,4,1}, // Algorithm 0
	{0xeba3,4,2}, // Algorithm 1
	{0xebab,4,2}, // Algorithm 2
	{0xebb3,4,2}, // Algorithm 3
	{0xebbb,2,2}, // Algorithm 4
	{0xebbf,2,3}, // Algorithm 5
	{0xebc5,2,3}, // Algorithm 6
	{0xebcb,1,3}  // Algorithm 7
};

static void makeFontSub(const fontPicData* p,const unsigned char src[][32])
{
	int x,y;
	const unsigned char* pSrc=src[0];
	unsigned int code=p->beginCode;
	
	for (y=0;y<(p->y);++y)
	{
		for (x=0;x<(p->x);++x)
		{
			DEFCHR(8,code,(UBYTE*)pSrc);
			
			pSrc+=32;code++;
		}
	}
}

void generateFont(void)
{
	makeFontSub(algoFontData+0,algo0);
	makeFontSub(algoFontData+1,algo1);
	makeFontSub(algoFontData+2,algo2);
	makeFontSub(algoFontData+3,algo3);
	makeFontSub(algoFontData+4,algo4);
	makeFontSub(algoFontData+5,algo5);
	makeFontSub(algoFontData+6,algo6);
	makeFontSub(algoFontData+7,algo7);
}

