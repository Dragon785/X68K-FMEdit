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
	int playing; /* ���t�����H */
	int oct; /* �炵�Ă���I�N�^�[�u */
	int note; /* �炵�Ă���m�[�g */
} playNoteData;

static playNoteData playNoteList[8]; /* �e�`�����l�����t��� */
static int currentOct=3; /* ���ݎw�肳��Ă���I�N�^�[�u */
static int curX=0,curY=0; // ���݂̃J�[�\���ʒu

/* �w�肳�ꂽ�I�N�^�[�u�ƃm�[�g�Ŋ��ɔ��������H */
/* @return �������F�g�p���Ă���`�����l�� �������Ă��Ȃ�:-1 */
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

/* �g���Ă��Ȃ��`�����l���������� */
/* @return ���������F�`�����l���C���f�b�N�X ������Ȃ�:-1 */
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

/* �w��`�����l�����g���Ďw�肵�����̍����Ŗ炷 */
inline static void playNote(int ch,int oct,int note)
{
	assert((ch>=0)&&(ch<=7));
	assert(playNoteList[ch].playing==0);

	playNoteList[ch].playing=1;
	playNoteList[ch].oct=oct;
	playNoteList[ch].note=note;

	keyOn(ch,oct,note,currentTone->opMask);
}

/* �w��`�����l���̉����~�߂� */
inline static void stopNote(int ch)
{
	assert((ch>=0)&&(ch<=7));
	assert(playNoteList[ch].playing!=0);

	playNoteList[ch].playing=0;
	keyOff(ch);
}

/* ���ׂẴ`�����l�����~ */
static void stopAllNotes()
{
	int ch;

	for (ch=0;ch<8;++ch)
	{
		playNoteList[ch].playing=0; /* �p�j�b�N���[�h�ł̎g�p���l���ĉ��t���`�F�b�N�Ȃ� */
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

/* ���t�L�[�{�[�h����̏��� */
static void updatePlayKB()
{
	/* �I�N�^�[�u�ύX�`�F�b�N */
	/* �����␳�̂��ߑS�������Ă���̂ň��S�̂��߃I�N�^�[�u��1-7 */
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
		/* NoteOn/Off�`�F�b�N */
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

// �J�[�\���L�[����
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
#define TONECHGMSG(s) "���F�ԍ�(1-" STR(s) ")"

// ���F�؂�ւ��֌W
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

		inputString(tmp,TONENAMEMAX,"�V�������F��:");
		if (strlen(tmp)>0)
		{
			// ���F���ύX
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
		setTone(); // ��ʏ�������
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
	
	/* ������ */
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

		// �I������
		if (keyTrigOn[KEY_EXIT])
		{
			char key=inputChar("�I�����܂��B��낵���ł����H(Y/Other)");
			if ((key=='Y')||(key=='y'))
			{
				loopExit=1;
			}
		}

#if (!defined(DISABLE_VSYNC))
		oldVSync=vSyncCount;
		while (oldVSync==vSyncCount) {}; // VBLANK�ҋ@
#endif
		updateDisp();
	}

	/* ��n�� */
#if (!defined(DISABLE_VSYNC))
	VDISPST(0,0,0);
#endif
	stopAllNotes();
	finishDisp();
	waitKeyClear();
	
	BREAKCK(breakCkFlg);
	return 0;
}
