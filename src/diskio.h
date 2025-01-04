// ファイルへの入出力関係
#if (!defined(__DISKIO_H__))
#define __DISKIO_H

#include <stdio.h>
#include "tonedata.h"

extern int fexists(const char* filename);
// ファイルクローズなどは上位で面倒みる
extern void readToneData(FILE* f,toneDataWithName* dst);
extern void writeToneData(FILE* f,const toneDataWithName* src);

// ZMS向け書き出し
extern void exportToneDataAsZMS(FILE* f,const toneDataWithName* src,const int voiceNo);
// Cヘッダ書き出し
extern void exportToneDataAsCHdr(FILE* f,const toneDataWithName* src);
// BASIC書き出し。最後のデータだけ一番最後に,がつかない
extern void exportToneDataAsBas(FILE* f,const toneDataWithName* src,const int voiceNo);

#endif

