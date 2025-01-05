/* 表示関係 */
#if (!defined(__DISP_H__))
#define __DISP_H__

#include "fmregdef.h"

extern void initDisp(const int oct);
extern void updateDisp(void);
extern void finishDisp(void);

/* プロンプト付き文字列入力 */
extern void inputString(char* ans,const int len,const char* prompt);
/* プロンプトつき一文字入力 */
extern char inputChar(const char* prompt);
/* プロンプト表示 */
extern void showPrompt(const char* prompt);
/* プロンプト消去 */
extern void hidePrompt();

/* どちらを使うか決めかねているので両方用意 */
extern void setDispToneData(const toneData* src);
extern void setDispOpmParam(const opmParam* src);

extern void setDispToneName(const char* name);

extern void getDispToneData(toneData* dst);
extern void getDispOpmParam(opmParam* dst);

extern void changeDispToneData(const int curX,const int curY,const int offset);
extern void changeDispOpMask(const int op);

// 画面全部書き直し
extern void refreshAllScreen(void);

/* 表示エリアに余裕があれば・・スプライト使う？ */
extern void dispNoteOn(const int note);
extern void dispNoteOff(const int note);
extern void dispAllNoteOff(void);

/* カーソル位置指示 表示パラメータ単位(11*5) */
extern void setCursor(const int x,const int y);

/* オクターブセット */
extern void dispPlayOctave(const int oct);

#endif

