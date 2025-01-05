/* �L�[���͎���𒊏ۉ����� */
#if (!defined(__KEYUTIL_H__))
#define __KEYUTIL_H__

/* �L�[�̒�` */
enum KeyType
{
	/* �V�X�e���֌W */
	KEY_EXIT=0,
	KEY_ALLSAVE, // �o���N�S���Z�[�u
	KEY_ALLLOAD, // �o���N�S�����[�h
	KEY_SETNAME, // ���O������
	KEY_CHANGETONE, // ���F�ւ�(�ԍ��w��)
	KEY_NEXTTONE, // ���F�ւ�(+1)
	KEY_PREVTONE, // ���F�ւ�(-1)
	KEY_ALLEXPORT, // �G�N�X�|�[�g(���F�S��)
	KEY_SINEXPORT, // �G�N�X�|�[�g(���݂̉��F)
	/* �J�[�\���֌W */
	KEY_CUR_UP,
	KEY_CUR_DOWN,
	KEY_CUR_LEFT,
	KEY_CUR_RIGHT,
	/* ���l�ύX */
	KEY_VALUE_UP_1,
	KEY_VALUE_UP_10,
	KEY_VALUE_DOWN_1,
	KEY_VALUE_DOWN_10,
	/* �I�y���[�^�}�X�N */
	KEY_OPMASK_1,
	KEY_OPMASK_2,
	KEY_OPMASK_3,
	KEY_OPMASK_4,
	/* ���[�e�B���e�B�֘A */
	KEY_COPYTOBUF, // ���[�N�o�b�t�@�փR�s�[
	KEY_PASTETOCURRENT, // ���[�N�o�b�t�@����\��t��
	/* ���t�֌W */
	KEY_OCT_DOWN,
	KEY_OCT_UP,
	KEY_PLAY_C, /* C */
	KEY_PLAY_C_,/* C# */
	KEY_PLAY_D, /* D */
	KEY_PLAY_D_,/* D# */
	KEY_PLAY_E, /* E */
	KEY_PLAY_F, /* F */
	KEY_PLAY_F_,/* F# */
	KEY_PLAY_G, /* G */
	KEY_PLAY_G_,/* G# */
	KEY_PLAY_A, /* A */
	KEY_PLAY_A_,/* A# */
	KEY_PLAY_B,  /* B */
	KEY_NUM
};

extern int keyTrigOn[KEY_NUM]; /* �������u�Ԃ̂�!=0 */
extern int keyPress[KEY_NUM]; /* ������Ă����!=0 */
extern int keyTrigOff[KEY_NUM]; /* �����ꂽ�u��!=0 */

extern void initKeyBuf(void);
extern void updateKeyBuf(void);

/* �L�[�o�b�t�@���N���A�����܂ŋ�ǂ݂��� */
extern void waitKeyClear(void);

#endif

