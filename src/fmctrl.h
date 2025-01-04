/* ���W�X�^���x���ł�OPM���� */
#if (!defined(__FMCTRL_H__))
#define __FMCTRL_H__

#include "fmregdef.h"

extern void toneDataToOpmParam(opmParam* dst,const toneData* src);
extern void opmParamToToneData(toneData* dst, const opmParam* src);

/* OPM�S�̂̃p�����[�^�ݒ� */
extern void setOPMParam(const opmParam* param);
extern void setToneData(const toneData* src);
/* OPM�S�̂̋��ʃp�����[�^�ݒ�(4��) */
extern void setOPMParam1(const opmParam1* param);
extern void setOPMParam2(const opmParam2* param);
extern void setOPMParam3(const opmParam3* param);
extern void setOPMParam4(const opmParam4* param);

/* ����̃`�����l���S�̂̃p�����[�^�ݒ� */
extern void setChParam(const chParam* param,int ch);

/* ����̃`�����l���̃`�����l�����ʃf�[�^�ݒ�(2��) */
extern void setChParam1(const chParam1* param, int ch);
extern void setChParam2(const chParam2* param, int ch);

/* ����̃`�����l���̓���̃I�y���[�^�����̃p�����[�^�ݒ� */
extern void setOpParam(const opParam* param,int ch,int op);

/* ����̃`�����l���̓���̃I�y���[�^�̓���̃A�h���X�p�����[�^�ݒ�(6���) */
extern void setOpParam1(const opParam1* param, int ch, int op);
extern void setOpParam2(const opParam2* param, int ch, int op);
extern void setOpParam3(const opParam3* param, int ch, int op);
extern void setOpParam4(const opParam4* param, int ch, int op);
extern void setOpParam5(const opParam5* param, int ch, int op);
extern void setOpParam6(const opParam6* param, int ch, int op);

/* ����̃`�����l���̃L�[�I��(�I�y���[�^�}�X�N��) */
/* note��0��C�ȉ���������+1(11�܂�) */
extern void keyOn(int ch,int oct,int note,int mask);

/* ����̃`�����l���̃L�[�I�t */
extern void keyOff(int ch);

#endif

