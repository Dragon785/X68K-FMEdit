#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "fmctrl.h"

void setToneData(const toneData* src)
{
	opmParam opmP;
	toneDataToOpmParam(&opmP,src);
	setOPMParam(&opmP);
}

void toneDataToOpmParam(opmParam* dst, const toneData* src)
{
	int ch;

	memset(dst, 0, sizeof(dst));

	dst->opmp1.lfofrq = src->speedLFO;
	dst->opmp2a.forPMD = 0;
	dst->opmp2a.lfoDepth = src->amd;
	dst->opmp2p.forPMD = 1;
	dst->opmp2p.lfoDepth = src->pmd;
	dst->opmp3.lfowf = src->lfoWaveform;
	
	/* ノイズ周りは使わない(全体で１つのみ) */

	/* 各チャンネルパラメータ */
	for (ch = 0; ch < 8; ++ch)
	{
		int op;

		dst->ch[ch].chp1.algorithm = src->algorithm;
		dst->ch[ch].chp1.feedbacklevel = src->fbLevel;
		dst->ch[ch].chp1.pan = src->pan;
		dst->ch[ch].opMask=src->opMask;
		
		/* 各オペレータデータ */
		for (op = 0; op < 4; ++op)
		{
			const opData* srcOp = &(src->opData[op]);
			opParam* dstOp = &(dst->ch[ch].op[op]);
			dstOp->opp1.dt1 = srcOp->detune1;
			dstOp->opp1.mul = srcOp->freqMul;

			dstOp->opp2.tl = srcOp->totalLevel;
			
			dstOp->opp3.ar = srcOp->attackRate;
			dstOp->opp3.keyscale = srcOp->keyScale;

			dstOp->opp4.amsEnable = srcOp->amsEnable;
			dstOp->opp4.d1r = srcOp->decay1Rate;

			dstOp->opp5.d2r = srcOp->decay2Rate;
			dstOp->opp5.dt2 = srcOp->detune2;

			dstOp->opp6.d1l = srcOp->decay1Level;
			dstOp->opp6.rr = srcOp->releaseRate;
		}
	}

}

void opmParamToToneData(toneData* dst, const opmParam* src)
{
	const chParam* srcCh = &(src->ch[0]);
	int op = 0;

	memset(dst, 0, sizeof(dst));

	dst->speedLFO = src->opmp1.lfofrq;
	dst->pmd = src->opmp2p.lfoDepth;
	dst->amd = src->opmp2a.lfoDepth;
	dst->lfoWaveform = src->opmp3.lfowf;

	/* チャンネル0のデータを音色として採用 */
	dst->algorithm = srcCh->chp1.algorithm;
	dst->fbLevel = srcCh->chp1.feedbacklevel;
	dst->pan = srcCh->chp1.pan;
	dst->opMask=srcCh->opMask;
	
	for (op = 0; op < 4; ++op)
	{
		const opParam* srcOp = &(srcCh->op[op]);
		opData* dstOp = &(dst->opData[op]);

		dstOp->detune1 = srcOp->opp1.dt1;
		dstOp->freqMul = srcOp->opp1.mul;

		dstOp->totalLevel = srcOp->opp2.tl;

		dstOp->attackRate = srcOp->opp3.ar;
		dstOp->keyScale = srcOp->opp3.keyscale;

		dstOp->amsEnable = srcOp->opp4.amsEnable;
		dstOp->decay1Rate = srcOp->opp4.d1r;

		dstOp->decay2Rate = srcOp->opp5.d2r;
		dstOp->detune2 = srcOp->opp5.dt2;

		dstOp->decay1Level = srcOp->opp6.d1l;
		dstOp->releaseRate = srcOp->opp6.rr;
	}
}

/* OPM全体のパラメータ設定 */
void setOPMParam(const opmParam* param)
{
	/* OPM共通パラメータ */
	setOPMParam1(&(param->opmp1));
	setOPMParam2(&(param->opmp2a));
	setOPMParam2(&(param->opmp2p));
	setOPMParam3(&(param->opmp3));
	setOPMParam4(&(param->opmp4));

	{
		int ch = 0;
		for (ch = 0; ch < 8; ++ch)
		{
			setChParam(&(param->ch[ch]), ch);
		}
	}
}

static void waitOPMReady()
{
	while ((OPMSNS()&0x80));
}

void setOPMParam1(const opmParam1* param)
{
	unsigned char writeData = *(unsigned char*)(param);
	waitOPMReady();
	OPMSET(0x18, writeData);
}

void setOPMParam2(const opmParam2* param)
{
	unsigned char writeData = *(unsigned char*)(param);
	waitOPMReady();
	OPMSET(0x19, writeData);
}

void setOPMParam3(const opmParam3* param)
{
	unsigned char writeData = *(unsigned char*)(param);
	waitOPMReady();
	OPMSET(0x1b, writeData);
}

void setOPMParam4(const opmParam4* param)
{
	unsigned char writeData = *(unsigned char*)(param);
	waitOPMReady();
	OPMSET(0x0f, writeData);
}

/* 特定のチャンネル全体のパラメータ設定 */
void setChParam(const chParam* param, int ch)
{
	setChParam1(&(param->chp1), ch);
	setChParam2(&(param->chp2), ch);
	{
		int op = 0;
		for (op = 0; op < 4; ++op)
		{
			setOpParam(&(param->op[op]), ch, op);
		}
	}
}

void setChParam1(const chParam1* param, int ch)
{
	unsigned char writeData = *(unsigned char*)(param);
	assert((ch >= 0) && (ch <= 7));

	waitOPMReady();
	OPMSET(0x20 + ch, writeData);
}

void setChParam2(const chParam2* param, int ch)
{
	unsigned char writeData = *(unsigned char*)(param);
	assert((ch >= 0) && (ch <= 7));

	waitOPMReady();
	OPMSET(0x38 + ch, writeData);
}


/* 特定のチャンネルの特定のオペレータだけのパラメータ設定 */
void setOpParam(const opParam* param, int ch, int op)
{
	setOpParam1(&(param->opp1), ch, op);
	setOpParam2(&(param->opp2), ch, op);
	setOpParam3(&(param->opp3), ch, op);
	setOpParam4(&(param->opp4), ch, op);
	setOpParam5(&(param->opp5), ch, op);
	setOpParam6(&(param->opp6), ch, op);
}

/* オペレーターごとのアドレスオフセット */
static const int opOffset[4] = { 0x00,0x10,0x08,0x18 };

void setOpParam1(const opParam1* param, int ch, int op)
{
	unsigned char writeData = *(unsigned char*)(param);

	assert((ch >= 0) && (ch <= 7));
	assert((op >= 0) && (op <= 3));

	waitOPMReady();
	OPMSET(0x40 + opOffset[op] + ch, writeData);
}

void setOpParam2(const opParam2* param, int ch, int op)
{
	unsigned char writeData = *(unsigned char*)(param);
	assert((ch >= 0) && (ch <= 7));
	assert((op >= 0) && (op <= 3));

	waitOPMReady();
	OPMSET(0x60 + opOffset[op] + ch, writeData);
}

void setOpParam3(const opParam3* param, int ch, int op)
{
	unsigned char writeData = *(unsigned char*)(param);
	assert((ch >= 0) && (ch <= 7));
	assert((op >= 0) && (op <= 3));

	waitOPMReady();
	OPMSET(0x80 + opOffset[op] + ch, writeData);
}

void setOpParam4(const opParam4* param, int ch, int op)
{
	unsigned char writeData = *(unsigned char*)(param);
	assert((ch >= 0) && (ch <= 7));
	assert((op >= 0) && (op <= 3));

	waitOPMReady();
	OPMSET(0xa0 + opOffset[op] + ch, writeData);
}

void setOpParam5(const opParam5* param, int ch, int op)
{
	unsigned char writeData = *(unsigned char*)(param);
	assert((ch >= 0) && (ch <= 7));
	assert((op >= 0) && (op <= 3));

	waitOPMReady();
	OPMSET(0xc0 + opOffset[op] + ch, writeData);
}

void setOpParam6(const opParam6* param, int ch, int op)
{
	unsigned char writeData = *(unsigned char*)(param);
	assert((ch >= 0) && (ch <= 7));
	assert((op >= 0) && (op <= 3));

	waitOPMReady();
	OPMSET(0xe0 + opOffset[op] + ch, writeData);
}

/* note→OPMのNOTE変換テーブル(全音補正のみ、オクターブはソフトで補正) */
const int toneTbl[12]={0x0c,0x0d,0x0e,0x0,0x1,
						0x2,0x4,0x5,0x6,0x8,0x9,0xa};
/* 特定のチャンネルのキーオン(オペレータマスクつき) */
/* noteは0がC,以下11まで半音ずつ */
void keyOn(int ch, int oct, int note, int mask)
{
	/* 全音高い音が出るので補正(D以下はオクターブ下げる) */
	if (note<=2)
	{
		oct--;
	}
	
	assert((ch>=0)&&(ch<=7));
	assert((oct>=0)&&(oct<=7));
	/* Set Octave/Note */
	{
		playParam2 param;
		param.dummy=0;
		param.oct=oct;
		param.note=toneTbl[note];

		waitOPMReady();
		OPMSET(0x28+ch,*(unsigned char*)(&param));
	}
	/* Set KeyOn */
	{
		playParam1 param;
		param.dummy=0;
		param.kon=mask;
		param.ch=ch;

		waitOPMReady();
		OPMSET(0x08,*(unsigned char*)(&param));
	}
}

/* 特定のチャンネルのキーオフ */
void keyOff(int ch)
{
	assert((ch>=0)&&(ch<=7));
	{
		playParam1 param={0};
		param.ch=ch;

		waitOPMReady();
		OPMSET(0x08,*(unsigned char*)(&param));
	}
}
