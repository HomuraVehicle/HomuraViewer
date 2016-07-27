#ifndef HMRVLIB_MESSAGE_PACKETDATA_INC
#define HMRVLIB_MESSAGE_PACKETDATA_INC 103
#
#include<hmLib/config_vc.h>
#include<sstream>
#include<string>
#include<ctime>
#include<chrono>
#include<vector>
#include<boost/format.hpp>
#include<HomuraViewer/VMC1.h>
#include<HomuraViewer/chrono.hpp>

/*===hmrData===
データ型の管理用ファイル

hmrData:v1_03/130615 hmIto
	Datumハッシュ,Packetハッシュを追加
hmrData v1_02/130410 hmIto
	packet::Timeを復活
	packet/datumに、時間によるoperator<を追加
hmrData:v1_01/130324 hmIto
	packet::Timeを削除
hmrData:v1_00/130310 hmIto
	datum,packetを追加
*/
namespace hmr{
	namespace viewer{
		namespace message{
			namespace{
				template<typename object>
				class no_generator{
				public:
					typedef unsigned int no_type;
				private:
					static unsigned int No;
				public:
					static unsigned int no(void){ return ++No; }
				};
				template<typename object>
				unsigned int no_generator<object>::No = 0;
			}
			struct datum{
				typedef no_generator<datum> no_gen;
				typedef no_gen::no_type no_type;
				typedef vmc1_did_t id_type;
				typedef vmc1_dsize_t size_type;
				typedef clock::time_point time_point;
			public:
				//データ番号
				no_type No;
				//データ時刻
				time_point Time;
				//データID
				id_type ID;
				// データ数
				size_type Size;
				//文字列
				//	注意：ここには純粋なコマンドのみ入る。ＩＤ，Ｓｉｚｅのデータは含まれない
				std::string Str;
				//エラーフラグ
				bool ErrFlag;
			public:
				datum() :No(0){}
			public:
				bool operator<(const datum& My_)const{ return No<My_.No; }
				bool operator==(const datum& My_)const{ return No == My_.No; }
				bool operator!=(const datum& My_)const{ return No != My_.No; }
			};
			struct packet{
				typedef no_generator<packet> no_gen;
				typedef no_gen::no_type no_type;
				typedef clock::time_point time_point;
				typedef unsigned int no_type;
			public:
				//番号
				no_type No;
				//パケット時刻
				time_point Time;
				//データ
				std::vector<datum> Data;
				//エラーパケットフラグ
				bool ErrFlag;
			public:
				packet() :No(0){}
			public:
				bool operator<(const packet& My_)const{ return No<My_.No; }
				bool operator==(const packet& My_)const{ return No == My_.No; }
			};
		}
	}
}
#
#endif
