#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "tonedata.h"
#include "disp.h"
#include "diskio.h"

// 指定範囲の音色をエキスポートする
static void FileExportRange(const char* topMsg,const int st,const int ed)
{
	static enum
	{
		CONV_NONE=0,
		CONV_ZMS,
		CONV_CHDR,
		CONV_BAS
	} convType=CONV_NONE;

	int total=ed-st+1;

	assert (st>=0);
	assert (ed>=st);
	assert (TONEDATAMAX>ed);

	
	// 最初にエキスポート種別を選ぶ
	{
		char buf[256];
		char t;

		sprintf(buf,"%s種別 1:ZMS 2:Cヘッダ 3:BASIC other:exit",topMsg);

		t=inputChar(buf);

		switch(t)
		{
			case '1':
				convType=CONV_ZMS;
				break;
			case '2':
				convType=CONV_CHDR;
				break;
			case '3':
				convType=CONV_BAS;
				break;
			default:
				break;
		}
	}

	if (convType==CONV_NONE)
	{
		return;
	}

	{
		char fname[128];
		// エキスポートファイル名入力
		inputString(fname,128,"出力ファイル名(空==終了)");
		if (strlen(fname)==0)
		{
			return;
		}
		// オーバーライト確認
		if (fexists(fname))
		{
			char ow=inputChar("ファイルが存在します。上書きしますか？(y/Other)");
			if ((ow!='y')&&(ow!='Y'))
			{
				return;
			}
		}
		{
			int i;
			char prbuf[128];
			
			FILE* f=fopen(fname,"wt");
			// 先頭部分 (BASICは1音色ごと別配列にしないといけない)
			switch (convType)
			{
				case CONV_ZMS: // do nothing
					break;
				case CONV_CHDR:
					fprintf(f,"const char toneData[%d][5][11]={\n",total);
					break;
				case CONV_BAS:
					// do nothing
					break;
				default:
					break;
			}
			
			// 書き出し処理
			for (i=st;i<=ed;++i)
			{
				sprintf(prbuf,"出力中(%d/%d)",i-st+1,total);
				showPrompt(prbuf);
				switch (convType)
				{
					case CONV_ZMS:
						exportToneDataAsZMS(f,&(allToneData[i]),i-st+1);
						break;
					case CONV_CHDR:
						exportToneDataAsCHdr(f,&(allToneData[i]));
						break;
					case CONV_BAS:
						exportToneDataAsBas(f,&(allToneData[i]),i-st+1);
					default:
						break;
				}
			}
			// 終端
			switch (convType)
			{
				case CONV_ZMS: // do nothing
					break;
				case CONV_CHDR:
					fprintf(f,"};\n");
					break;
				case CONV_BAS:
					fprintf(f,"%c",0x1a); // BASIC終端
				default:
					break;
			}
			
			fclose(f);
			hidePrompt();
		}
	}
}

void FileExportAll(void)
{
	FileExportRange("全音色エキスポート",0,TONEDATAMAX-1);
}

void FileExportSingle(const int voiceNo) // 選択した音色だけエクスポート
{
	FileExportRange("現在の音色エキスポート",voiceNo,voiceNo);
}
