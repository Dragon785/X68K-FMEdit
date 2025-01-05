/* ���F�f�[�^�Q���� */
#if (!defined(__TONEDATA_H__))
#define __TONEDATA_H__

#include "fmregdef.h"

#define TONENAMEMAX (64) // ���F���ő�o�C�g

// ���O�����F�f�[�^
typedef struct
{
	char toneName[TONENAMEMAX];
	toneData tone;
} toneDataWithName;

#define TONEDATAMAX 64 // �f�[�^�ő吔 �����񉻎g���̂Ŋ��ʂł����ꂸ

extern void initToneData(const char* defFile); // ���������Ɏw�肵���t�@�C��������Γǂݍ���

// ���F�f�[�^�Ǘ�
extern toneDataWithName allToneData[TONEDATAMAX];

// ���F�f�[�^�ǂݍ���
extern int readAllToneData(void);
// ���F�f�[�^�����o��
extern int writeAllToneData(void);

#endif

