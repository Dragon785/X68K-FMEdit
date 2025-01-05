#if (!defined(__FMREGDEF_H__))
#define __FMREGDEF_H__

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	unsigned char attackRate; /* AR 0-31 */
	unsigned char decay1Rate; /* D1R 0-31 */
	unsigned char decay2Rate; /* D2R 0-31 */
	unsigned char releaseRate; /* RR 0-15 */
	unsigned char decay1Level; /* D1L 0-15 */
	unsigned char totalLevel; /* TL 0-127 0:max */
	unsigned char keyScale; /* KeyScale 0-3 */
	unsigned char freqMul; /* Freq multiply 0-15 */
	unsigned char detune1; /* Detune 1 0-7 */
	unsigned char detune2; /* Detune 2 0-3 */
	unsigned char amsEnable; /* Amplitude Modulation Enable */
	unsigned char dummy; /* pudding */
} opData;

typedef struct
{
	unsigned char fbLevel; /* Feedback Level 0-7 */
	unsigned char algorithm; /* Algorithm 0-7 */
	unsigned char lfoWaveform; /* LFO WaveForm 0-3 */
	unsigned char syncLFO; /* LFO Sync 0-1 */
	unsigned char speedLFO; /* LFO Speed 0-255 */
	unsigned char pmd; /* Phase modulation depth 0-127(=-63 to 63) */
	unsigned char amd; /* Amplitude modulation depth 0-127 */
	unsigned char pms; /* Phase modulation sense 0 to 7 */
	unsigned char ams; /* Amplitude modulation sense 0 to 7 */
	unsigned char pan; /* pan 0-3 0:disable */
	unsigned char opMask; /* Operator Mask */
	unsigned char dummy; /* pudding */
	opData opData[4];
} toneData;

/* レジスタにセットする際の構造体群 */
/* オペレーター１つ分のパラメータ */
typedef struct
{
	unsigned char dummy:1;
	unsigned char dt1:3; /* detune 1 0 to 7 */
	unsigned char mul:4; /* freq. multiply */
} opParam1; /* op1:$40+ch op3:$48+ch op2:$50+ch op4:$58+ch */

typedef struct
{
	unsigned char dummy:1;
	unsigned char tl:7; /* total level 0 to 127 0:max 127:mute */
} opParam2; /* op1:$60+ch op3:$68+ch op2:$70+ch op4:$78+ch */

typedef struct
{
	unsigned char keyscale:2; /* key scale 0 to 3 */
	unsigned char dummy:1;
	unsigned char ar:5; /* attack rate 0 to 15 */
} opParam3; /* op1:$80+ch op3:$88+ch op2:$90+ch op4:$98+ch */

typedef struct
{
	unsigned char amsEnable:1; /* ams enable */
	unsigned char dummy:2;
	unsigned char d1r:5; /* d1 rate 0 to 15 */
} opParam4; /* op1:$a0+ch op3:$a8+ch op2:$b0+ch op4:$b8+ch */

typedef struct
{
	unsigned char dt2:2; /* detune 2 0 to 3*/
	unsigned char dummy:1;
	unsigned char d2r:5; /* d2 rate 0 to 15 */
	
} opParam5; /* op1:$c0+ch op3:$c8+ch op2:$d0+ch op4:$d8+ch */

typedef struct
{
	unsigned char d1l:4; /* d1 level */
	unsigned char rr:4; /* release rate */
} opParam6; /* op1:$e0+ch op3:$e8+ch op2:$f0+ch op4:$f8+ch */

typedef struct
{
	opParam1 opp1;
	opParam2 opp2;
	opParam3 opp3;
	opParam4 opp4;
	opParam5 opp5;
	opParam6 opp6;
} opParam;

/* チャンネルごとのパラメータ */
typedef struct
{
	unsigned char pan:2; /* PAN 0:off 1:L 2:R 3:C */
	unsigned char feedbacklevel:3; /* Feedback Level 0 to 7 */
	unsigned char algorithm:3; /* Algorithm 0 to 7 */
} chParam1; /* addr:0x20+ch */

typedef struct
{
	unsigned char dummy1:1;
	unsigned char pms:3; /* phase modulation sensivity 0 to 7 */
	unsigned char dummy2:2;
	unsigned char ams:2; /* ampletude modulation sensivity 0 to 3 */
} chParam2; /* addr:0x38+ch */

typedef struct
{
	chParam1 chp1;
	chParam2 chp2;
	opParam op[4];
	unsigned char opMask;
} chParam;

/* 音源全体でのパラメータ */
typedef struct
{
	unsigned char lfofrq; /* lfo frequency */
} opmParam1; /* 0x18 */

typedef struct
{
	unsigned char forPMD:1; /* for PMD:1 AMD:0 */
	unsigned char lfoDepth:7; /* depth: pmd -64 to 63 amd 0 to 127 */
} opmParam2; /* 0x19 */

typedef struct
{
	unsigned char ct1:1;
	unsigned char ct2:1;
	unsigned char dummy:4;
	unsigned char lfowf:2; /* LFO wave form 0:saw 1:sq 2:tri 3:s&h */
} opmParam3; /* 0x1b */

typedef struct
{
	unsigned char noiseen:1; /* noise enable */
	unsigned char dummy:2;
	unsigned char noisefreq:5; /* fnoise=4/(32*freq) */
} opmParam4; /* 0x0f */

/* OPM全体 */
typedef struct
{
	opmParam1 opmp1;
	opmParam2 opmp2a; /* AMD用 */
	opmParam2 opmp2p; /* PMD用 */
	opmParam3 opmp3;
	opmParam4 opmp4;
	chParam ch[8];
} opmParam;

/* 演奏制御 */
typedef struct
{
	unsigned char dummy:1;
	unsigned char kon:4; /* keyon op4/op3/op2/op1 */
	unsigned char ch:3; /* target channel */
} playParam1; /* 0x08 */

typedef struct
{
	unsigned char dummy:1;
	unsigned char oct:3; /* オクターブ */
	unsigned char note:4; /* note 半音ずれているのに注意 */
} playParam2; /* 0x28+ch */

#pragma pack(pop)

#endif
