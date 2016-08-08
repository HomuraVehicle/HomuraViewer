/*===========hmrV2500.cpp===========
HomuraViewer v1_07/160808
	a
		�قނ�̃f�B���N�g���\������V

hmrV2500 v1_06/160601
	a
		Git�ړ��㏉�߂ăR���p�C���ɐ���
	c
		HomuraViewer���� Data, File, VMC1���ړ�
	d
		�f�B���N�g���\����ύX�@Data�f�B���N�g�������ɂ��ׂẴf�[�^���o��悤�ɏC��
		���킹�āA���x�v�̎��Ԗ��X�V�o�O���X�V
		�o�̓t�@�C����tsv�`���ɍX�V
		Date���o�͂���悤�ɂ���
	e
		AcceleLogger�̊p�x���ߕq�ɔ�����������C��
	f
		FullADC�@�\������
	g
		Add foma settingh function
	h
		�V�����f�B���N�g���\���Ɉڍs
hmrV2500 v1_05/130914
	p
		�\���ʒu���C��
	n
		�R���p�X�ɕ����␳�@�\��ǉ�
	h
		Operator���f�o�b�O
	g
		�t�@�C���@�\����x�O��
		����d�ʐM�ւ̑Ή�
	�t�@�C���@�\����������
hmrV2500 v1_04/130803
	�t�@�C���@�\�������J�n

hmrV2500 v1_03/130713 
	�����ł��Ă��邱��
	�@�E��{�I�ȋ@�\�̎���
	�@
*/

#include"hmLibVer.hpp"
#include<hmLib/config_vc.h> // std::min, std::max�@�́@windows�}�N���ł���`����Ă���A�����I�ɕ������Ă��Ȃ��Ƃ���

#include<hmLib/bufgate.hpp>
#include<hmLib/any_iterator.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include"predicate.hpp"



#include"Message/DxCom.hpp"
#include"IO/DxIO_v2.hpp"
#include"IO/DxGateSwitcher.hpp"

#include"IO.hpp"
#include"Message.hpp"

#include"Thermo.hpp"
#include"Camera.hpp"
#include"CO2.hpp"
#include"FullADC.hpp"

#include"Motor.hpp"
#include"Accele.hpp"
#include"Compass.hpp"
#include"Gyro.hpp"

#include"GPS.hpp"
#include"Battery.hpp"
#include"UniSensor.hpp"

#include"DeviceManage.hpp"
#include"File.hpp"

#include"View.hpp"
#include"Controller.hpp"

#include"Resource.hpp"

#include<hmLib_v2/hmLib.cpp>

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){
	namespace hmrv = hmr::viewer;

	SetWindowIconID(ID_ICON);

	//hmLib_dx���W���[����������
	dx::ini("hmrV2500_v1_07a", 960,720);

	try{
		//Message
		hmrv::cMessage Message;


		//IO
		//	PC����̏o�����Ǘ�����@
		hmrv::cIO IO(Message.ComVMC,Message.MessageDriver);

		//�e���W���[���錾		
		hmrv::cBattery Battery;
		hmrv::cFullADC FullADC;
		hmrv::cAccele Accele;
		hmrv::cCompass Compass;
		hmrv::cGyro Gyro;
		hmrv::cGPS GPS;
		hmrv::cMotor Motor;
		hmrv::cThermo Thermo;
		hmrv::cCO2 CO2;
		hmrv::cCamera Camera;
		hmrv::cDeviceManage DeviceManage;

		Message.regist('b', Battery.MsgAgent);
		Message.regist('f', FullADC.MsgAgent);
		Message.regist('a', Accele.MsgAgent);
		Message.regist('c', Compass.MsgAgent);
		Message.regist('G', Gyro.MsgAgent);
		Message.regist('g', GPS.MsgAgent);
		Message.regist('m', Motor.MsgAgent);
		Message.regist('t', Thermo.MsgAgent);
		Message.regist('C', CO2.MsgAgent);
		Message.regist('j', Camera.MsgAgent);
		Message.regist('D', DeviceManage.MsgAgent);

		//����n�f�o�C�X
		hmrv::cController Controller;
		Controller.connect_Pad(Motor.MsgAgent);
		Controller.connect_Pad(Battery.MsgAgent);
		Controller.connect_Pad(Accele.MsgAgent);
		Controller.connect_Pad(Compass.MsgAgent);
		Controller.connect_Pad(Gyro.MsgAgent);
		Controller.connect_Pad(GPS.MsgAgent);
		Controller.connect_Pad(Thermo.MsgAgent);
		Controller.connect_Pad(Camera.MsgAgent);
		Controller.connect_Pad(CO2.MsgAgent);

		//�e�f�B���N�g��
		hmrv::cFile File;

		//Battery�f�[�^�ۑ�
		File.regist(Battery.FileAgent);

		//FullADC�f�[�^�ۑ�
		File.regist(FullADC.FileAgent);

		// Thermo �f�[�^��ۑ�
		File.regist(Thermo.FileAgent);

		// CO2 �f�[�^��ۑ�
		File.regist(CO2.FileAgent);

		//GPS�f�[�^��ۑ�
		File.regist(GPS.FileAgent);

		//GPGGA�f�[�^��ۑ�
		File.regist(GPS.GPGGAFileAgent);

		//�J�����f�[�^��ۑ�
		File.regist(Camera.FileAgent);

		//��ʕ\�����W���[��
		hmrv::cView View(IO,Message, FullADC, Accele,Compass,Gyro,Camera,GPS,Battery);

		View.registSUI(DeviceManage.SUI);
		View.registSUI(IO.GateSwSUI);
		View.registSUI(IO.BufGateSUI);
		View.registSUI(IO.ioLogGateSUI);
		View.registSUI(IO.IODriverSUI);
		View.registSUI(IO.VMCSUI);
		View.registSUI(File.SUI);

		View.registMUI(Motor.MUI);
		View.registMUI(Battery.MUI);
		View.registMUI(Accele.MUI);
		View.registMUI(Compass.MUI);
		View.registMUI(Gyro.MUI);
		View.registMUI(GPS.MUI);
		View.registMUI(Thermo.MUI);
		View.registMUI(Camera.MUI);
		View.registMUI(CO2.MUI);
		View.registMUI(FullADC.MUI);

		Controller.connect_Keyboard(View);

		while(!dx::work(30)){
			Controller();
			IO();
			Message();
			View();

			if(dx::getKey(KEY::ESC)<0)break;
		}
	}catch(const std::exception& Escp){
		printfDx("===ERROR===\n");
		printfDx("%s\n",Escp.what());
		while(!dx::work(30)){
			if(dx::getKey(KEY::ESC)<0)break;
		}
	}catch(const std::string& Escp){
		printfDx("===ERROR===\n");
		printfDx("%s\n",Escp.c_str());
		while(!dx::work(30)){
			if(dx::getKey(KEY::ESC)<0)break;
		}
	}

	dx::end();
	
	return 0;

}
