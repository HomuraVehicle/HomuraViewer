#ifndef HMR_CO2_CPP_INC
#define HMR_CO2_CPP_INC 101
#

#ifndef HMR_CO2_INC
#	include"hmrCO2.hpp"
#endif

const double hmr::cCO2MsgAgent::D_ADMaxValue=4096.;
double hmr::cCO2MsgAgent::toCO2(unsigned char LowByte,unsigned char HighByte){
	unsigned int tmp=static_cast<unsigned int>(LowByte+HighByte*256);
	return ((static_cast<double>(tmp)/D_ADMaxValue*2.5)-0.5)*1000.;
}
bool hmr::cCO2MsgAgent::listen(datum::time_point Time_,bool Err_,const std::string& Str_){
	
	// data getting mode
	if(static_cast<unsigned char>(Str_[0])==0x00){
		if(Str_.size() != 3)return true;
		Value=toCO2(static_cast<unsigned char>(Str_[1]),static_cast<unsigned char>(Str_[2]));
		Time=Time_;
		signal_newData(Value, Time);
//		Ref.insert(Datum(_time,_errFlag,static_cast<unsigned char>(Str_[2])*256+static_cast<unsigned char>(Str_[1])));
		return false;
	}

	// Change Data getting mode
	else if(static_cast<unsigned char>(Str_[0])==0x10){
		if(Str_.size() != 1)return true;
		DataModeFlagirl.set_pic(true);
		return false;
	}
	else if(static_cast<unsigned char>(Str_[0])==0x11){
		if(Str_.size() != 1)return true;
		DataModeFlagirl.set_pic(false);
		return false;
	}

	// Change sensor power status
	else if(static_cast<unsigned char>(Str_[0])==0x20){
		if(Str_.size() != 1)return true;
		SensorPWFlagirl.set_pic(true);
		return false;
	}
	else if(static_cast<unsigned char>(Str_[0])==0x21){
		if(Str_.size() != 1)return true;
		SensorPWFlagirl.set_pic(false);
		return false;
	}

	// Change pump power status
	else if(static_cast<unsigned char>(Str_[0])==0x30){
		if(Str_.size() != 1)return true;
		PumpPWFlagirl.set_pic(true);
		return false;
	}
	else if(static_cast<unsigned char>(Str_[0])==0x31){
		if(Str_.size() != 1)return true;
		PumpPWFlagirl.set_pic(false);
		return false;
	}
			
	// change Log Mode
	else if (static_cast<unsigned char>(Str_[0]) == 0xC0){
		if (Str_.size() != 1)return true;
		LogModeFlagirl.set_pic(true);
		return false;
	}
	else if (static_cast<unsigned char>(Str_[0]) == 0xC1){
		if (Str_.size() != 1)return true;
		LogModeFlagirl.set_pic(false);
		return false;
	}
	
	// getting log data
	if (static_cast<unsigned char>(Str_[0]) == 0xD0){
		if (Str_.size() != 7)return true;
		// dataéÊìæ
		LogValue = static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(1)))
			+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(2))) * 256;

		// åªç›éûçèéÊìæ
		hmLib_sint32 TimeSec = static_cast<hmLib_sint32>(
			static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(3))) 
			+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(4))) * 256
			+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(5))) * 256 * 256
			+ static_cast<hmLib_uint32>(static_cast<unsigned char>(Str_.at(6))) * 256 * 256 * 256
		);
		// time_point Ç…ïœä∑
		LogTime = std::chrono::system_clock::from_time_t(TimeSec);

		signal_newLogData(LogValue, LogTime);
		//		Ref.insert(Datum(_time,_errFlag,static_cast<unsigned char>(Str_[2])*256+static_cast<unsigned char>(Str_[1])));
		return false;
	}

	return true;
}
bool hmr::cCO2MsgAgent::talk(std::string& Str){
	Str="";
	if(DataModeFlagirl.talk()){
		if(DataModeFlagirl.request())Str.push_back(static_cast<unsigned char>(0x10));
		else Str.push_back(static_cast<unsigned char>(0x11));
		return false; 
	}else if(SensorPWFlagirl.talk()){
		if (SensorPWFlagirl.request())Str.push_back(static_cast<unsigned char>(0x20));
		else Str.push_back(static_cast<unsigned char>(0x21));
		return false;
	}else if(PumpPWFlagirl.talk()){
		if (PumpPWFlagirl.request())Str.push_back(static_cast<unsigned char>(0x30));
		else Str.push_back(static_cast<unsigned char>(0x31));
		return false;
	}
	else if (LogModeFlagirl.talk()){
		if (LogModeFlagirl.request())Str.push_back(static_cast<unsigned char>(0xC0));
		else Str.push_back(static_cast<unsigned char>(0xC1));
		return false;
	}

	return true;	
}
void hmr::cCO2MsgAgent::setup_talk(){
	DataModeFlagirl.setup_talk();
	LogModeFlagirl.setup_talk();

	SensorPWFlagirl.setup_talk();
	PumpPWFlagirl.setup_talk();
	LogModeFlagirl.setup_talk();
}

void hmr::cCO2MsgAgent::setDataMode(bool Mode_){DataModeFlagirl.set_request(Mode_);}
void hmr::cCO2MsgAgent::setSensorPW(bool Mode_){SensorPWFlagirl.set_request(Mode_);}
void hmr::cCO2MsgAgent::setPumpPW(bool Mode_){PumpPWFlagirl.set_request(Mode_);}
double hmr::cCO2MsgAgent::getValue()const{return Value;}

void hmr::cCO2MsgAgent::slot_setDataMode(boost::signals2::signal<void(bool)>& Signal_){
	SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setDataMode(Flag);}));	
}
void hmr::cCO2MsgAgent::slot_setDataMode(boost::signals2::signal<void(void)>& Signal_){
	SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setDataMode(!DataModeFlagirl.request());}));	
}

void hmr::cCO2MsgAgent::slot_setSensorPW(boost::signals2::signal<void(bool)>& Signal_){
	SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setSensorPW(Flag);}));	
}
void hmr::cCO2MsgAgent::slot_setSensorPW(boost::signals2::signal<void(void)>& Signal_){
	SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setSensorPW(!SensorPWFlagirl.request());}));	
}

void hmr::cCO2MsgAgent::slot_setPumpPW(boost::signals2::signal<void(bool)>& Signal_){
	SignalConnections(hmLib::signals::connect(Signal_,[&](bool Flag)->void{this->setPumpPW(Flag);}));	
}
void hmr::cCO2MsgAgent::slot_setPumpPW(boost::signals2::signal<void(void)>& Signal_){
	SignalConnections(hmLib::signals::connect(Signal_,[&](void)->void{this->setPumpPW(!PumpPWFlagirl.request());}));	
}

void hmr::cCO2MsgAgent::slot_setLogMode(boost::signals2::signal<void(bool)>& Signal_){
	SignalConnections(hmLib::signals::connect(Signal_, [&](bool flag)->void{this->LogModeFlagirl.set_request(flag); }));
}


void hmr::cCO2MsgAgent::contact_getValue(hmLib::inquiries::inquiry<double>& Inquiry_){
	InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->double{return this->getValue();}));
}
void hmr::cCO2MsgAgent::contact_getTime(hmLib::inquiries::inquiry<clock::time_point>& Inquiry_){
	InquiryConnections(hmLib::inquiries::connect(Inquiry_,Time));
}

void hmr::cCO2MsgAgent::contact_getPicDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
	InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->DataModeFlagirl.pic();}));
}
void hmr::cCO2MsgAgent::contact_getPicPumpPW(hmLib::inquiries::inquiry<bool>& Inquiry_){
	InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->PumpPWFlagirl.pic();}));
}
void hmr::cCO2MsgAgent::contact_getPicSensorPW(hmLib::inquiries::inquiry<bool>& Inquiry_){
	InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->SensorPWFlagirl.pic();}));
}
void hmr::cCO2MsgAgent::contact_getPicLogMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
	InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->LogModeFlagirl.pic(); }));
}

void hmr::cCO2MsgAgent::contact_getRequestDataMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
	InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->DataModeFlagirl.request();}));
}
void hmr::cCO2MsgAgent::contact_getRequestPumpPW(hmLib::inquiries::inquiry<bool>& Inquiry_){
	InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->PumpPWFlagirl.request();}));
}
void hmr::cCO2MsgAgent::contact_getRequestSensorPW(hmLib::inquiries::inquiry<bool>& Inquiry_){
	InquiryConnections(hmLib::inquiries::connect(Inquiry_,[&](void)->bool{return this->SensorPWFlagirl.request();}));
}
void hmr::cCO2MsgAgent::contact_getRequestLogMode(hmLib::inquiries::inquiry<bool>& Inquiry_){
	InquiryConnections(hmLib::inquiries::connect(Inquiry_, [&](void)->bool{return this->LogModeFlagirl.request(); }));
}

#
#endif