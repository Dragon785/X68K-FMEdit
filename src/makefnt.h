#if (!defined(__MAKEFNT_H__))
#define __MAKEFNT_H__

// フォントデータの読み込みと書き込み
extern void generateFont(void);
typedef struct
{
	unsigned int beginCode;
	unsigned int x;
	unsigned int y;
} fontPicData;

extern const fontPicData algoFontData[8];

#endif

