/* 音色データ群統括 */
#if (!defined(__TONEDATA_H__))
#define __TONEDATA_H__

#include "fmregdef.h"

#define TONENAMEMAX (64) // 音色名最大バイト

// 名前つき音色データ
typedef struct
{
	char toneName[TONENAMEMAX];
	toneData tone;
} toneDataWithName;

#define TONEDATAMAX 64 // データ最大数 文字列化使うので括弧でくくれず

extern void initToneData(const char* defFile); // 初期化時に指定したファイルがあれば読み込む

// 音色データ管理
extern toneDataWithName allToneData[TONEDATAMAX];

// 音色データ読み込み
extern int readAllToneData(void);
// 音色データ書き出し
extern int writeAllToneData(void);

#endif

