#if (!defined(__MAKEFNT_H__))
#define __MAKEFNT_H__

// �t�H���g�f�[�^�̓ǂݍ��݂Ə�������
extern void generateFont(void);
typedef struct
{
	unsigned int beginCode;
	unsigned int x;
	unsigned int y;
} fontPicData;

extern const fontPicData algoFontData[8];

#endif

