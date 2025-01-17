// 画面に表示するキーボードの画像
// 画面に出すには16bitだが4bitパック
// 色は次の形で割り当てる
// 0:背景 1:C 2:C# 3:D 4:D# 5:E 6:F 7:F# 8:G 9:G# 10:A 11:A# 12:B
#if (!defined(__KBPTN_H__))
#define __KBPTN_H__

unsigned char kbptn[3][12*7/2]=
{
	// 黒鍵を含むパターン 7-10-2-10-7 7-10-2-10-2-10-7
	{
		0x11,0x11,0x11,0x02,0x22,0x22,0x22,0x22,0x03,0x04,
		0x44,0x44,0x44,0x44,0x05,0x55,0x55,0x50,
		0x66,0x66,0x66,0x07,0x77,0x77,0x77,0x77,
		0x08,0x09,0x99,0x99,0x99,0x99,0x0a,0x0b,
		0xbb,0xbb,0xbb,0xbb,0x0c,0xcc,0xcc,0xc0
	},
	// 黒鍵を含まないが隙間になっているパターン 7-10-2-10-7 7-10-2-10-2-10-7
	{
		0x11,0x11,0x11,0x00,0x00,0x00,0x00,0x00,0x03,0x00,
		0x00,0x00,0x00,0x00,0x05,0x55,0x55,0x50,
		0x66,0x66,0x66,0x00,0x00,0x00,0x00,0x00,
		0x08,0x00,0x00,0x00,0x00,0x00,0x0a,0x00,
		0x00,0x00,0x00,0x00,0x0c,0xcc,0xcc,0xc0
	},
	// 黒鍵を含まず白鍵だけのパターン 12-12-12-12-12-12-12
	{
		0x11,0x11,0x11,0x11,0x11,0x10, // C
		0x33,0x33,0x33,0x33,0x33,0x30, // D
		0x55,0x55,0x55,0x55,0x55,0x50, // E
		0x66,0x66,0x66,0x66,0x66,0x60, // F
		0x88,0x88,0x88,0x88,0x88,0x80, // G
		0xaa,0xaa,0xaa,0xaa,0xaa,0xa0, // A
		0xcc,0xcc,0xcc,0xcc,0xcc,0xc0  // B
	},
};


#endif

