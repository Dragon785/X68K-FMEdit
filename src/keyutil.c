#include <iocslib.h>
#include <doslib.h>
#include <stdio.h>
#include "keyutil.h"

int keyTrigOn[KEY_NUM];
int keyPress[KEY_NUM];
int keyTrigOff[KEY_NUM];

typedef struct
{
	int group;
	int bit;
	enum KeyType key;
} keySensElem;

static const keySensElem scanTable[]={
	{0,1,KEY_EXIT}, // ESC

	{0xc,3,KEY_ALLLOAD}, // F1
	{0xc,4,KEY_ALLSAVE}, // F2
//	{0xc,5,KEY_ALLEXPORT}, // F3
	{0xc,6,KEY_CHANGETONE}, // F4
	{0xc,7,KEY_PREVTONE}, // F5
	{0xd,0,KEY_NEXTTONE}, // F6
	{0xd,1,KEY_SETNAME}, // F7
	{0xd,2,KEY_ALLEXPORT}, // F8
	{0xd,3,KEY_SINEXPORT}, // F9
//	{0xd,4,KEY_PREVTONE}, // F10(Windowsエミュレータでは使えない)
	
	{7,3,KEY_CUR_LEFT},// LEFT
	{7,4,KEY_CUR_UP}, // UP
	{7,5,KEY_CUR_RIGHT}, // RIGHT
	{7,6,KEY_CUR_DOWN}, // DOWN

	// パラメータ変更系
	{2,1,KEY_VALUE_DOWN_10},// -10 Q
	{2,2,KEY_VALUE_DOWN_1}, // -1 W
	{2,3,KEY_VALUE_UP_1}, // +1 E
	{2,4,KEY_VALUE_UP_10},// +10 R

	{0,2,KEY_OPMASK_1}, // 1
	{0,3,KEY_OPMASK_2}, // 2
	{0,4,KEY_OPMASK_3}, // 3
	{0,5,KEY_OPMASK_4}, // 4
	
	/* 演奏関係 */
	{5,2,KEY_OCT_DOWN}, /* Z */
	{5,3,KEY_OCT_UP}, /* X */

	{5,4,KEY_PLAY_C}, /* C */
	{4,1,KEY_PLAY_C_}, /* F */
	{5,5,KEY_PLAY_D}, /* V */
	{4,2,KEY_PLAY_D_}, /* G */
	{5,6,KEY_PLAY_E}, /* B */
	{5,7,KEY_PLAY_F}, /* N */
	{4,4,KEY_PLAY_F_}, /* J */
	{6,0,KEY_PLAY_G}, /* M */
	{4,5,KEY_PLAY_G_}, /* K */
	{6,1,KEY_PLAY_A}, /* <, */
	{4,6,KEY_PLAY_A_}, /* L */
	{6,2,KEY_PLAY_B}, /* >. */
};


/* 内部で保存しておく一つ前のキーを押した情報 */
static int preKeyPress[KEY_NUM];

static const int SCANTABLE_SIZE=(sizeof(scanTable)/sizeof(scanTable[0]));

void initKeyBuf(void)
{
	int i;

	for (i=0;i<KEY_NUM;++i)
	{
		keyTrigOn[i]=0;keyPress[i]=0;keyTrigOff[i]=0;preKeyPress[i]=0;
	}
}


void updateKeyBuf(void)
{
	int i;
	/* BITSNSで一括して覚えておくキーバッファ */
	int bitSnsBuf[16];
	
	/* 読み込む前の情報を保存 */
	for (i=0;i<KEY_NUM;++i)
	{
		preKeyPress[i]=keyPress[i];
	}
	/* BITSNSで一気読み */
	for (i=0;i<16;++i)
	{
		bitSnsBuf[i]=BITSNS(i);
	}
	/* debug */
	/* keyPressに読み込み */
	for (i=0;i<SCANTABLE_SIZE;++i)
	{
		const keySensElem* p=&(scanTable[i]);
		keyPress[p->key]= ( (bitSnsBuf[p->group] & (1<<(p->bit)))!=0);
	}
	
	/* トリガー判定 */
	for (i=0;i<KEY_NUM;++i)
	{
		keyTrigOn[i] =(preKeyPress[i]==0)&&(keyPress[i]!=0);
		keyTrigOff[i]=(preKeyPress[i]!=0)&&(keyPress[i]==0);
	}
}

void waitKeyClear(void)
{
	KFLUSHIO(0xfe);
}

