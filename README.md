# X68K-FMEdit
X68000用のFM音源音色エディタ

## はじめに
X68000は比較的ソフトウェア資産がWeb上に残っていますが、FM音源の音色エディタが見当たらないので作ってみました。

## 必要環境
gcc(真里子版で確認、それ以外でもいけるかも)+XCライブラリ
VSYNCで内部同期を取っているだけなので、main.cのDISABLE_VSYNCを有効にしてビルドし直せばXCでも動くかもしれません。
OPMドライバの類は不要です(IOCS経由でOPMレジスタを直接いじっています)

## 機能
最大64音色まで内部に保持、音色名も保持している。
キーボードで音を鳴らしながらパラメータの編集が可能なので、パラメータを変えることによる音色変化が理解しやすいです。
ノートオン/オフをちゃんと見ているので、長さ固定で鳴らすとわかりにくいサスティン(D2)やリリース周りのエンベローブパラメータも確認しやすいです。

内部データ形式での64音色一括保存/ロードの他に、ZMS形式/Cヘッダ形式/BASIC変数宣言形式で64音色一括か編集中の音色単独での書き出しが可能です。

## 制限事項
Sync LFOはパラメータとして持っていますが使っていません。

## ビルド方法
srcディレクトリでmakeすれはfmedit.xができあがるはずです。

## 使用方法
fmeditで起動。起動時にdefault.tonというファイルがあれば読み込みます。

### キー操作
カーソルキー：カーソルを移動
q/w/e/r：パラメータを増減(q:-10 w:-1 e:+1 r:+10)
フルキーの1/2/3/4：それぞれのオペレータをトグル
z/x：テスト演奏のオクターブ変更(z:オクターブ下げる x:オクターブ上げる)
c/f/v/g/b/n/j/m/k/,/l/.:テスト演奏(CがCになります）
ESC:終了
ファンクションキー：データセーブ/ロードや音色名セット音色切り替え、一括/単独エキスポートなど

## 内部データ形式
名前付き音色データ(124Byte)が64個
### 名前付き音色データ
音色名(64Byte)+音色データ(60Byte)
#### 音色データ
共通データ 12byte/オペレータデータ(12byte)*4
##### 共通データ（各1Byte)
* フィードバックレベル
* アルゴリズム
* LFO Waveform
* sync LFO
* LFO Speed
* PMD(Phase modulation depth) (0-127で-63から63に対応)
* AMD(Amplitude modulation depth)
* PMS(Phase modulation sense)
* AMS(Amplitude modulation sense)
* PAN
* オペレータマスク(1が有効、最下位がオペレータ1)
* パディング

##### オペレータデータ(各1Byte)
* Attack rate
* Decay1 rate
* Decay2 rate(Sustain rate)
* Release rate
* Decay1 level(Sustain level)
* Total level
* KeyScale
* Multiply
* Detune 1
* Detune 2
* AMS Enable
* パディング
