// ファイルエキスポート処理の流れ
#if (!defined(__FEXPORT_H__))
#define __FEXPORT_H__

extern void FileExportAll(void); // 全ファイル一括エクスポート
extern void FileExportSingle(const int voiceNo); // 選択した音色だけエクスポート

#endif

