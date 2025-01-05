#include <stdio.h>
#include <string.h>
#include "tonedata.h"
#include "disp.h"
#include "diskio.h"

toneDataWithName allToneData[TONEDATAMAX];

static void readAllDataSub(const char* filename)
{
	int i;
	char buf[128];
	
	FILE* f=fopen(filename,"rb");
	
	for (i=0;i<TONEDATAMAX;++i)
	{
		sprintf(buf,"一括ロード中 %d/%d",i+1,TONEDATAMAX);
		showPrompt(buf);
		readToneData(f,&(allToneData[i]));
	}
	fclose(f);
	hidePrompt();
}

void initToneData(const char* defFile)
{
	if (fexists(defFile))
	{
		readAllDataSub(defFile);
	}
	else
	{
		// 仮で名前だけつけて全部0にする
		int i;

		showPrompt("音色初期化中...");
		
		for (i=0;i<TONEDATAMAX;++i)
		{
			int op;
			
			toneDataWithName* p=allToneData+i;
			sprintf(p->toneName,"音色#%d",i+1);
			memset(&(p->tone),0,sizeof(toneData));
			// 最低限音が出るようにパラメーター修正
			p->tone.algorithm=7;
			p->tone.opMask=15;
			p->tone.pan=3;

			for (op=0;op<4;++op)
			{
				p->tone.opData[op].attackRate=31;
				p->tone.opData[op].releaseRate=15;
				p->tone.opData[op].freqMul=1;
			}
		}
		hidePrompt();
	}
}

// 音色データ読み込み
int readAllToneData(void)
{
	char tmp[64];

	inputString(tmp,64,"ロードファイル名:");
	if (strlen(tmp)>0)
	{
		
		if (fexists(tmp)==0)
		{
			(void)inputChar("指定ファイルが存在しません。(Any Key)");
			return 1; // fail
		}
		readAllDataSub(tmp);
	}
	
	return 0; // success
}

// 音色データ書き出し
int writeAllToneData(void)
{
	char tmp[64];
	
	inputString(tmp,32,"セーブファイル名:");
	if (strlen(tmp)>0)
	{
		int i;
		FILE* f;

		// ファイル存在チェック
		if (fexists(tmp))
		{
			char ans;
			
			ans=inputChar("ファイルが存在します。上書きしますか？(Y/Other)");
			if ((ans!='y')&&(ans!='Y'))
			{
				return 1; // 中断
			}
		}
		f=fopen(tmp,"wb");
		for (i=0;i<TONEDATAMAX;++i)
		{
			writeToneData(f,&(allToneData[i]));
		}
		fclose(f);
	}

	return 0; // success
}

