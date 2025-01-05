#include <stdio.h>
#include <string.h>
#include "tonedata.h"
#include "disp.h"
#include "diskio.h"

toneDataWithName allToneData[TONEDATAMAX];

static void readAllDataSub(const char* filename)
{
	int i;
	char buf[128];
	
	FILE* f=fopen(filename,"rb");
	
	for (i=0;i<TONEDATAMAX;++i)
	{
		sprintf(buf,"�ꊇ���[�h�� %d/%d",i+1,TONEDATAMAX);
		showPrompt(buf);
		readToneData(f,&(allToneData[i]));
	}
	fclose(f);
	hidePrompt();
}

void initToneData(const char* defFile)
{
	if (fexists(defFile))
	{
		readAllDataSub(defFile);
	}
	else
	{
		// ���Ŗ��O�������đS��0�ɂ���
		int i;

		showPrompt("���F��������...");
		
		for (i=0;i<TONEDATAMAX;++i)
		{
			int op;
			
			toneDataWithName* p=allToneData+i;
			sprintf(p->toneName,"���F#%d",i+1);
			memset(&(p->tone),0,sizeof(toneData));
			// �Œ�������o��悤�Ƀp�����[�^�[�C��
			p->tone.algorithm=7;
			p->tone.opMask=15;
			p->tone.pan=3;

			for (op=0;op<4;++op)
			{
				p->tone.opData[op].attackRate=31;
				p->tone.opData[op].releaseRate=15;
				p->tone.opData[op].freqMul=1;
			}
		}
		hidePrompt();
	}
}

// ���F�f�[�^�ǂݍ���
int readAllToneData(void)
{
	char tmp[64];

	inputString(tmp,64,"���[�h�t�@�C����:");
	if (strlen(tmp)>0)
	{
		
		if (fexists(tmp)==0)
		{
			(void)inputChar("�w��t�@�C�������݂��܂���B(Any Key)");
			return 1; // fail
		}
		readAllDataSub(tmp);
	}
	
	return 0; // success
}

// ���F�f�[�^�����o��
int writeAllToneData(void)
{
	char tmp[64];
	
	inputString(tmp,32,"�Z�[�u�t�@�C����:");
	if (strlen(tmp)>0)
	{
		int i;
		FILE* f;

		// �t�@�C�����݃`�F�b�N
		if (fexists(tmp))
		{
			char ans;
			
			ans=inputChar("�t�@�C�������݂��܂��B�㏑�����܂����H(Y/Other)");
			if ((ans!='y')&&(ans!='Y'))
			{
				return 1; // ���f
			}
		}
		f=fopen(tmp,"wb");
		for (i=0;i<TONEDATAMAX;++i)
		{
			writeToneData(f,&(allToneData[i]));
		}
		fclose(f);
	}

	return 0; // success
}

