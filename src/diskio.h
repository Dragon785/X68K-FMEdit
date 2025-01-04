// �t�@�C���ւ̓��o�͊֌W
#if (!defined(__DISKIO_H__))
#define __DISKIO_H

#include <stdio.h>
#include "tonedata.h"

extern int fexists(const char* filename);
// �t�@�C���N���[�Y�Ȃǂ͏�ʂŖʓ|�݂�
extern void readToneData(FILE* f,toneDataWithName* dst);
extern void writeToneData(FILE* f,const toneDataWithName* src);

// ZMS���������o��
extern void exportToneDataAsZMS(FILE* f,const toneDataWithName* src,const int voiceNo);
// C�w�b�_�����o��
extern void exportToneDataAsCHdr(FILE* f,const toneDataWithName* src);
// BASIC�����o���B�Ō�̃f�[�^������ԍŌ��,�����Ȃ�
extern void exportToneDataAsBas(FILE* f,const toneDataWithName* src,const int voiceNo);

#endif

