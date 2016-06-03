#ifndef HMRPAD_HPP
#define HMRPAD_HPP 100

#include "hmLibVer.hpp"
#include <hmLib_v2/dxPad.hpp>
#include <map>
#include "hmr.h"
#define D_PAD_1 4
#define D_PAD_2 5
#define D_PAD_3 6
#define D_PAD_4 7
#define D_PAD_5 8
#define D_PAD_6 9
#define D_PAD_7 10
#define D_PAD_8 11
#define D_PAD_9 12
#define D_PAD_10 13
#define D_PAD_13 16
#define D_PAD_14 17
#define D_PAD_15 18
#define D_PAD_16 19
class hmrPad{
public:
	class abtPadCtr{
	public:
		//�{�^���̏󋵂ɉ����ď���
		virtual void work(int sw)=0;
		friend class hmrPad;
	};
	class dxoPadWarn:public dxReObject,public hmoBox{
	private:
	protected:
		virtual int redraw(dxO& dxo){
			if(GetJoypadNum()){
				if(dxo.draw(Pint(0,0),dxoButIO(size,"Use Pad",CLR::SoftGreen,true))){
					dxPad::connect();
				}
			}else{
				if(dxo.draw(Pint(0,0),dxoBut(size,"No Pad",CLR::SoftRed))<0){
					dxPad::connect();
				}
			}
			return 0;
		}
	public:
		dxoPadWarn(Pint _size):hmoBox(_size){return;}
	};
	typedef std::multimap<int,abtPadCtr*> PadMap;
	typedef std::pair<int,abtPadCtr*> pairPadCtr;
private:
//	dxPad Pad;
	DINPUT_JOYSTATE Pad;//���݂�PadData
	DINPUT_JOYSTATE bPad;//�ȑO��PadData
public:
	//�X�e�B�b�N�p
	abtPadCtr* StickL_X;
	abtPadCtr* StickL_Y;
	abtPadCtr* StickR_X;
	abtPadCtr* StickR_Y;
	//�{�^���p
	PadMap Buts;
public:
	hmrPad(){
		StickL_X=0;
		StickL_Y=0;
		StickR_X=0;
		StickR_Y=0;
		//PadData������
		for(int i=0;i<32;i++){
			Pad.Buttons[i] = 0;
			bPad.Buttons[i] = 0;
		}
	}
	void work(){
		//If there is no Pad, return.
		if(!is_connect())return;

		//Pad.work();
		//�X�e�B�b�N�f�[�^�X�V
		GetJoypadDirectInputState( DX_INPUT_PAD1, &Pad);

		//�X�e�B�b�N����
		if(StickL_X!=0)StickL_X->work(Pad.X);
		if(StickL_Y!=0)StickL_Y->work(-Pad.Y);
		if(StickR_X!=0)StickR_X->work(-Pad.Rz);
		if(StickR_Y!=0)StickR_Y->work(-Pad.Z);

		//�{�^������
		PadMap::iterator itr=Buts.begin(),citr=Buts.begin(),enditr=Buts.end();
		for(;itr!=enditr;++itr){
			//Button �̏�������
			//	�͂��߂ĉ����ꂽ 1
			//	2��A���ŉ����ꂽ�@2
			//	�O�񉟂��ꂽ�����񉟂���Ă��Ȃ��@-1
			//	�O��������������Ă��Ȃ��@0
			int Button = (Pad.Buttons[itr->first] != 0)*2 + (Pad.Buttons[itr->first] == bPad.Buttons[itr->first]) - 1;
			itr->second->work(Button);
		}
		//����̃{�^���o���Ƃ�
		bPad = Pad;
	}
public:
	static void connect(){dxPad::connect();}
	static bool is_connect(){return GetJoypadNum()>0;}
};
/*
class hmrPad{
public:
	class abtPadCtr{
	private:
		//�{�^���̏󋵂ɉ����ď���
		virtual void work(int sw)=0;
		friend class hmrPad;
	};
	class dxoPadWarn:public dxReObject,public hmoBox{
	private:
	protected:
		virtual int redraw(dxO& dxo){
			if(GetJoypadNum()){
				if(dxo.draw(Pint(0,0),dxoButIO(size,"Use Pad",CLR::SoftGreen,true))){
					dxPad::connect();
				}
			}else{
				if(dxo.draw(Pint(0,0),dxoBut(size,"No Pad",CLR::SoftRed))<0){
					dxPad::connect();
				}
			}
			return 0;
		}
	public:
		dxoPadWarn(Pint _size):hmoBox(_size){return;}
	};
	typedef std::multimap<int,abtPadCtr*> PadMap;
	typedef std::pair<int,abtPadCtr*> pairPadCtr;
private:
	dxPad Pad;
public:
	//�X�e�B�b�N�p
	abtPadCtr* StickL;
	abtPadCtr* StickR;
	//�{�^���p
	PadMap Buts;
public:
	hmrPad():Pad(DX_INPUT_PAD1){
		StickL=0;
		StickR=0;
	}
	void work(){
		Pad.work();
		int Cnt=0;

		//�X�e�B�b�N����
		if(StickL!=0)StickL->work(-Pad.getAnlgL().y);
		if(StickR!=0)StickR->work(-Pad.getAnlgR().y);

		//�{�^������
		PadMap::iterator itr=Buts.begin(),citr=Buts.begin(),enditr=Buts.end();
		for(;itr!=enditr;++itr){
			while(itr==citr){
				++Cnt;
				citr=Buts.upper_bound(Cnt);
			}
			itr->second->work(Pad.getBut(Cnt+3));
		}
	}
};
*/
#endif
