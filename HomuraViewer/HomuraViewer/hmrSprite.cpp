#ifndef HMR_SPRITE_CPP_INC
#define HMR_SPRITE_CPP_INC 102
#
#include<hmLib/config_vc.h>
#ifndef HMR_SPRITE_INC
#	include"hmrSprite.hpp"
#endif
/*
bool hmr::cSpriteMsgAgent::listen(datum::time_point Time_,bool Err_,const std::string& Str_){
	//�J�����̃X�e�[�^�X�擾
	unsigned char c=static_cast<unsigned char>(Str_[0]);

	//�J�����G���[
	if(c==0xff){
		//����������Ȃ��ꍇ�A0xff�i�[
		if(Str_.size()<3){
			ErrStatus=0xff;
			ErrCode_r=0xff;
			return true;
		}
				
		//�G���[�f�[�^���i�[
		ErrStatus=static_cast<unsigned char>(Str_[1]);
		ErrCode_r=static_cast<unsigned char>(Str_[2]);

		//�G���[���ł��邱�Ƃ��o���Ă���
		IsErr_r=true;

		return false;
	}//�J���������I����
	else if(c==0xf0){
		//�p���[���Z�b�g�v���t���O�����Z�b�g
		DoPowerReset=false;
			
		//�G���[���ł��邱�Ƃ��o���Ă���
		IsErr_r=true;

		return false;
	}
	//�J��������B�e�J�n���m�F
	//  �ʐ^�T�C�Y�ƃf�[�^�T�C�Y�����o��
	// �@auto reset �Ƃ�����Ă��܂��ƁA�J�����̏�Ԃ�������Ȃ��̂ŁA�Ƃ肠�������̃R�}���h�ŃJ�����̌��ݏ󋵂�K���B�e��Ԃɂ��Ă������Ƃɂ���
	else if(c==0x00){
		// �f�[�^������Ȃ���
		if(Str_.size()<4){
			return true;
		}

		// �ʐ^�T�C�Y�ƃf�[�^�T�C�Y�����o��
		PictureSize_r = Str_.at(1);
		DataSize=static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(2))) + (static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(3))<<8));

		//DataStr���N���A
		DataStr.assign(DataSize,0x00);
		
		//Phase �ݒ�
		Phase = 0x10;//�B�e�v�����

		return false;

	}//�J��������B�e���f�[�^������ꂽ�Ƃ�
	else if(c==0x01){
		//�f�[�^������Ȃ���
		if(Str_.size()<3)return true;

		//���ۂɃJ��������f�[�^��҂��Ă�����
		// �ύX�̉\�������邽�߁A�����͕ۗ�����(2012.1.27)
		// dataPos ���A�����ł͂���܂łɃQ�b�g�����f�[�^���ɂȂ��Ă��邪�A�������ǂ����邩�H�H
		if(Phase==0x10 || Phase == 0x20){
			//�f�[�^�T�C�Y
			DataPos=static_cast<unsigned int>(static_cast<unsigned char>(Str_[1]))+(static_cast<unsigned int>(static_cast<unsigned char>(Str_[2]))<<8);
			unsigned int DataBegin=DataPos-(Str_.size()-3);

			//�f�[�^��������
			for(unsigned int i=0;i<Str_.size()-3;++i){
				if(DataSize<=DataBegin+i)break;
				DataStr.at(DataBegin+i)=Str_.at(i+3);
			}

			Phase = 0x20;// data �擾��

			//�S�Ẵf�[�^����M�����Ƃ�
			if(DataSize<=DataPos){
				Time=Time_;
				signal_setPicture(DataStr,Time);
				Phase=0x30;// �B�e����
			}
			return false;
		}//�v�����ĂȂ���
		else{
			Phase=0x00;
			return false;
		}
	}//�R�}���h���Z�b�g�v��������
	else if(c == 0x60){
		DoCommandReset=false;
		Phase=0x00;
		return false;
	}//���Z�b�g�v��������
	else if(c == 0x70){
		DoPowerReset=false;
		Phase=0x00;
		return false;
	}
	//�I�[�g���Z�b�g�ύX��
	else if( (c & 0xFE) == 0x80 ){
		AutoPowerResetFlagirl.set_pic(c == 0x80); 	
		return false;
	}
	//���C�g�_����ԕύX
	else if( (c & 0xFE) == 0x30 ){
		LightFlagirl.set_pic(c == 0x30);
		return false;
	}
	//�I�[�g���C�g(�����t���b�V��)�ύX
	else if( (c & 0xFE) == 0x40 ){
		AutoLightFlagirl.set_pic(c == 0x40);
		return false;
	}
	//�~�j�p�P�b�g���M���[�h�ύX
	else if( (c & 0xFE) == 0x50 ){
		MiniPacketModeFlagirl.set_pic(c == 0x50);
		return false;
	}
	// �A���B�e���[�h�ύX
	else if( (c & 0xFE) == 0x10 ){
		AutoTakePicFlagirl.set_pic(c == 0x10);
		return false;
	}
	// �������X�V
	else if( c == 0x90){
		// �f�[�^������Ȃ���
		if(Str_.size()<6){
			return true;
		}
		// �e��f�[�^�擾�A�X�V
		Status = Str_.at(1);								//status
		IsErr_r= (Str_.at(2) != 0);						// �G���[���ǂ���
		ErrCode_r = Str_.at(3);							// �G���[�X�e�[�^�X
		PictureSize_r = Str_.at(4);						//�@�J�������̉摜�T�C�Y
		AutoPowerResetFlagirl.set_pic(Str_.at(5) != 0); // �I�[�g���Z�b�g���[�h���ǂ���

		return false;
	}
	return true;
}
void hmr::cSpriteMsgAgent::setup_talk(void){
	AutoPowerResetFlagirl.setup_talk();
	AutoLightFlagirl.setup_talk();
	LightFlagirl.setup_talk();
	AutoTakePicFlagirl.setup_talk();
	MiniPacketModeFlagirl.setup_talk();
}
void hmr::cSpriteMsgAgent::setup_listen(void){
	ErrCode_r=0x00;
	ErrStatus=0x00;
}
bool hmr::cSpriteMsgAgent::talk(std::string& Str){
	Str="";
	//PowerReset�v����
	if(DoPowerReset){
		DoPowerReset=false;
		Str.push_back(static_cast<unsigned char>(0x70));
		return false;
	}//AutoPowerReset�ݒ�ύX��
	else if(AutoPowerResetFlagirl.talk()){
		if(AutoPowerResetFlagirl.request())Str.push_back(static_cast<unsigned char>(0x80));
		else Str.push_back(static_cast<unsigned char>(0x81));
		return false;
	}//CommandReset�v����
	else if(DoCommandReset){
		DoCommandReset=false;
		Str.push_back(static_cast<unsigned char>(0x60));
		return false;
	}//�I�[�g���C�g�ݒ莞
	else if(AutoLightFlagirl.talk()){
		if(AutoLightFlagirl.request())Str.push_back(static_cast<unsigned char>(0x40));
		else Str.push_back(static_cast<unsigned char>(0x41));
		return false;
	}//���C�g�̐ݒ�
	else if(LightFlagirl.talk()){
		if(LightFlagirl.request())Str.push_back(static_cast<unsigned char>(0x30));
		else Str.push_back(static_cast<unsigned char>(0x31));
		return false;
	}//MIni�p�P�b�g���[�h�̐ݒ�
	else if(MiniPacketModeFlagirl.talk()){
		if(MiniPacketModeFlagirl.request())Str.push_back(static_cast<unsigned char>(0x50));
		else Str.push_back(static_cast<unsigned char>(0x51));
		return false;
	}// �A���B�e���[�h�̐ݒ�
	else if(AutoTakePicFlagirl.talk()){
		if(AutoTakePicFlagirl.request()){
			Str.push_back(static_cast<unsigned char>(0x10));
			Str.push_back(static_cast<unsigned char>(0x00));// Picture �T�C�Y�͈�ԏ���������
		}
		else Str.push_back(static_cast<unsigned char>(0x11));
		return false;
	}
	//�ʐ^�v�����[�h�ɂȂ��Ă���
	else if(DoSetTakePicture){
		DoSetTakePicture=false;
		//PictureSize��K���l�ɕ␳
		if(PictureSize<0)PictureSize=0;
		else if(PictureSize>2)PictureSize=2;
		//�R�}���h0x00���M
		Str.push_back(static_cast<unsigned char>(0x00));
		Str.push_back(PictureSize);
		return false;
	}
	// �������v���R�}���h���������ꍇ
	else if(DoGetInformation){
		DoGetInformation=false;
		// �R�}���h���M
		Str.push_back(static_cast<unsigned char>(0x90));
		return false;
	}else{
		return true;
	}
}
*/
bool hmr::cSpriteMsgAgent::listen(datum::time_point Time_,bool Err_,const std::string& Str_){
	//�J�����̃X�e�[�^�X�擾
	unsigned char c=static_cast<unsigned char>(Str_[0]);

	//�J��������B�e�J�n���m�F
	//  �ʐ^�T�C�Y�ƃf�[�^�T�C�Y�����o��
	// �@auto reset �Ƃ�����Ă��܂��ƁA�J�����̏�Ԃ�������Ȃ��̂ŁA�Ƃ肠�������̃R�}���h�ŃJ�����̌��ݏ󋵂�K���B�e��Ԃɂ��Ă������Ƃɂ���
	if(c==0x00){
		// �f�[�^������Ȃ���
		if(Str_.size()<4){
			return true;
		}

		// �ʐ^�T�C�Y�ƃf�[�^�T�C�Y�����o��
		IsData=true;
		DataPos=0;
		DataSize=static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(2))) + (static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(3))<<8));

		//DataStr���N���A
		DataStr.assign(DataSize,0x00);
		
		return false;

	}//�J��������B�e���f�[�^������ꂽ�Ƃ�
	else if(c==0x01){
		//�f�[�^������Ȃ���
		if(Str_.size()<3)return true;

		//���ۂɃJ��������f�[�^��҂��Ă�����
		// �ύX�̉\�������邽�߁A�����͕ۗ�����(2012.1.27)
		// dataPos ���A�����ł͂���܂łɃQ�b�g�����f�[�^���ɂȂ��Ă��邪�A�������ǂ����邩�H�H
		if(IsData){
			//�f�[�^�T�C�Y
			DataPos=static_cast<unsigned int>(static_cast<unsigned char>(Str_[1]))+(static_cast<unsigned int>(static_cast<unsigned char>(Str_[2]))<<8);
//			unsigned int DataBegin=DataPos-(Str_.size()-3);
			unsigned int DataBegin=DataPos;

			//�f�[�^��������
			for(unsigned int i=0;i<Str_.size()-3;++i){
				if(DataSize<=DataBegin+i)break;
				DataStr.at(DataBegin+i)=Str_.at(i+3);
			}

			//�S�Ẵf�[�^����M�����Ƃ�
//			if(DataSize<=DataPos){
			if(DataSize<=DataBegin+Str_.size()-3){
				DataTime=Time_;
				signal_setPicture(DataStr,DataTime);
				IsData=false;
			}
			return false;
		}//�v�����ĂȂ���
		else{
			return false;
		}
	}//�R�}���h���Z�b�g�v��������
	else if(c == 0x60){
		DoCommandReset=false;
		return false;
	}//���Z�b�g�v��������
	else if(c == 0x70){
		DoPowerReset=false;
		return false;
	}
	//�I�[�g���Z�b�g�ύX��
	else if( (c & 0xFE) == 0x80 ){
		AutoPowerResetFlagirl.set_pic(c == 0x80); 	
		return false;
	}
	//���C�g�_����ԕύX
	else if( (c & 0xFE) == 0x30 ){
		LightFlagirl.set_pic(c == 0x30);
		return false;
	}
	//�I�[�g���C�g(�����t���b�V��)�ύX
	else if( (c & 0xFE) == 0x40 ){
		AutoLightFlagirl.set_pic(c == 0x40);
		return false;
	}
	//�~�j�p�P�b�g���M���[�h�ύX
	else if( (c & 0xFE) == 0x50 ){
		MiniPacketModeFlagirl.set_pic(c == 0x50);
		return false;
	}
	// �A���B�e���[�h�ύX
	else if( (c & 0xFE) == 0x10 ){
		AutoTakePicFlagirl.set_pic(c == 0x10);
		return false;
	}
	// �������X�V
	else if( c == 0xA0){
		// �f�[�^������Ȃ���
		if(Str_.size()<4){
			return true;
		}

		Status=static_cast<unsigned char>(Str_[1]);
		IsErr=(static_cast<unsigned char>(Str_[2])!=0);
		ErrCode=static_cast<unsigned char>(Str_[3]);
		StatusTime=Time_;

		return false;
	}
	// ������񑗐M���[�h�ύX
	else if( (c & 0xFE) == 0xB0){
		InfoModeFlagirl.set_pic(c == 0xB0);
		return false;
	}
	// log Mode
	else if (c == 0xC0){
		if (Str_.size() != 1) return true;
		LogModeFlagirl.set_pic(true);
	}
	else if (c == 0xC1){
		if (Str_.size() != 1) return true;
		LogModeFlagirl.set_pic(false);
	}

	// log Data ��M�t�F�[�Y
	//�J��������B�e�J�n���m�F
	//  �ʐ^�T�C�Y�ƃf�[�^�T�C�Y�����o��
	// �@auto reset �Ƃ�����Ă��܂��ƁA�J�����̏�Ԃ�������Ȃ��̂ŁA�Ƃ肠�������̃R�}���h�ŃJ�����̌��ݏ󋵂�K���B�e��Ԃɂ��Ă������Ƃɂ���
	if (c == 0xD0){
		// �f�[�^������Ȃ���
		if (Str_.size()<8){
			return true;
		}

		// �ʐ^�T�C�Y�ƃf�[�^�T�C�Y�����o��
		log_IsData = true;
		log_DataPos = 0;
		log_DataSize = static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(2))) + (static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(3)) << 8));

		//DataStr���N���A
		log_DataStr.assign(DataSize, 0x00);

		// ���ݎ����擾
		hmLib_sint32 TimeSec = static_cast<hmLib_sint32>(
			static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(4))) 
			+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(5))) * 256
			+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(6))) * 256 * 256 
			+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(7))) * 256 * 256 * 256
		);

		// time_point �ɕϊ�
		log_DataTime = std::chrono::system_clock::from_time_t(TimeSec);

		return false;

	}//�J��������B�e���f�[�^������ꂽ�Ƃ�
	else if (c == 0xD1){
		//�f�[�^������Ȃ���
		if (Str_.size()<3)return true;

		//���ۂɃJ��������f�[�^��҂��Ă�����
		// dataPos ���A�����ł͂���܂łɃQ�b�g�����f�[�^���ɂȂ��Ă��邪�A�������ǂ����邩�H�H
		if (log_IsData){
			//�f�[�^�T�C�Y
			log_DataPos = static_cast<unsigned int>(static_cast<unsigned char>(Str_[1])) + (static_cast<unsigned int>(static_cast<unsigned char>(Str_[2])) << 8);
			//			unsigned int DataBegin=DataPos-(Str_.size()-3);
			unsigned int log_DataBegin = log_DataPos;

			//�f�[�^��������
			for (unsigned int i = 0; i<Str_.size() - 3; ++i){
				if (log_DataSize <= log_DataBegin + i)break;
				log_DataStr.at(log_DataBegin + i) = Str_.at(i + 3);
			}

			//�S�Ẵf�[�^����M�����Ƃ�
			//			if(DataSize<=DataPos){
			if (log_DataSize <= log_DataBegin + Str_.size() - 3){
				signal_setLogPicture(log_DataStr, log_DataTime);
				log_IsData = false;
			}
			return false;
		}//�v�����ĂȂ���
		else{
			return false;
		}
	}



	return true;
}
void hmr::cSpriteMsgAgent::setup_talk(void){
	AutoPowerResetFlagirl.setup_talk();
	AutoLightFlagirl.setup_talk();
	LightFlagirl.setup_talk();
	AutoTakePicFlagirl.setup_talk();
	MiniPacketModeFlagirl.setup_talk();
	InfoModeFlagirl.setup_talk();
	LogModeFlagirl.setup_talk();
}
void hmr::cSpriteMsgAgent::setup_listen(void){
	ErrCode=0x00;
	IsErr=false;
}
bool hmr::cSpriteMsgAgent::talk(std::string& Str){
	Str="";
	//PowerReset�v����
	if(DoPowerReset){
		DoPowerReset=false;
		Str.push_back(static_cast<unsigned char>(0x70));
		return false;
	}//AutoPowerReset�ݒ�ύX��
	else if(AutoPowerResetFlagirl.talk()){
		if(AutoPowerResetFlagirl.request())Str.push_back(static_cast<unsigned char>(0x80));
		else Str.push_back(static_cast<unsigned char>(0x81));
		return false;
	}//CommandReset�v����
	else if(DoCommandReset){
		DoCommandReset=false;
		Str.push_back(static_cast<unsigned char>(0x60));
		return false;
	}//�I�[�g���C�g�ݒ莞
	else if(AutoLightFlagirl.talk()){
		if(AutoLightFlagirl.request())Str.push_back(static_cast<unsigned char>(0x40));
		else Str.push_back(static_cast<unsigned char>(0x41));
		return false;
	}//���C�g�̐ݒ�
	else if(LightFlagirl.talk()){
		if(LightFlagirl.request())Str.push_back(static_cast<unsigned char>(0x30));
		else Str.push_back(static_cast<unsigned char>(0x31));
		return false;
	}//MIni�p�P�b�g���[�h�̐ݒ�
	else if(MiniPacketModeFlagirl.talk()){
		if(MiniPacketModeFlagirl.request())Str.push_back(static_cast<unsigned char>(0x50));
		else Str.push_back(static_cast<unsigned char>(0x51));
		return false;
	}// �A���B�e���[�h�̐ݒ�
	else if(AutoTakePicFlagirl.talk()){
		if(AutoTakePicFlagirl.request()){
			Str.push_back(static_cast<unsigned char>(0x10));
			Str.push_back(static_cast<unsigned char>(0x00));// Picture �T�C�Y�͈�ԏ���������
		}
		else Str.push_back(static_cast<unsigned char>(0x11));
		return false;
	}
	//�ʐ^�v�����[�h�ɂȂ��Ă���
	else if(DoSetTakePicture){
		DoSetTakePicture=false;
		//PictureSize��K���l�ɕ␳
		if(PictureSize<0)PictureSize=0;
		else if(PictureSize>2)PictureSize=2;
		//�R�}���h0x00���M
		Str.push_back(static_cast<unsigned char>(0x00));
		Str.push_back(PictureSize);
		return false;
	}
	// ������񑗐M���[�h�̐ݒ�
	else if(InfoModeFlagirl.talk()){
		if(InfoModeFlagirl.request())Str.push_back(static_cast<unsigned char>(0xB0));
		else Str.push_back(static_cast<unsigned char>(0xB1));
		return false;
	}
	// log mode�̏���
	else if (LogModeFlagirl.talk()){
		if (LogModeFlagirl.request()) Str.push_back(static_cast<unsigned char>(0xC0));
		else Str.push_back(static_cast<unsigned char>(0xC1));
		return false;
	}
	else{
		return true;
	}
}
#
#endif
