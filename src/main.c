#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iocslib.h>
#include <doslib.h>

#include "fmctrl.h"
#include "keyutil.h"
#include "tonedata.h"
#include "disp.h"
#include "diskio.h"
#include "fexport.h"

// #define DISABLE_VSYNC

static int currentToneIndex=0;
static toneData* currentTone=&(allToneData[0].tone);

static void setTone()
{
	const char* name=allToneData[currentToneIndex].toneName;

	currentTone=&(allToneData[currentToneIndex].tone);
	setToneData(currentTone);
	setDispToneData(currentTone);
	setDispToneName(name);
}

typedef struct
{
	int playing; /* 演奏中か？ */
	int oct; /* 鳴らしているオクターブ */
	int note; /* 鳴らしているノート */
} playNoteData;

static playNoteData playNoteList[8]; /* 各チャンネル演奏情報 */
static int currentOct=3; /* 現在指定されているオクターブ */
static int curX=0,curY=0; // 現在のカーソル位置

/* 指定されたオクターブとノートで既に発音中か？ */
/* @return 発音中：使用しているチャンネル 発音していない:-1 */
inline static int isPlay(int oct,int note)
{
	int i;
	playNoteData* p=playNoteList;
	
	for (i=0;i<8;++i,p++)
	{
		if ((p->playing)&&(p->oct==oct)&&(p->note==note))
		{
			return i;
		}
	}
	return -1; /* not found */
}

/* 使っていないチャンネルを見つける */
/* @return 見つかった：チャンネルインデックス 見つからない:-1 */
inline static int searchUnusedCh(void)
{
	int i;
	playNoteData* p=playNoteList;

	for (i=0;i<8;++i,p++)
	{
		if ((p->playing)==0)
		{
			return i;
		}
	}

	return -1; /* not found */
}

/* 指定チャンネルを使って指定した音の高さで鳴らす */
inline static void playNote(int ch,int oct,int note)
{
	assert((ch>=0)&&(ch<=7));
	assert(playNoteList[ch].playing==0);

	playNoteList[ch].playing=1;
	playNoteList[ch].oct=oct;
	playNoteList[ch].note=note;

	keyOn(ch,oct,note,currentTone->opMask);
}

/* 指定チャンネルの音を止める */
inline static void stopNote(int ch)
{
	assert((ch>=0)&&(ch<=7));
	assert(playNoteList[ch].playing!=0);

	playNoteList[ch].playing=0;
	keyOff(ch);
}

/* すべてのチャンネルを停止 */
static void stopAllNotes()
{
	int ch;

	for (ch=0;ch<8;++ch)
	{
		playNoteList[ch].playing=0; /* パニックモードでの使用も考えて演奏中チェックなし */
		keyOff(ch);
	}
}

static void updateValueKey()
{
	int changeValue=0;
	if (keyTrigOn[KEY_VALUE_UP_1])
	{
		changeValue+=1;
	}
	if (keyTrigOn[KEY_VALUE_UP_10])
	{
		changeValue+=10;
	}
	if (keyTrigOn[KEY_VALUE_DOWN_1])
	{
		changeValue-=1;
	}
	if (keyTrigOn[KEY_VALUE_DOWN_10])
	{
		changeValue-=10;
	}
	if (changeValue!=0)
	{
		changeDispToneData(curX,curY,changeValue);
		getDispToneData(currentTone);
		setToneData(currentTone);
	}
}

/* 演奏キーボード周りの処理 */
static void updatePlayKB()
{
	/* オクターブ変更チェック */
	/* 音高補正のため全音下げているので安全のためオクターブは1-7 */
	if (keyTrigOn[KEY_OCT_DOWN])
	{
		if (currentOct>1)
		{
			currentOct--;
			dispPlayOctave(currentOct);
		}
	}
	if (keyTrigOn[KEY_OCT_UP])
	{
		if (currentOct<7)
		{
			currentOct++;
			dispPlayOctave(currentOct);
		}
	}
	{
		/* NoteOn/Offチェック */
		enum KeyType checkKey;

		for (checkKey=KEY_PLAY_C;checkKey<=KEY_PLAY_B;++checkKey)
		{
			int checkNote=checkKey-KEY_PLAY_C;
			if (keyTrigOn[checkKey])
			{
				/* note on */
				int index=searchUnusedCh();
				if (index>=0)
				{
					dispNoteOn(checkNote);
					playNote(index,currentOct,checkNote);
				}
			}
			if (keyTrigOff[checkKey])
			{
				/* note off */
				int index=isPlay(currentOct,checkNote);
				if (index>=0)
				{
					dispNoteOff(checkNote);
					stopNote(index);
				}
			}
		}
	}
}

static volatile int vSyncCount=0;
static interrupt void vSyncInt(void)
{
	vSyncCount++;
}

// カーソルキー処理
static void updateCursorKey()
{
	if ((keyTrigOn[KEY_CUR_UP])&&(curY>0))
	{
		curY--;
		setCursor(curX,curY);
	}
	if ((keyTrigOn[KEY_CUR_DOWN])&&(curY<4))
	{
		curY++;
		setCursor(curX,curY);
	}
	if ((keyTrigOn[KEY_CUR_LEFT])&&(curX>0))
	{
		curX--;
		setCursor(curX,curY);
	}
	if ((keyTrigOn[KEY_CUR_RIGHT])&&(curX<10))
	{
		curX++;
		setCursor(curX,curY);
	}
}

#define STR(s) #s
#define TONECHGMSG(s) "音色番号(1-" STR(s) ")"

// 音色切り替え関係
static void updateToneChangeKey(void)
{
	if (keyTrigOn[KEY_CHANGETONE])
	{
		char tmp[16];
		int newTone;

		inputString(tmp,16,TONECHGMSG(TONEDATAMAX));
		newTone=strtol(tmp,NULL,10);
		if ((newTone>0)&&(newTone<=TONEDATAMAX))
		{
			currentToneIndex=newTone-1;
			setTone();
		}
	}
	if (keyTrigOn[KEY_NEXTTONE])
	{
		if (currentToneIndex<TONEDATAMAX-1)
		{
			currentToneIndex++;
			setTone();
		}
	}
	if (keyTrigOn[KEY_PREVTONE])
	{
		if (currentToneIndex>0)
		{
			currentToneIndex--;
			setTone();
		}
	}
	if (keyTrigOff[KEY_SETNAME])
	{
		char tmp[TONENAMEMAX];

		inputString(tmp,TONENAMEMAX,"新しい音色名:");
		if (strlen(tmp)>0)
		{
			// 音色名変更
			strncpy(allToneData[currentToneIndex].toneName,tmp,TONENAMEMAX);
			setDispToneName(tmp);
		}
	}
}

static void updateOpMaskKey(void)
{
	int op;
	int dirty=0;
	
	for (op=0;op<4;++op)
	{
		if (keyTrigOn[KEY_OPMASK_1+op])
		{
			changeDispOpMask(op);
			dirty=1;
		}
	}
	if (dirty)
	{
		getDispToneData(currentTone);
		setTone();
	}
}

static void updateFileKey(void)
{
	if (keyTrigOn[KEY_ALLSAVE])
	{
		writeAllToneData();
	}
	if (keyTrigOn[KEY_ALLLOAD])
	{
		readAllToneData();
		setTone(); // 画面書き直し
	}
	if (keyTrigOn[KEY_ALLEXPORT])
	{
		FileExportAll();
	}
	if (keyTrigOn[KEY_SINEXPORT])
	{
		FileExportSingle(currentToneIndex);
	}
}

int breakCkFlg=0;

int main(void)
{
	int loopExit=0;
	
	breakCkFlg=BREAKCK(-1);
	BREAKCK(2);// Ignore All BreakCheck
	
	/* 初期化 */
	initKeyBuf();
	initDisp(currentOct);
#if (!defined(DISABLE_VSYNC))
	VDISPST((unsigned char*)(vSyncInt),0,1);
#endif

	initToneData("default.ton");
	setTone();

	stopAllNotes();
	while (loopExit==0)
	{
		volatile int oldVSync;
		
		updateKeyBuf();
		updateCursorKey();
		updateToneChangeKey();
		updateOpMaskKey();
		updateValueKey();
		updateFileKey();
		updatePlayKB();

		// 終了処理
		if (keyTrigOn[KEY_EXIT])
		{
			char key=inputChar("終了します。よろしいですか？(Y/Other)");
			if ((key=='Y')||(key=='y'))
			{
				loopExit=1;
			}
		}

#if (!defined(DISABLE_VSYNC))
		oldVSync=vSyncCount;
		while (oldVSync==vSyncCount) {}; // VBLANK待機
#endif
		updateDisp();
	}

	/* 後始末 */
#if (!defined(DISABLE_VSYNC))
	VDISPST(0,0,0);
#endif
	stopAllNotes();
	finishDisp();
	waitKeyClear();
	
	BREAKCK(breakCkFlg);
	return 0;
}
