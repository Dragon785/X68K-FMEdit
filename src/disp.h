/* �\���֌W */
#if (!defined(__DISP_H__))
#define __DISP_H__

#include "fmregdef.h"

extern void initDisp(const int oct);
extern void updateDisp(void);
extern void finishDisp(void);

/* �v�����v�g�t����������� */
extern void inputString(char* ans,const int len,const char* prompt);
/* �v�����v�g���ꕶ������ */
extern char inputChar(const char* prompt);
/* �v�����v�g�\�� */
extern void showPrompt(const char* prompt);
/* �v�����v�g���� */
extern void hidePrompt();

/* �ǂ�����g�������߂��˂Ă���̂ŗ����p�� */
extern void setDispToneData(const toneData* src);
extern void setDispOpmParam(const opmParam* src);

extern void setDispToneName(const char* name);

extern void getDispToneData(toneData* dst);
extern void getDispOpmParam(opmParam* dst);

extern void changeDispToneData(const int curX,const int curY,const int offset);
extern void changeDispOpMask(const int op);

// ��ʑS����������
extern void refreshAllScreen(void);

/* �\���G���A�ɗ]�T������΁E�E�X�v���C�g�g���H */
extern void dispNoteOn(const int note);
extern void dispNoteOff(const int note);
extern void dispAllNoteOff(void);

/* �J�[�\���ʒu�w�� �\���p�����[�^�P��(11*5) */
extern void setCursor(const int x,const int y);

/* �I�N�^�[�u�Z�b�g */
extern void dispPlayOctave(const int oct);

#endif

