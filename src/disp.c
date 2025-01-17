#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <doslib.h>
#include "disp.h"
#include "keyutil.h"
#include "fmctrl.h"

#include "kbptn.h"
#include "makefnt.h"

static int oldCrtMode=0; // 実行前のCRTMode
static int oldTextMode=0; // 実行前のテキスト使用モード
static int oldGraphMode=0; // 実行前のグラフィック使用モード
static int oldFuncKeyMode=0; // 実行前のファンクションキーモード
static int oldScrollRange=0;
static toneData dispToneData={0}; // 表示用トーンデータ
static int dispOct=3; //表示用オクターブ
static int curX=0,curY=0; // カーソル表示位置

static char toneName[128]={0}; // 音色名

// 全部書き替えると遅いのでダーティーフラグで管理する
enum
{
	DIRTY_COMMON=0, // 共通パート
	DIRTY_OP1, // Operator 1
	DIRTY_OP2, // Operator 2
	DIRTY_OP3, // Operator 3
	DIRTY_OP4,  // Operator 4
	// ここまで表示Ｙに合わせること
	// 以下表示重い部分切り出して特殊処理
	DIRTY_TONENAME, // 音色表示
	DIRTY_ALGO, // アルゴリズム表示
	DIRTY_NUM
};

struct toneDispTable
{
	unsigned char* place;
	int min;
	int max;
};

// 表示位置と表示内容の変換テーブル
const struct toneDispTable toneIndexTbl[5][11]=
{
	// 共通項目
	{
		{&dispToneData.fbLevel,0,7},
		{&dispToneData.algorithm,0,7},
		{&dispToneData.opMask,0,15 },
		{&dispToneData.lfoWaveform, 0, 3},
		{&dispToneData.syncLFO, 0, 1},
		{&dispToneData.speedLFO, 0,255},
		{&dispToneData.pmd, 0,127},
		{&dispToneData.amd, 0,127},
		{&dispToneData.pms, 0, 7},
		{&dispToneData.ams, 0, 3},
		{&dispToneData.pan, 0, 3}
	},
	{
		{&(dispToneData.opData[0].totalLevel),0,127},
		{&(dispToneData.opData[0].decay1Level),0,15},
		{&(dispToneData.opData[0].attackRate), 0,31},
		{&(dispToneData.opData[0].decay1Rate), 0,31},
		{&(dispToneData.opData[0].decay2Rate), 0,15},
		{&(dispToneData.opData[0].releaseRate), 0,15},
		{&(dispToneData.opData[0].keyScale), 0, 3},
		{&(dispToneData.opData[0].freqMul), 0,15},
		{&(dispToneData.opData[0].detune1), 0, 7},
		{&(dispToneData.opData[0].detune2), 0, 3},
		{&(dispToneData.opData[0].amsEnable), 0, 1},
	},
	{
		{&(dispToneData.opData[1].totalLevel),0,127},
		{&(dispToneData.opData[1].decay1Level),0,15},
		{&(dispToneData.opData[1].attackRate), 0,31},
		{&(dispToneData.opData[1].decay1Rate), 0,31},
		{&(dispToneData.opData[1].decay2Rate), 0,15},
		{&(dispToneData.opData[1].releaseRate), 0,15},
		{&(dispToneData.opData[1].keyScale), 0, 3},
		{&(dispToneData.opData[1].freqMul), 0,15},
		{&(dispToneData.opData[1].detune1), 0, 7},
		{&(dispToneData.opData[1].detune2), 0, 3},
		{&(dispToneData.opData[1].amsEnable), 0, 1},
	},
	{
		{&(dispToneData.opData[2].totalLevel),0,127},
		{&(dispToneData.opData[2].decay1Level),0,15},
		{&(dispToneData.opData[2].attackRate), 0,31},
		{&(dispToneData.opData[2].decay1Rate), 0,31},
		{&(dispToneData.opData[2].decay2Rate), 0,15},
		{&(dispToneData.opData[2].releaseRate), 0,15},
		{&(dispToneData.opData[2].keyScale), 0, 3},
		{&(dispToneData.opData[2].freqMul), 0,15},
		{&(dispToneData.opData[2].detune1), 0, 7},
		{&(dispToneData.opData[2].detune2), 0, 3},
		{&(dispToneData.opData[2].amsEnable), 0, 1},
	},
	{
		{&(dispToneData.opData[3].totalLevel),0,127},
		{&(dispToneData.opData[3].decay1Level),0,15},
		{&(dispToneData.opData[3].attackRate), 0,31},
		{&(dispToneData.opData[3].decay1Rate), 0,31},
		{&(dispToneData.opData[3].decay2Rate), 0,15},
		{&(dispToneData.opData[3].releaseRate), 0,15},
		{&(dispToneData.opData[3].keyScale), 0, 3},
		{&(dispToneData.opData[3].freqMul), 0,15},
		{&(dispToneData.opData[3].detune1), 0, 7},
		{&(dispToneData.opData[3].detune2), 0, 3},
		{&(dispToneData.opData[3].amsEnable), 0, 1},
	},
};

static int toneDataDirty[DIRTY_NUM];

/* アルゴリズム表記（外字) */
/* 一度fmeditを実行するとフォントが書き変わるので見えるようになる */
static const char* algStr[8][3]=
{
	{
		/* alg 0 */
		"�滔��｡�｢",
		"        ",
		"        ",
	},
	{
		/* alg 1 */
		"�｣�､�･�ｦ",
		"�ｧ�ｨ�ｩ�ｪ",
		"        "
	},
	{
		/* alg 2 */
		"�ｫ�ｬ�ｭ�ｮ",
		"�ｯ�ｰ�ｱ�ｲ",
		"        "
	},
	{
		/* alg 3 */
		"�ｳ�ｴ�ｵ�ｶ",
		"�ｷ�ｸ�ｹ�ｺ",
		"        "
	},
	{
		/* alg 4 */
		"  �ｻ�ｼ  ",
		"  �ｽ�ｾ  ",
		"        "
	},
	{
		/* alg 5 */
		"  �ｿ�ﾀ  ",
		"  �ﾁ�ﾂ  ",
		"  �ﾃ�ﾄ  "
	},
	{
		/* alg 6 */
		"  �ﾅ�ﾆ  ",
		"  �ﾇ�ﾈ  ",
		"  �ﾉ�ﾊ  "
	},
	{
		/* alg 7 */
		"    �ﾋ  ",
		"    �ﾌ  ",
		"    �ﾍ  "
	}
};
	
const char panChar[4]={'*','L','R','C'};

// キーボードグラフィック表示バッファ(84*42)
static unsigned char kbBitmap[42][42];

// キーボードグラフィックパターンを生成する
static void createKbGraph(void)
{
	int y;
	unsigned char* basePt;

	for (y=0;y<42;++y)
	{
		int x;

		// 使うパターンを選ぶ
		if (y<21)
		{
			basePt=kbptn[0]; // 0-20 黒鍵盤付き(index 0)
		}
		else
		{
			if (y==21)
			{
				basePt=kbptn[1]; // 21 隙間(index 1)
			}
			else
			{
				basePt=kbptn[2]; // 22-41 白鍵のみ(index 2)
			}
		}
		for (x=0;x<84;++x)
		{
			kbBitmap[y][x]=(basePt[x]);
		}
	}
}

// 表示位置
const int dispToneNameY=2;
const int dispCommonY=4;
const int dispOpStartY=9;
const int dispOpHeight=4;
const int dispPromptY=0;
const int dispHelpX=45;
const int dispHelpY=26;
const int dispKbHelpX=12;
const int dispKbHelpY=26;
const int dispOctX=4;
const int dispOctY=29;

const int dispKbX=8; // kb pos x dot
const int dispKbY=400; // kb pos y dot

// テキスト色定義
const int toneNameCol=3+8;
const int commonHeaderCol=1+8;
const int opHeaderCol=1+8;
const int opTopCol=3+4;
const int normalParamCol=3;
const int selectParamCol=3+8;
const int inputPromptCol=3;
const int inputCol=3;

const char* fkeyName[10]=
{
	" 全LOAD ", // F1
	" 全SAVE ", // F2
	" ****** ", // F3
	"音色番号", // F4
	" 前音色 ", // F5
	" 次音色 ", // F6
	" 音色名 ", // F7
	"全EXPORT", // F8
	"単EXPORT", // F9
	" ****** ", // F10
};

static void dispHelp(void)
{
	B_COLOR(3);
	B_LOCATE(dispHelpX,dispHelpY);
	printf("Fullkey 1/2/3/4:Toggle OpMask");
	B_LOCATE(dispHelpX,dispHelpY+1);
	printf("Cursor:move cursor");
	B_LOCATE(dispHelpX,dispHelpY+2);
	printf("value: q:-10 w:-1 e:+1 r:+10");
	B_LOCATE(dispHelpX,dispHelpY+3);
	printf("O:copy to buffer P:copy from buffer");
	
	// 演奏部分ヘルプ
	B_LOCATE(dispKbHelpX,dispKbHelpY);
	printf("         F G   J K l");
	B_LOCATE(dispKbHelpX,dispKbHelpY+1);
	printf("Oct Z/X C V B N M , .");
	
}

/* 書き変わらない部分の表示 */
static void dispHeaders(void)
{
	// 共通部分
	B_COLOR(commonHeaderCol);
	B_LOCATE(0,dispToneNameY);
	printf("音色:");
	B_LOCATE(0,dispCommonY);
	printf("    FB:  ALGO:  MASK:  FORM:  SYNC:LFOSPD:   PMD:   AMD:   PMS:   AMS:   PAN:");

	B_COLOR(opHeaderCol);
	// オペレーターヘッダ
	{
		int op;

		for (op=0;op<4;++op)
		{
			B_LOCATE(0,dispOpStartY+op*dispOpHeight+1);
			printf("    TL:   D1L:    AR:   D1R:   D2R:    RR:    KS:   MUL:   DT1:   DT2: AMSEN:");
		}
	}
	// 一番下のファンクションキー表示
	{
		int i;

		B_COLOR(3+8);
		for (i=0;i<10;++i)
		{
			B_LOCATE(i*9+(i>4)*2+2,31);
			printf(fkeyName[i]);
		}
	}

	dispHelp();
}

/* オペレーターマスク表示 */
static void dispOpMaskHdr()
{
	int op;
	for (op=0;op<4;++op)
	{
		int dispY=dispOpStartY+op*dispOpHeight;
		// オペレータON/OFF情報表示
		B_COLOR(opTopCol);
		B_LOCATE(0,dispY);
		printf("OP%2d %s",op+1,(((dispToneData.opMask)&(1<<op))) ? "ON ":"OFF");
	}
}

/* 音色名書き替え */
static void dispToneName(void)
{
	B_LOCATE(6,dispToneNameY);
	B_COLOR(toneNameCol);
	printf("%s",toneName);
	B_COLOR(normalParamCol);
	B_ERA_ED();
}

// アルゴリズムヘルプ表示（表示量が多いので切り分け）
static void dispAlgorithm()
{
	int i;
	
	/* アルゴリズムヘルプ表示 */
	B_COLOR(normalParamCol);
	for (i=0;i<3;++i)
	{
		B_LOCATE(7,dispCommonY+2+i);
		printf("%s",algStr[dispToneData.algorithm][i]);
	}
}

/* 共通データ表示 */
static void dispCommonData(void)
{
	int x;

	for (x=0;x<11;++x)
	{
		const unsigned char* dispData=toneIndexTbl[0][x].place;

		if ((curX==x)&&(curY==0))
		{
			B_COLOR(selectParamCol);
		}
		else
		{
			B_COLOR(normalParamCol);
		}
		B_LOCATE(7*x,dispCommonY+1);

		if (dispData==&(dispToneData.opMask))
		{
			// オペレータマスクは例外
			int i;
			printf("   ");
			
			for (i=0;i<4;++i)
			{
				printf(((*dispData)&(1<<i)) ? "1" : "0");
			}
			dispOpMaskHdr();
		}
		else
		{
			if (dispData==&(dispToneData.pan))
			{
				printf("   %c   ",panChar[*dispData]);
			}
			else
			{
				printf("%7d",*dispData);
			}
		}
	}
}


/* 各オペレータデータ表示 */
static void dispOperatorData(int op)
{
	int dispY=dispOpStartY+op*dispOpHeight;
	int x;

	for (x=0;x<11;++x)
	{
		if ((curX==x)&&(curY==op+1))
		{
			B_COLOR(selectParamCol);
		}
		else
		{
			B_COLOR(normalParamCol);
		}
		B_LOCATE(7*x,dispY+2);
		printf("%7d",*(toneIndexTbl[op+1][x].place));
	}
}

#define COL(r,g,b,i) ((g<<11)|(r<<6)|(b<<1)|(i))

#define colBlack COL(0,0,0,0)
#define normWhite COL(31,31,31,0)
#define pushWhite COL(31,31,4,1)
#define normBlack COL(15,15,15,0)
#define pushBlack COL(18,18,2,1)

const unsigned short normPalet[16]=
{
	colBlack,
	normWhite,normBlack,normWhite,normBlack,normWhite, // C-C#-D-D#-E
	normWhite,normBlack,normWhite,normBlack,normWhite,normBlack,normWhite, // F-F#-G-G#-A-A#-B
	normWhite,normWhite,normWhite // dummy
};

const unsigned short pushPalet[16]=
{
	colBlack,
	pushWhite,pushBlack,pushWhite,pushBlack,pushWhite, // C-C#-D-D#-E
	pushWhite,pushBlack,pushWhite,pushBlack,pushWhite,pushBlack,pushWhite, // F-F#-G-G#-A-A#-B
	normWhite,normWhite,normWhite // dummy
};

void initDisp(const int oct)
{
	B_CUROFF();
	oldScrollRange=B_CONSOL(0,0,-1,-1);
	oldFuncKeyMode=C_FNKMOD(-1);
	oldCrtMode=CRTMOD(-1);
	CRTMOD(16); // 768*512に切り替える
	oldTextMode=TGUSEMD(1,2);
	oldGraphMode=TGUSEMD(0,2);
	G_CLR_ON();
	{
		int i;
		for (i=0;i<16;++i)
		{
			GPALET(i,normPalet[i]);
		}
	}

	generateFont();
	
	strcpy(toneName,"init");
	dispPlayOctave(oct);
	dispHeaders();
	{
		struct PUTPTR p;
		p.x1=dispKbX;
		p.y1=dispKbY;
		p.x2=dispKbX+83;
		p.y2=dispKbY+41;
		p.buf_start=(unsigned char*)(kbBitmap);
		p.buf_end=(unsigned char*)(kbBitmap+84*42);
		createKbGraph();
		PUTGRM(&p);
	}

	// 最初は全部描画
	{
		int i;

		for (i=0;i<DIRTY_NUM;++i)
		{
			toneDataDirty[i]=1;
		}
	}
}

void refreshAllScreen(void)
{
	// テキストクリア
	B_CLR_AL();
	// ヘッダも書き直す
	dispHeaders();
	// 全部描画
	{
		int i;

		for (i=0;i<DIRTY_NUM;++i)
		{
			toneDataDirty[i]=1;
		}
	}
}

void updateDisp(void)
{
	int op;
	
	if (toneDataDirty[DIRTY_COMMON])
	{
		toneDataDirty[DIRTY_COMMON]=0;
		dispCommonData();
	}
	for (op=0;op<4;++op)
	{
		if (toneDataDirty[op+DIRTY_OP1])
		{
			toneDataDirty[op+DIRTY_OP1]=0;
			dispOperatorData(op);
		}
	}
	if (toneDataDirty[DIRTY_TONENAME])
	{
		toneDataDirty[DIRTY_TONENAME]=0;
		dispToneName();
	}
	if (toneDataDirty[DIRTY_ALGO])
	{
		toneDataDirty[DIRTY_ALGO]=0;
		dispAlgorithm();
	}
	B_CUROFF();
}

void finishDisp(void)
{
	TGUSEMD(0,oldGraphMode);
	TGUSEMD(1,oldTextMode);
	CRTMOD(oldCrtMode);
	C_FNKMOD(oldFuncKeyMode);
	B_CONSOL(0,0,oldScrollRange>>16,oldScrollRange&(0xffff));
	B_CURON();
}

void setDispToneName(const char* name)
{
	strncpy(toneName,name,127);
	toneName[127]=0;
	toneDataDirty[DIRTY_TONENAME]=1;
}

/* どちらを使うか決めかねているので両方用意 */
void setDispToneData(const toneData* src)
{
	int i;
	for (i=0;i<DIRTY_NUM;++i)
	{
		toneDataDirty[i]=1;
	}
	dispToneData=*src;
}

void setDispOpmData(const opmParam* src)
{
	toneData tmpToneData;
	opmParamToToneData(&tmpToneData,src);
	setDispToneData(&tmpToneData);
}

void getDispToneData(toneData* dst)
{
	*dst=dispToneData;
}

void getDispOpmParam(opmParam* dst)
{
	opmParam tmp;
	toneDataToOpmParam(&tmp,&dispToneData);
	*dst=tmp;
}

void changeDispToneData(const int curX,const int curY,const int offset)
{
	unsigned char* tgt=toneIndexTbl[curY][curX].place;
	const int minVal=toneIndexTbl[curY][curX].min;
	const int maxVal=toneIndexTbl[curY][curX].max;
	int val=offset+(*tgt);

	if (val<minVal)
	{
		val=minVal;
	}
	if (val>maxVal)
	{
		val=maxVal;
	}
	if ((*tgt)!=(unsigned char)(val))
	{
		toneDataDirty[curY]=1;
		*tgt=(unsigned char)(val);
		if (tgt==&dispToneData.algorithm)
		{
			toneDataDirty[DIRTY_ALGO]=1; // アルゴリズム書き変わった
		}
	}
}

void changeDispOpMask(const int op)
{
	dispToneData.opMask^=(1<<op);
	toneDataDirty[DIRTY_COMMON]=1;
}

void dispNoteOn(const int note)
{
	assert((note>=0)&&(note<=11));
	GPALET(note+1,pushPalet[note+1]);
}

void setCursor(const int x,const int y)
{
	if ((x!=curX)||(y!=curY))
	{
		// 移動前カーソル再描画対象
		toneDataDirty[curY]=1;
		curX=x;curY=y;
		// 移動後カーソル再描画対象
		toneDataDirty[curY]=1;
	}
}

void dispNoteOff(const int note)
{
	assert((note>=0)&&(note<=11));
	GPALET(note+1,normPalet[note+1]);
}

void dispAllNoteOff()
{
	int note;

	for (note=0;note<12;++note)
	{
		GPALET(note+1,normPalet[note+1]);
	}
}

void dispPlayOctave(const int oct)
{
	dispOct=oct;
	B_COLOR(3);B_LOCATE(dispOctX,dispOctY);
	printf("%d/7",dispOct);
}

/* 入力共通前処理 */
static void preInput(const char* prompt)
{
	B_LOCATE(0,dispPromptY);
	B_COLOR(inputPromptCol);
	printf("%s",prompt);
	B_COLOR(inputCol);
	B_ERA_ED();
	waitKeyClear();
}

/* 入力共通後処理 */
static void postInput(void)
{
	B_LOCATE(0,dispPromptY);
	B_ERA_ED();	
}

void showPrompt(const char* prompt)
{
	preInput(prompt);
}

void hidePrompt(void)
{
	postInput();
}

/* プロンプト付き文字列入力 */
void inputString(char* ans,const int len,const char* prompt)
{
	char* inpBuf=NULL;

	inpBuf=malloc(sizeof(len+2));
	if (inpBuf)
	{
		B_CURON();
		preInput(prompt);

		if (fgets(inpBuf,len,stdin))
		{
			if (inpBuf[strlen(inpBuf)-1]=='\n')
			{
				inpBuf[strlen(inpBuf)-1]=0; // 改行文字消去
			}
			strncpy(ans,inpBuf,len);
		}
		else
		{
			strcpy(ans,"");
		}
	
		B_CUROFF();
		postInput();

		free(inpBuf);
	}
	else
	{
		strcpy(ans,"");
	}
}
/* プロンプトつき一文字入力 */
char inputChar(const char* prompt)
{
	char ret=0;
	preInput(prompt);
	ret=INKEY();
	postInput();
	return ret;
}

