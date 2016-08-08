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
#include <hmLib/comgate.hpp>
#include <hmLib/gate.hpp>
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
	//��M�������Ă��邩�ǂ����̊m�F
	virtual bool flowing()override{ return pGate->flowing(); }
	//��M�\���̊m�F
	virtual bool can_getc()override{return pGate->can_getc();}
	//����byte��M�@��M�����A�h���X�ƁA��M�������������@���ۂ̎�M���������߂�l
	virtual size_type gets(char* str_,const size_type& size_)override{return pGate->gets(str_,size_);}
	// �P���o�C�g��M
	virtual char getc()override{ return pGate->getc(); }

	//���M�\��Ԃ��̊m�F
	virtual bool can_putc()override{return pGate->can_putc();}
	//������著��
	virtual void flush()override{return pGate->flush();}
	//����byte���M�@���M�����A�h���X�ƁA���M�������������@���ۂ̑��M���������߂�l
	virtual size_type puts(const char* str_,const size_type& size_)override{return pGate->puts(str_,size_);}
	// �P�o�C�g���M
	virtual void putc(char c)override{ return pGate->putc(c); }

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
	//��M�\�f�[�^�̗L��
	virtual bool can_getc()override{return FomaGate.can_getc();}
	//��M�������Ă��邩�ۂ�
	virtual bool flowing()override{return FomaGate.flowing();}
	//����byte��M�@��M�����A�h���X�ƁA��M�������������@���ۂ̎�M���������߂�l
	virtual size_type gets(char* str_,const size_type& size_)override{return FomaGate.gets(str_,size_);}
	// �P�o�C�g��M
	virtual char getc()override{ return FomaGate.getc(); }

	//���M�\�f�[�^�����邩�̊m�F
	virtual bool can_putc()override{return FomaGate.can_putc();}
	// ������著��
	virtual void flush()override{return FomaGate.flush();}
	//����byte���M�@���M�����A�h���X�ƁA���M�������������@���ۂ̑��M���������߂�l
	virtual size_type puts(const char* str_,const size_type& size_)override{return FomaGate.puts(str_,size_);}
	// �P�o�C�g���M
	virtual void putc(char c)override{ return FomaGate.putc(c); }

};
#
#endif
