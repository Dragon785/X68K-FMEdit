#include "diskio.h"

int fexists(const char* filename) // �t�@�C��������
{
	int ret=0;
	FILE* f;

	f=fopen(filename,"rb");
	if (f)
	{
		fclose(f);
		ret=1;
	}
	
	return ret;
}

void readToneData(FILE* f,toneDataWithName* dst)
{
	fread(dst,sizeof(toneDataWithName),1,f);
}

void writeToneData(FILE* f,const toneDataWithName* src)
{
	fwrite(src,sizeof(toneDataWithName),1,f);
}

/* �G�L�X�|�[�g�̋��ʕ��� */
/* �R�����g�����Ȃǎ�ʂňႤ�Ƃ���͌ďo���ɋz�� */
static void exportCommonArea(FILE* f,const toneDataWithName* src,
	const char* comPrefix,const char* comSuffix, // �R�����g�v���t�B�N�X/�T�t�B�b�N�X
	const char* packPrefix,const char* packSuffix, // �z������̋�؂蕶�� {} ()
	const char* contstr, // �ŏI�s�ȊO�̒ǉ����� ,
	const char* laststr // �ŏI�s�̒ǉ�����
	)
{
	int ope;
	const toneData* t=&(src->tone);
	
	// �S�̒�`
	// �w�b�_
	fprintf(f,"%s\t FA  OM  WF  SY  SP PMD AMD PMS AMS PAN%s\n",comPrefix,comSuffix);
	fprintf(f,  "\t%s%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,  0%s%s\n",
		packPrefix,
		(t->fbLevel<<3)|(t->algorithm), // FEEDBACK/ALGORITHM
		t->opMask, // OM
		t->lfoWaveform, // WF
		t->syncLFO, // SY
		t->speedLFO, // SP
		t->pmd, // PMD
		t->amd, // AMD
		t->pms, // PMS
		t->ams, // AMS
		t->pan, // PAN
		packSuffix,
		contstr
	);
	
	// �I�y���[�^����
	// �w�b�_
	  fprintf(f,"%s\t AR  DR  SR  RR  SL  OL  KS  ML DT1 DT2 AME%s\n",comPrefix,comSuffix);
	for (ope=0;ope<4;++ope)
	{
		const opData* op=&(t->opData[ope]);
		fprintf(f,"\t%s%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d%s",
		packPrefix,
		op->attackRate, // AR
		op->decay1Rate, // DR(D1R)
		op->decay2Rate, // SR(D2R)
		op->releaseRate, // RR
		op->decay1Level, // SL(D1L)
		op->totalLevel, // OL(TL)
		op->keyScale, // KS
		op->freqMul, // ML
		op->detune1, // DT1
		op->detune2, // DT2
		op->amsEnable, // AME
		packSuffix
		);
		if (ope==3)
		{
			fprintf(f,"%s",laststr);
		}
		else
		{
			fprintf(f,"%s",contstr);
		}
		fprintf(f,"\n");
	}
}

// ZMS�����G�L�X�|�[�g�i���F�ԍ�����B�P�X�^�[�g���ۂ��̂ō��킹��)
void exportToneDataAsZMS(FILE* f,const toneDataWithName* src,const int voiceNo)
{
	fprintf(f,"(v%d,0\n",voiceNo);
	exportCommonArea(f,src,"/","", // �R�����g�J�n��/�ŏI�������͂Ȃ�
	"","", // �z��̑O��ɂ͂Ȃɂ����Ȃ�
	"", // �r���ɂ��Ȃɂ����Ȃ�
	")" // �ŏI�s�ɂ�)������
	);
}

// C�w�b�_�����G�L�X�|�[�g
void exportToneDataAsCHdr(FILE* f,const toneDataWithName* src)
{
	fprintf(f,"{");
	exportCommonArea(f,src,"/*","*/", // �R�����g�� /* .. */ �Ŋ���
	"{","}", // �z��̑O���{}�ł�����
	",", // �r���ɂ̓J���}
	""); // �ŏI�s�ɂ͂Ȃɂ����Ȃ�
	fprintf(f,"},\n");
}

// BASIC�����G�L�X�|�[�g
// BASIC��1���F���Ƃɕʕϐ��ŏ����o���Ȃ��Ƃ����Ȃ�
void exportToneDataAsBas(FILE* f,const toneDataWithName* src,const int voiceNo)
{
	fprintf(f,"dim char toneData%d(4,10)={\n",voiceNo);
	exportCommonArea(f,src,"/*","", // �R�����g�� /* �Ŏn�܂�
	"+","", // �z��̐擪�ɂ�+��ǉ��A���ɂ͂Ȃɂ�����Ȃ�
	",", // �r���ɂ̓J���}
	"}"); // �Ō�}�ŕ���
}

