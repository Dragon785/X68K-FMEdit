/* レジスタレベルでのOPM制御 */
#if (!defined(__FMCTRL_H__))
#define __FMCTRL_H__

#include "fmregdef.h"

extern void toneDataToOpmParam(opmParam* dst,const toneData* src);
extern void opmParamToToneData(toneData* dst, const opmParam* src);

/* OPM全体のパラメータ設定 */
extern void setOPMParam(const opmParam* param);
extern void setToneData(const toneData* src);
/* OPM全体の共通パラメータ設定(4種) */
extern void setOPMParam1(const opmParam1* param);
extern void setOPMParam2(const opmParam2* param);
extern void setOPMParam3(const opmParam3* param);
extern void setOPMParam4(const opmParam4* param);

/* 特定のチャンネル全体のパラメータ設定 */
extern void setChParam(const chParam* param,int ch);

/* 特定のチャンネルのチャンネル共通データ設定(2種) */
extern void setChParam1(const chParam1* param, int ch);
extern void setChParam2(const chParam2* param, int ch);

/* 特定のチャンネルの特定のオペレータだけのパラメータ設定 */
extern void setOpParam(const opParam* param,int ch,int op);

/* 特定のチャンネルの特定のオペレータの特定のアドレスパラメータ設定(6種類) */
extern void setOpParam1(const opParam1* param, int ch, int op);
extern void setOpParam2(const opParam2* param, int ch, int op);
extern void setOpParam3(const opParam3* param, int ch, int op);
extern void setOpParam4(const opParam4* param, int ch, int op);
extern void setOpParam5(const opParam5* param, int ch, int op);
extern void setOpParam6(const opParam6* param, int ch, int op);

/* 特定のチャンネルのキーオン(オペレータマスクつき) */
/* noteは0がC以下半音毎に+1(11まで) */
extern void keyOn(int ch,int oct,int note,int mask);

/* 特定のチャンネルのキーオフ */
extern void keyOff(int ch);

#endif

