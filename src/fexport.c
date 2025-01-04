#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "tonedata.h"
#include "disp.h"
#include "diskio.h"

// �w��͈͂̉��F���G�L�X�|�[�g����
static void FileExportRange(const char* topMsg,const int st,const int ed)
{
	static enum
	{
		CONV_NONE=0,
		CONV_ZMS,
		CONV_CHDR,
		CONV_BAS
	} convType=CONV_NONE;

	int total=ed-st+1;

	assert (st>=0);
	assert (ed>=st);
	assert (TONEDATAMAX>ed);

	
	// �ŏ��ɃG�L�X�|�[�g��ʂ�I��
	{
		char buf[256];
		char t;

		sprintf(buf,"%s��� 1:ZMS 2:C�w�b�_ 3:BASIC other:exit",topMsg);

		t=inputChar(buf);

		switch(t)
		{
			case '1':
				convType=CONV_ZMS;
				break;
			case '2':
				convType=CONV_CHDR;
				break;
			case '3':
				convType=CONV_BAS;
				break;
			default:
				break;
		}
	}

	if (convType==CONV_NONE)
	{
		return;
	}

	{
		char fname[128];
		// �G�L�X�|�[�g�t�@�C��������
		inputString(fname,128,"�o�̓t�@�C����(��==�I��)");
		if (strlen(fname)==0)
		{
			return;
		}
		// �I�[�o�[���C�g�m�F
		if (fexists(fname))
		{
			char ow=inputChar("�t�@�C�������݂��܂��B�㏑�����܂����H(y/Other)");
			if ((ow!='y')&&(ow!='Y'))
			{
				return;
			}
		}
		{
			int i;
			char prbuf[128];
			
			FILE* f=fopen(fname,"wt");
			// �擪���� (BASIC��1���F���ƕʔz��ɂ��Ȃ��Ƃ����Ȃ�)
			switch (convType)
			{
				case CONV_ZMS: // do nothing
					break;
				case CONV_CHDR:
					fprintf(f,"const char toneData[%d][5][11]={\n",total);
					break;
				case CONV_BAS:
					// do nothing
					break;
				default:
					break;
			}
			
			// �����o������
			for (i=st;i<=ed;++i)
			{
				sprintf(prbuf,"�o�͒�(%d/%d)",i-st+1,total);
				showPrompt(prbuf);
				switch (convType)
				{
					case CONV_ZMS:
						exportToneDataAsZMS(f,&(allToneData[i]),i-st+1);
						break;
					case CONV_CHDR:
						exportToneDataAsCHdr(f,&(allToneData[i]));
						break;
					case CONV_BAS:
						exportToneDataAsBas(f,&(allToneData[i]),i-st+1);
					default:
						break;
				}
			}
			// �I�[
			switch (convType)
			{
				case CONV_ZMS: // do nothing
					break;
				case CONV_CHDR:
					fprintf(f,"};\n");
					break;
				case CONV_BAS:
					fprintf(f,"%c",0x1a); // BASIC�I�[
				default:
					break;
			}
			
			fclose(f);
			hidePrompt();
		}
	}
}

void FileExportAll(void)
{
	FileExportRange("�S���F�G�L�X�|�[�g",0,TONEDATAMAX-1);
}

void FileExportSingle(const int voiceNo) // �I���������F�����G�N�X�|�[�g
{
	FileExportRange("���݂̉��F�G�L�X�|�[�g",voiceNo,voiceNo);
}
