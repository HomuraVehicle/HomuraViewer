#ifndef HMR_ITFCOM_INC
#define HMR_ITFCOM_INC 100
#
#include <HomuraViewer/Data.hpp>

/*===hmrItfCom===
hmrItfCom:v1_01/130324 hmIto
	is_empty/is_fullをempty/fullに変更
	各関数のコメントを書き直す
hmrItfCom:v1_00/130310 hmito
	関数名等整理
Sample_for_vmc_hmrAbtCom: v1_00/130302 amby
	とりあえず大枠を作成してみる。

思想
　Operatorに必要最低限の機能を提供するためのクラス。
　hmrComとOperatorの関係を明確にする

*/
namespace hmr{
	class itfRecvCom{
	public:
		//受信バッファ内のデータの存在の有無を確認する
		virtual bool empty()=0;
		//受信バッファ内からデータを取得する
		virtual hmr::datum recv()=0;
		//受信ストリームから更新する必要があるかを確認する
		virtual bool can_sync() = 0;
		//受信ストリームから受信バッファを更新する
		virtual void sync()=0;
	};
	class itfSendCom{
	public:
		//送信バッファの空きを確認する関数
		virtual bool full()=0;
		//送信バッファにデータを追加する
		virtual void send(const hmr::datum& dat_)=0;
		//送信ストリームに送信バッファを反映する
		virtual void flush()=0;
		//送信ストリームに反映可能かを確認する
		virtual bool can_flush() = 0;
	};
	class itfCom:public itfRecvCom,public itfSendCom{};
}

#
#endif
