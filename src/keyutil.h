/* キー入力周りを抽象化する */
#if (!defined(__KEYUTIL_H__))
#define __KEYUTIL_H__

/* キーの定義 */
enum KeyType
{
	/* システム関係 */
	KEY_EXIT=0,
	KEY_ALLSAVE, // バンク全部セーブ
	KEY_ALLLOAD, // バンク全部ロード
	KEY_SETNAME, // 名前をつける
	KEY_CHANGETONE, // 音色替え(番号指定)
	KEY_NEXTTONE, // 音色替え(+1)
	KEY_PREVTONE, // 音色替え(-1)
	KEY_ALLEXPORT, // エクスポート(音色全部)
	KEY_SINEXPORT, // エクスポート(現在の音色)
	/* カーソル関係 */
	KEY_CUR_UP,
	KEY_CUR_DOWN,
	KEY_CUR_LEFT,
	KEY_CUR_RIGHT,
	/* 数値変更 */
	KEY_VALUE_UP_1,
	KEY_VALUE_UP_10,
	KEY_VALUE_DOWN_1,
	KEY_VALUE_DOWN_10,
	/* オペレータマスク */
	KEY_OPMASK_1,
	KEY_OPMASK_2,
	KEY_OPMASK_3,
	KEY_OPMASK_4,
	/* ユーティリティ関連 */
	KEY_COPYTOBUF, // ワークバッファへコピー
	KEY_PASTETOCURRENT, // ワークバッファから貼り付け
	/* 演奏関係 */
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

extern int keyTrigOn[KEY_NUM]; /* 押した瞬間のみ!=0 */
extern int keyPress[KEY_NUM]; /* 押されている間!=0 */
extern int keyTrigOff[KEY_NUM]; /* 離された瞬間!=0 */

extern void initKeyBuf(void);
extern void updateKeyBuf(void);

/* キーバッファがクリアされるまで空読みする */
extern void waitKeyClear(void);

#endif

