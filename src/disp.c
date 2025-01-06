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

static int oldCrtMode=0; // ���s�O��CRTMode
static int oldTextMode=0;
static int oldGraphMode=0;
static toneData dispToneData; // �\���p�g�[���f�[�^
static int dispOct=3; //�\���p�I�N�^�[�u
static int curX=0,curY=0; // �J�[�\���\���ʒu

static char toneName[128]={0}; // ���F��

// �S�������ւ���ƒx���̂Ń_�[�e�B�[�t���O�ŊǗ�����
enum
{
	DIRTY_COMMON=0, // ���ʃp�[�g
	DIRTY_OP1, // Operator 1
	DIRTY_OP2, // Operator 2
	DIRTY_OP3, // Operator 3
	DIRTY_OP4,  // Operator 4
	// �����܂ŕ\���x�ɍ��킹�邱��
	// �ȉ��\���d�������؂�o���ē��ꏈ��
	DIRTY_TONENAME, // ���F�\��
	DIRTY_ALGO, // �A���S���Y���\��
	DIRTY_NUM
};

struct toneDispTable
{
	unsigned char* place;
	int min;
	int max;
};

// �\���ʒu�ƕ\�����e�̕ϊ��e�[�u��
const struct toneDispTable toneIndexTbl[5][11]=
{
	// ���ʍ���
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

/* �A���S���Y���\�L�i�O��) */
/* ��xfmedit�����s����ƃt�H���g�������ς��̂Ō�����悤�ɂȂ� */
static const char* algStr[8][3]=
{
	{
		/* alg 0 */
		"����",
		"        ",
		"        ",
	},
	{
		/* alg 1 */
		"����",
		"����",
		"        "
	},
	{
		/* alg 2 */
		"����",
		"����",
		"        "
	},
	{
		/* alg 3 */
		"����",
		"����",
		"        "
	},
	{
		/* alg 4 */
		"  ��  ",
		"  ��  ",
		"        "
	},
	{
		/* alg 5 */
		"  ���  ",
		"  ����  ",
		"  ����  "
	},
	{
		/* alg 6 */
		"  ����  ",
		"  ����  ",
		"  ����  "
	},
	{
		/* alg 7 */
		"    ��  ",
		"    ��  ",
		"    ��  "
	}
};
	
const char panChar[4]={'*','L','R','C'};

// �L�[�{�[�h�O���t�B�b�N�\���o�b�t�@(84*42)
static unsigned char kbBitmap[42][42];

// �L�[�{�[�h�O���t�B�b�N�p�^�[���𐶐�����
static void createKbGraph(void)
{
	int y;
	unsigned char* basePt;

	for (y=0;y<42;++y)
	{
		int x;

		// �g���p�^�[����I��
		if (y<21)
		{
			basePt=kbptn[0]; // 0-20 �����Օt��(index 0)
		}
		else
		{
			if (y==21)
			{
				basePt=kbptn[1]; // 21 ����(index 1)
			}
			else
			{
				basePt=kbptn[2]; // 22-41 �����̂�(index 2)
			}
		}
		for (x=0;x<84;++x)
		{
			kbBitmap[y][x]=(basePt[x]);
		}
	}
}

// �\���ʒu
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

// �e�L�X�g�F��`
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
	" �SLOAD ", // F1
	" �SSAVE ", // F2
	" ****** ", // F3
	"���F�ԍ�", // F4
	" �O���F ", // F5
	" �����F ", // F6
	" ���F�� ", // F7
	"�SEXPORT", // F8
	"�PEXPORT", // F9
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
	
	// ���t�����w���v
	B_LOCATE(dispKbHelpX,dispKbHelpY);
	printf("         F G   J K l");
	B_LOCATE(dispKbHelpX,dispKbHelpY+1);
	printf("Oct Z/X C V B N M , .");
	
}

/* �����ς��Ȃ������̕\�� */
static void dispHeaders(void)
{
	// ���ʕ���
	B_COLOR(commonHeaderCol);
	B_LOCATE(0,dispToneNameY);
	printf("���F:");
	B_LOCATE(0,dispCommonY);
	printf("    FB:  ALGO:  MASK:  FORM:  SYNC:LFOSPD:   PMD:   AMD:   PMS:   AMS:   PAN:");

	B_COLOR(opHeaderCol);
	// �I�y���[�^�[�w�b�_
	{
		int op;

		for (op=0;op<4;++op)
		{
			B_LOCATE(0,dispOpStartY+op*dispOpHeight+1);
			printf("    TL:   D1L:    AR:   D1R:   D2R:    RR:    KS:   MUL:   DT1:   DT2: AMSEN:");
		}
	}
	// ��ԉ��̃t�@���N�V�����L�[�\��
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

/* �I�y���[�^�[�}�X�N�\�� */
static void dispOpMaskHdr()
{
	int op;
	for (op=0;op<4;++op)
	{
		int dispY=dispOpStartY+op*dispOpHeight;
		// �I�y���[�^ON/OFF���\��
		B_COLOR(opTopCol);
		B_LOCATE(0,dispY);
		printf("OP%2d %s",op+1,(((dispToneData.opMask)&(1<<op))) ? "ON ":"OFF");
	}
}

/* ���F�������ւ� */
static void dispToneName(void)
{
	B_LOCATE(6,dispToneNameY);
	B_COLOR(toneNameCol);
	printf("%s",toneName);
	B_COLOR(normalParamCol);
	B_ERA_ED();
}

// �A���S���Y���w���v�\���i�\���ʂ������̂Ő؂蕪���j
static void dispAlgorithm()
{
	int i;
	
	/* �A���S���Y���w���v�\�� */
	B_COLOR(normalParamCol);
	for (i=0;i<3;++i)
	{
		B_LOCATE(7,dispCommonY+2+i);
		printf("%s",algStr[dispToneData.algorithm][i]);
	}
}

/* ���ʃf�[�^�\�� */
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
			// �I�y���[�^�}�X�N�͗�O
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


/* �e�I�y���[�^�f�[�^�\�� */
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
	oldCrtMode=CRTMOD(-1);
	CRTMOD(16); // 768*512�ɐ؂�ւ���
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

	// �ŏ��͑S���`��
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
	// �e�L�X�g�N���A
	B_CLR_AL();
	// �w�b�_����������
	dispHeaders();
	// �S���`��
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
	CRTMOD(oldCrtMode);
	TGUSEMD(0,oldGraphMode);
	TGUSEMD(1,oldTextMode);
	B_CURON();
}

void setDispToneName(const char* name)
{
	strncpy(toneName,name,127);
	toneName[127]=0;
	toneDataDirty[DIRTY_TONENAME]=1;
}

/* �ǂ�����g�������߂��˂Ă���̂ŗ����p�� */
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
			toneDataDirty[DIRTY_ALGO]=1; // �A���S���Y�������ς����
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
		// �ړ��O�J�[�\���ĕ`��Ώ�
		toneDataDirty[curY]=1;
		curX=x;curY=y;
		// �ړ���J�[�\���ĕ`��Ώ�
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

/* ���͋��ʑO���� */
static void preInput(const char* prompt)
{
	B_LOCATE(0,dispPromptY);
	B_COLOR(inputPromptCol);
	printf("%s",prompt);
	B_COLOR(inputCol);
	B_ERA_ED();
	waitKeyClear();
}

/* ���͋��ʌ㏈�� */
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

/* �v�����v�g�t����������� */
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
				inpBuf[strlen(inpBuf)-1]=0; // ���s��������
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
/* �v�����v�g���ꕶ������ */
char inputChar(const char* prompt)
{
	char ret=0;
	preInput(prompt);
	ret=INKEY();
	postInput();
	return ret;
}

