#ifndef FOMAGATE_INC
#define FOMAGATE_INC 102
#
/*===fomagate===
foma�ڑ��p�̊ȈՃQ�[�g�Afomagate�ƁA
com���g����foma�ڑ��p�Q�[�g�Afomacom_gate

fomagate:v1_02/130324 hmIto
	���O��foma_gate����fomagate�ɕύX
fomagate:v1_01/130310 hmIto
	hmLib_v3_04�ɍX�V
	fomacom_gate�ǉ�
*/
#include <string>
#include <Windows.h>
#include <hmLib_v3_05/comgate.hpp>
#include <hmLib_v3_05/gate.hpp>
class fomagate:public hmLib::gate{
private:
	hmLib::gate* pGate;
public:
	fomagate():pGate(0){}
	bool open(hmLib::gate& rGate_,const std::string& foma_number){
		if(is_open())return true;

		pGate=&rGate_;

		hmLib::ogatestream gout(*pGate);

		/*����̓d�b�ԍ��֔��M*/
		gout<<"ATD"<<foma_number<<static_cast<unsigned char>(0x0d)<<static_cast<unsigned char>(0x0a);

		return false;
	}
	bool close(){
		if(!is_open())return true;

		hmLib::gatestream gout(*pGate);

		Sleep(1000);

		gout<<"+++"<<static_cast<unsigned char>(0x0d)<<static_cast<unsigned char>(0x0a);

		Sleep(1000);

		gout<<"ATH"<<static_cast<unsigned char>(0x0d)<<static_cast<unsigned char>(0x0a);

		pGate=0;

		return false;
	}
	bool is_open(){return pGate!=0;}
public:
	//��M�\��Ԃ��̊m�F
	virtual bool can_get(){return pGate->can_get();}
	//��M�\�f�[�^�̗L��
	virtual bool empty(){return pGate->empty();}
	//����byte��M�@��M�����A�h���X�ƁA��M�������������@���ۂ̎�M���������߂�l
	virtual size_type get(char* str_,const size_type& size_){return pGate->get(str_,size_);}
	//���M�\��Ԃ��̊m�F
	virtual bool can_put(){return pGate->can_put();}
	//���M�\�f�[�^�̗L��
	virtual bool full(){return pGate->full();}
	//����byte���M�@���M�����A�h���X�ƁA���M�������������@���ۂ̑��M���������߂�l
	virtual size_type put(const char* str_,const size_type& size_){return pGate->put(str_,size_);}
};
class fomacom_gate:public hmLib::gate{
private:
	hmLib::comgate ComGate;
	fomagate FomaGate;
public:
	bool open(int PortNo_,int bps_,const std::string& FomaNumber_){
		if(is_open())return true;

		if(ComGate.open(PortNo_,bps_))return true;
		
		if(FomaGate.open(ComGate,FomaNumber_)){
			ComGate.close();
			return true;
		}

		return false;
	}
	bool close(){
		if(!is_open())return true;

		FomaGate.close();

		ComGate.close();

		return false;
	}
	bool is_open(){return FomaGate.is_open();}
public:
	//��M�\��Ԃ��̊m�F
	virtual bool can_get(){return FomaGate.can_get();}
	//��M�\�f�[�^�̗L��
	virtual bool empty(){return FomaGate.empty();}
	//����byte��M�@��M�����A�h���X�ƁA��M�������������@���ۂ̎�M���������߂�l
	virtual size_type get(char* str_,const size_type& size_){return FomaGate.get(str_,size_);}
	//���M�\��Ԃ��̊m�F
	virtual bool can_put(){return FomaGate.can_put();}
	//���M�\�f�[�^�̗L��
	virtual bool full(){return FomaGate.full();}
	//����byte���M�@���M�����A�h���X�ƁA���M�������������@���ۂ̑��M���������߂�l
	virtual size_type put(const char* str_,const size_type& size_){return FomaGate.put(str_,size_);}
};
#
#endif
