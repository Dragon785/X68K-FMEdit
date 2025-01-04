#include "diskio.h"

int fexists(const char* filename) // ファイル名存在
{
	int ret=0;
	FILE* f;

	f=fopen(filename,"rb");
	if (f)
	{
		fclose(f);
		ret=1;
	}
	
	return ret;
}

void readToneData(FILE* f,toneDataWithName* dst)
{
	fread(dst,sizeof(toneDataWithName),1,f);
}

void writeToneData(FILE* f,const toneDataWithName* src)
{
	fwrite(src,sizeof(toneDataWithName),1,f);
}

/* エキスポートの共通部分 */
/* コメント文字など種別で違うところは呼出時に吸収 */
static void exportCommonArea(FILE* f,const toneDataWithName* src,
	const char* comPrefix,const char* comSuffix, // コメントプレフィクス/サフィックス
	const char* packPrefix,const char* packSuffix, // 配列内部の区切り文字 {} ()
	const char* contstr, // 最終行以外の追加文字 ,
	const char* laststr // 最終行の追加文字
	)
{
	int ope;
	const toneData* t=&(src->tone);
	
	// 全体定義
	// ヘッダ
	fprintf(f,"%s\t FA  OM  WF  SY  SP PMD AMD PMS AMS PAN%s\n",comPrefix,comSuffix);
	fprintf(f,  "\t%s%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,  0%s%s\n",
		packPrefix,
		(t->fbLevel<<3)|(t->algorithm), // FEEDBACK/ALGORITHM
		t->opMask, // OM
		t->lfoWaveform, // WF
		t->syncLFO, // SY
		t->speedLFO, // SP
		t->pmd, // PMD
		t->amd, // AMD
		t->pms, // PMS
		t->ams, // AMS
		t->pan, // PAN
		packSuffix,
		contstr
	);
	
	// オペレータ毎に
	// ヘッダ
	  fprintf(f,"%s\t AR  DR  SR  RR  SL  OL  KS  ML DT1 DT2 AME%s\n",comPrefix,comSuffix);
	for (ope=0;ope<4;++ope)
	{
		const opData* op=&(t->opData[ope]);
		fprintf(f,"\t%s%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d%s",
		packPrefix,
		op->attackRate, // AR
		op->decay1Rate, // DR(D1R)
		op->decay2Rate, // SR(D2R)
		op->releaseRate, // RR
		op->decay1Level, // SL(D1L)
		op->totalLevel, // OL(TL)
		op->keyScale, // KS
		op->freqMul, // ML
		op->detune1, // DT1
		op->detune2, // DT2
		op->amsEnable, // AME
		packSuffix
		);
		if (ope==3)
		{
			fprintf(f,"%s",laststr);
		}
		else
		{
			fprintf(f,"%s",contstr);
		}
		fprintf(f,"\n");
	}
}

// ZMS向けエキスポート（音色番号あり。１スタートっぽいので合わせる)
void exportToneDataAsZMS(FILE* f,const toneDataWithName* src,const int voiceNo)
{
	fprintf(f,"(v%d,0\n",voiceNo);
	exportCommonArea(f,src,"/","", // コメント開始は/で終了文字はなし
	"","", // 配列の前後にはなにもつかない
	"", // 途中にもなにもつかない
	")" // 最終行には)をつける
	);
}

// Cヘッダ向けエキスポート
void exportToneDataAsCHdr(FILE* f,const toneDataWithName* src)
{
	fprintf(f,"{");
	exportCommonArea(f,src,"/*","*/", // コメントは /* .. */ で括る
	"{","}", // 配列の前後は{}でくくる
	",", // 途中にはカンマ
	""); // 最終行にはなにもつかない
	fprintf(f,"},\n");
}

// BASIC向けエキスポート
// BASICは1音色ごとに別変数で書き出さないといけない
void exportToneDataAsBas(FILE* f,const toneDataWithName* src,const int voiceNo)
{
	fprintf(f,"dim char toneData%d(4,10)={\n",voiceNo);
	exportCommonArea(f,src,"/*","", // コメントは /* で始まる
	"+","", // 配列の先頭には+を追加、後ろにはなにも括らない
	",", // 途中にはカンマ
	"}"); // 最後}で閉じる
}

