#ifndef HMR_SPRITE_CPP_INC
#define HMR_SPRITE_CPP_INC 102
#
#include<hmLib/config_vc.h>
#ifndef HMR_SPRITE_INC
#	include"hmrSprite.hpp"
#endif
/*
bool hmr::cSpriteMsgAgent::listen(datum::time_point Time_,bool Err_,const std::string& Str_){
	//カメラのステータス取得
	unsigned char c=static_cast<unsigned char>(Str_[0]);

	//カメラエラー
	if(c==0xff){
		//長さが足りない場合、0xff格納
		if(Str_.size()<3){
			ErrStatus=0xff;
			ErrCode_r=0xff;
			return true;
		}
				
		//エラーデータを格納
		ErrStatus=static_cast<unsigned char>(Str_[1]);
		ErrCode_r=static_cast<unsigned char>(Str_[2]);

		//エラー中であることを覚えておく
		IsErr_r=true;

		return false;
	}//カメラ強制終了中
	else if(c==0xf0){
		//パワーリセット要求フラグをリセット
		DoPowerReset=false;
			
		//エラー中であることを覚えておく
		IsErr_r=true;

		return false;
	}
	//カメラから撮影開始を確認
	//  写真サイズとデータサイズを取り出す
	// 　auto reset とかされてしまうと、カメラの状態が分からないので、とりあえずこのコマンドでカメラの現在状況を必ず撮影状態にしておくことにする
	else if(c==0x00){
		// データが足りない時
		if(Str_.size()<4){
			return true;
		}

		// 写真サイズとデータサイズを取り出す
		PictureSize_r = Str_.at(1);
		DataSize=static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(2))) + (static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(3))<<8));

		//DataStrをクリア
		DataStr.assign(DataSize,0x00);
		
		//Phase 設定
		Phase = 0x10;//撮影要求状態

		return false;

	}//カメラから撮影中データが得られたとき
	else if(c==0x01){
		//データが足りない時
		if(Str_.size()<3)return true;

		//実際にカメラからデータを待っていた時
		// 変更の可能性があるため、ここは保留する(2012.1.27)
		// dataPos が、ここではこれまでにゲットしたデータ数になっているが、ここをどうするか？？
		if(Phase==0x10 || Phase == 0x20){
			//データサイズ
			DataPos=static_cast<unsigned int>(static_cast<unsigned char>(Str_[1]))+(static_cast<unsigned int>(static_cast<unsigned char>(Str_[2]))<<8);
			unsigned int DataBegin=DataPos-(Str_.size()-3);

			//データ書き込み
			for(unsigned int i=0;i<Str_.size()-3;++i){
				if(DataSize<=DataBegin+i)break;
				DataStr.at(DataBegin+i)=Str_.at(i+3);
			}

			Phase = 0x20;// data 取得中

			//全てのデータを受信したとき
			if(DataSize<=DataPos){
				Time=Time_;
				signal_setPicture(DataStr,Time);
				Phase=0x30;// 撮影完了
			}
			return false;
		}//要求してない時
		else{
			Phase=0x00;
			return false;
		}
	}//コマンドリセット要求成功時
	else if(c == 0x60){
		DoCommandReset=false;
		Phase=0x00;
		return false;
	}//リセット要求成功時
	else if(c == 0x70){
		DoPowerReset=false;
		Phase=0x00;
		return false;
	}
	//オートリセット変更時
	else if( (c & 0xFE) == 0x80 ){
		AutoPowerResetFlagirl.set_pic(c == 0x80); 	
		return false;
	}
	//ライト点灯状態変更
	else if( (c & 0xFE) == 0x30 ){
		LightFlagirl.set_pic(c == 0x30);
		return false;
	}
	//オートライト(自動フラッシュ)変更
	else if( (c & 0xFE) == 0x40 ){
		AutoLightFlagirl.set_pic(c == 0x40);
		return false;
	}
	//ミニパケット送信モード変更
	else if( (c & 0xFE) == 0x50 ){
		MiniPacketModeFlagirl.set_pic(c == 0x50);
		return false;
	}
	// 連続撮影モード変更
	else if( (c & 0xFE) == 0x10 ){
		AutoTakePicFlagirl.set_pic(c == 0x10);
		return false;
	}
	// 内部情報更新
	else if( c == 0x90){
		// データが足りない時
		if(Str_.size()<6){
			return true;
		}
		// 各種データ取得、更新
		Status = Str_.at(1);								//status
		IsErr_r= (Str_.at(2) != 0);						// エラーかどうか
		ErrCode_r = Str_.at(3);							// エラーステータス
		PictureSize_r = Str_.at(4);						//　カメラ内の画像サイズ
		AutoPowerResetFlagirl.set_pic(Str_.at(5) != 0); // オートリセットモードかどうか

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
	//PowerReset要求時
	if(DoPowerReset){
		DoPowerReset=false;
		Str.push_back(static_cast<unsigned char>(0x70));
		return false;
	}//AutoPowerReset設定変更時
	else if(AutoPowerResetFlagirl.talk()){
		if(AutoPowerResetFlagirl.request())Str.push_back(static_cast<unsigned char>(0x80));
		else Str.push_back(static_cast<unsigned char>(0x81));
		return false;
	}//CommandReset要求時
	else if(DoCommandReset){
		DoCommandReset=false;
		Str.push_back(static_cast<unsigned char>(0x60));
		return false;
	}//オートライト設定時
	else if(AutoLightFlagirl.talk()){
		if(AutoLightFlagirl.request())Str.push_back(static_cast<unsigned char>(0x40));
		else Str.push_back(static_cast<unsigned char>(0x41));
		return false;
	}//ライトの設定
	else if(LightFlagirl.talk()){
		if(LightFlagirl.request())Str.push_back(static_cast<unsigned char>(0x30));
		else Str.push_back(static_cast<unsigned char>(0x31));
		return false;
	}//MIniパケットモードの設定
	else if(MiniPacketModeFlagirl.talk()){
		if(MiniPacketModeFlagirl.request())Str.push_back(static_cast<unsigned char>(0x50));
		else Str.push_back(static_cast<unsigned char>(0x51));
		return false;
	}// 連続撮影モードの設定
	else if(AutoTakePicFlagirl.talk()){
		if(AutoTakePicFlagirl.request()){
			Str.push_back(static_cast<unsigned char>(0x10));
			Str.push_back(static_cast<unsigned char>(0x00));// Picture サイズは一番小さいもの
		}
		else Str.push_back(static_cast<unsigned char>(0x11));
		return false;
	}
	//写真要求モードになっている
	else if(DoSetTakePicture){
		DoSetTakePicture=false;
		//PictureSizeを適正値に補正
		if(PictureSize<0)PictureSize=0;
		else if(PictureSize>2)PictureSize=2;
		//コマンド0x00送信
		Str.push_back(static_cast<unsigned char>(0x00));
		Str.push_back(PictureSize);
		return false;
	}
	// 内部情報要求コマンドがあった場合
	else if(DoGetInformation){
		DoGetInformation=false;
		// コマンド送信
		Str.push_back(static_cast<unsigned char>(0x90));
		return false;
	}else{
		return true;
	}
}
*/
bool hmr::cSpriteMsgAgent::listen(datum::time_point Time_,bool Err_,const std::string& Str_){
	//カメラのステータス取得
	unsigned char c=static_cast<unsigned char>(Str_[0]);

	//カメラから撮影開始を確認
	//  写真サイズとデータサイズを取り出す
	// 　auto reset とかされてしまうと、カメラの状態が分からないので、とりあえずこのコマンドでカメラの現在状況を必ず撮影状態にしておくことにする
	if(c==0x00){
		// データが足りない時
		if(Str_.size()<4){
			return true;
		}

		// 写真サイズとデータサイズを取り出す
		IsData=true;
		DataPos=0;
		DataSize=static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(2))) + (static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(3))<<8));

		//DataStrをクリア
		DataStr.assign(DataSize,0x00);
		
		return false;

	}//カメラから撮影中データが得られたとき
	else if(c==0x01){
		//データが足りない時
		if(Str_.size()<3)return true;

		//実際にカメラからデータを待っていた時
		// 変更の可能性があるため、ここは保留する(2012.1.27)
		// dataPos が、ここではこれまでにゲットしたデータ数になっているが、ここをどうするか？？
		if(IsData){
			//データサイズ
			DataPos=static_cast<unsigned int>(static_cast<unsigned char>(Str_[1]))+(static_cast<unsigned int>(static_cast<unsigned char>(Str_[2]))<<8);
//			unsigned int DataBegin=DataPos-(Str_.size()-3);
			unsigned int DataBegin=DataPos;

			//データ書き込み
			for(unsigned int i=0;i<Str_.size()-3;++i){
				if(DataSize<=DataBegin+i)break;
				DataStr.at(DataBegin+i)=Str_.at(i+3);
			}

			//全てのデータを受信したとき
//			if(DataSize<=DataPos){
			if(DataSize<=DataBegin+Str_.size()-3){
				DataTime=Time_;
				signal_setPicture(DataStr,DataTime);
				IsData=false;
			}
			return false;
		}//要求してない時
		else{
			return false;
		}
	}//コマンドリセット要求成功時
	else if(c == 0x60){
		DoCommandReset=false;
		return false;
	}//リセット要求成功時
	else if(c == 0x70){
		DoPowerReset=false;
		return false;
	}
	//オートリセット変更時
	else if( (c & 0xFE) == 0x80 ){
		AutoPowerResetFlagirl.set_pic(c == 0x80); 	
		return false;
	}
	//ライト点灯状態変更
	else if( (c & 0xFE) == 0x30 ){
		LightFlagirl.set_pic(c == 0x30);
		return false;
	}
	//オートライト(自動フラッシュ)変更
	else if( (c & 0xFE) == 0x40 ){
		AutoLightFlagirl.set_pic(c == 0x40);
		return false;
	}
	//ミニパケット送信モード変更
	else if( (c & 0xFE) == 0x50 ){
		MiniPacketModeFlagirl.set_pic(c == 0x50);
		return false;
	}
	// 連続撮影モード変更
	else if( (c & 0xFE) == 0x10 ){
		AutoTakePicFlagirl.set_pic(c == 0x10);
		return false;
	}
	// 内部情報更新
	else if( c == 0xA0){
		// データが足りない時
		if(Str_.size()<4){
			return true;
		}

		Status=static_cast<unsigned char>(Str_[1]);
		IsErr=(static_cast<unsigned char>(Str_[2])!=0);
		ErrCode=static_cast<unsigned char>(Str_[3]);
		StatusTime=Time_;

		return false;
	}
	// 内部情報送信モード変更
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

	// log Data 受信フェーズ
	//カメラから撮影開始を確認
	//  写真サイズとデータサイズを取り出す
	// 　auto reset とかされてしまうと、カメラの状態が分からないので、とりあえずこのコマンドでカメラの現在状況を必ず撮影状態にしておくことにする
	if (c == 0xD0){
		// データが足りない時
		if (Str_.size()<8){
			return true;
		}

		// 写真サイズとデータサイズを取り出す
		log_IsData = true;
		log_DataPos = 0;
		log_DataSize = static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(2))) + (static_cast<unsigned int>(static_cast<unsigned char>(Str_.at(3)) << 8));

		//DataStrをクリア
		log_DataStr.assign(DataSize, 0x00);

		// 現在時刻取得
		hmLib_sint32 TimeSec = static_cast<hmLib_sint32>(
			static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(4))) 
			+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(5))) * 256
			+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(6))) * 256 * 256 
			+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(7))) * 256 * 256 * 256
		);

		// time_point に変換
		log_DataTime = std::chrono::system_clock::from_time_t(TimeSec);

		return false;

	}//カメラから撮影中データが得られたとき
	else if (c == 0xD1){
		//データが足りない時
		if (Str_.size()<3)return true;

		//実際にカメラからデータを待っていた時
		// dataPos が、ここではこれまでにゲットしたデータ数になっているが、ここをどうするか？？
		if (log_IsData){
			//データサイズ
			log_DataPos = static_cast<unsigned int>(static_cast<unsigned char>(Str_[1])) + (static_cast<unsigned int>(static_cast<unsigned char>(Str_[2])) << 8);
			//			unsigned int DataBegin=DataPos-(Str_.size()-3);
			unsigned int log_DataBegin = log_DataPos;

			//データ書き込み
			for (unsigned int i = 0; i<Str_.size() - 3; ++i){
				if (log_DataSize <= log_DataBegin + i)break;
				log_DataStr.at(log_DataBegin + i) = Str_.at(i + 3);
			}

			//全てのデータを受信したとき
			//			if(DataSize<=DataPos){
			if (log_DataSize <= log_DataBegin + Str_.size() - 3){
				signal_setLogPicture(log_DataStr, log_DataTime);
				log_IsData = false;
			}
			return false;
		}//要求してない時
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
	//PowerReset要求時
	if(DoPowerReset){
		DoPowerReset=false;
		Str.push_back(static_cast<unsigned char>(0x70));
		return false;
	}//AutoPowerReset設定変更時
	else if(AutoPowerResetFlagirl.talk()){
		if(AutoPowerResetFlagirl.request())Str.push_back(static_cast<unsigned char>(0x80));
		else Str.push_back(static_cast<unsigned char>(0x81));
		return false;
	}//CommandReset要求時
	else if(DoCommandReset){
		DoCommandReset=false;
		Str.push_back(static_cast<unsigned char>(0x60));
		return false;
	}//オートライト設定時
	else if(AutoLightFlagirl.talk()){
		if(AutoLightFlagirl.request())Str.push_back(static_cast<unsigned char>(0x40));
		else Str.push_back(static_cast<unsigned char>(0x41));
		return false;
	}//ライトの設定
	else if(LightFlagirl.talk()){
		if(LightFlagirl.request())Str.push_back(static_cast<unsigned char>(0x30));
		else Str.push_back(static_cast<unsigned char>(0x31));
		return false;
	}//MIniパケットモードの設定
	else if(MiniPacketModeFlagirl.talk()){
		if(MiniPacketModeFlagirl.request())Str.push_back(static_cast<unsigned char>(0x50));
		else Str.push_back(static_cast<unsigned char>(0x51));
		return false;
	}// 連続撮影モードの設定
	else if(AutoTakePicFlagirl.talk()){
		if(AutoTakePicFlagirl.request()){
			Str.push_back(static_cast<unsigned char>(0x10));
			Str.push_back(static_cast<unsigned char>(0x00));// Picture サイズは一番小さいもの
		}
		else Str.push_back(static_cast<unsigned char>(0x11));
		return false;
	}
	//写真要求モードになっている
	else if(DoSetTakePicture){
		DoSetTakePicture=false;
		//PictureSizeを適正値に補正
		if(PictureSize<0)PictureSize=0;
		else if(PictureSize>2)PictureSize=2;
		//コマンド0x00送信
		Str.push_back(static_cast<unsigned char>(0x00));
		Str.push_back(PictureSize);
		return false;
	}
	// 内部情報送信モードの設定
	else if(InfoModeFlagirl.talk()){
		if(InfoModeFlagirl.request())Str.push_back(static_cast<unsigned char>(0xB0));
		else Str.push_back(static_cast<unsigned char>(0xB1));
		return false;
	}
	// log modeの処理
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
