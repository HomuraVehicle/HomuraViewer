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
�f�[�^�^�̊Ǘ��p�t�@�C��

hmrData:v1_03/130615 hmIto
	Datum�n�b�V��,Packet�n�b�V����ǉ�
hmrData v1_02/130410 hmIto
	packet::Time�𕜊�
	packet/datum�ɁA���Ԃɂ��operator<��ǉ�
hmrData:v1_01/130324 hmIto
	packet::Time���폜
hmrData:v1_00/130310 hmIto
	datum,packet��ǉ�
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
				//�f�[�^�ԍ�
				no_type No;
				//�f�[�^����
				time_point Time;
				//�f�[�^ID
				id_type ID;
				// �f�[�^��
				size_type Size;
				//������
				//	���ӁF�����ɂ͏����ȃR�}���h�̂ݓ���B�h�c�C�r�������̃f�[�^�͊܂܂�Ȃ�
				std::string Str;
				//�G���[�t���O
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
				//�ԍ�
				no_type No;
				//�p�P�b�g����
				time_point Time;
				//�f�[�^
				std::vector<datum> Data;
				//�G���[�p�P�b�g�t���O
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
