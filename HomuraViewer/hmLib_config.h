#ifndef HMLIB_CONFIG_INC
#define HMLIB_CONFIG_INC

//バージョン情報を表示したい場合は、以下を変更する。0:非表示、1:警告表示、2:注意表示、3:全表示
#define HMLIB_OUT 3

//ライブラリファイルを使わずにソースファイルをコンパイルしてリンクする場合は、以下を有効にする
#define HMLIB_NOLIB

//byte数の指定
#define HMLIB_TYPE_SIZE_CHAR 1
#define HMLIB_TYPE_SIZE_SHORT 2
#define HMLIB_TYPE_SIZE_INT 4
#define HMLIB_TYPE_SIZE_LONG 4
#define HMLIB_TYPE_SIZE_LONG_LONG 8
#define HMLIB_TYPE_SIZE_FLOAT 4
#define HMLIB_TYPE_SIZE_DOUBLE 8
#define HMLIB_TYPE_SIZE_LONG_DOUBLE 8

#endif
