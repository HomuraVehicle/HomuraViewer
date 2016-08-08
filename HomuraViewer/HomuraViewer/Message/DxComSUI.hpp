#ifndef HMR_VIEWER_MESSAGE_DXCOMSUI_INC
#define HMR_VIEWER_MESSAGE_DXCOMSUI_INC 100
#
#include<boost/format.hpp>
#include<hmLib/exceptions.hpp>
#include<hmLib/inquiries.hpp>
#include<HomuraViewer/View/DxBUI.hpp>
namespace hmr{
	namespace viewer{
		namespace message{
			class dxosComSUI :public dxosBUI{
			public:
				dxosComSUI() :dxosBUI("Com", 30, 105){}
			public:
				hmLib::inquiries::inquiry<unsigned int> RecvBufSizeInq;
				hmLib::inquiries::inquiry<unsigned int> SendBufSizeInq;
				//hmLib::inquiries::inquiry<unsigned int> inquiry_RecvLogSize;
				//hmLib::inquiries::inquiry<unsigned int> inquiry_SendLogSize;
				hmLib::inquiries::inquiry<clock::time_point> RecvTimeInq;
				hmLib::inquiries::inquiry<clock::time_point> SendTimeInq;

				// signal Œn—ñ‚Ì’è‹`
				boost::signals2::signal<void(void)> signal_require_clearRecvBuf;
				boost::signals2::signal<void(void)> signal_require_clearSendBuf;

			public:
				virtual int normal_draw(dxO& dxo)override{
					unsigned int val;

					// can get Œn—ñ‚Ì•`‰æ
					// is Open
					dxo.draw(Pint(5, 5), dxoStrP(Pint(70, 20), "Normal", getClr(normal, strobj)));

					// out Buf
					try{
						val = SendBufSizeInq();
						dxo.draw(Pint(80, 5), dxoStrP(Pint(20, 20), "O:", getClr(normal, strobj)));
						dxo.draw(Pint(100, 5), dxoStrP(Pint(50, 20), hmstd::fStr("%d", val), getClr(val < 5 ? normal : error, strobj)));
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(80, 5), dxoStr("Inquiry::outBufSize Žæ“¾Ž¸”s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(80, 5), dxoStr(Excp.what()));
					}

					// inBuf Size ‚É‚Â‚¢‚Ä‚Ìó‘Ô•\Ž¦
					try{
						val = RecvBufSizeInq();
						dxo.draw(Pint(155, 5), dxoStrP(Pint(20, 20), "I:", getClr(normal, strobj)));
						dxo.draw(Pint(175, 5), dxoStrP(Pint(50, 20), hmstd::fStr("%d", val), getClr(val < 5 ? normal : error, strobj)));
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(155, 5), dxoStr("Inquiry::inBufSize Žæ“¾Ž¸”s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(155, 5), dxoStr(Excp.what()));
					}

					return 0;
				}

				/*
				try{
				dxo.draw(Pint(5,5),dxoStrP(Pint(70,20),"Buf"));

				val=inquiry_RecvBufSize();
				dxo.draw(Pint(80,5),dxoStrP(Pint(70,20),(boost::format("%d")%val).str(),getClr(val<5?normal:error,strobj)));
				val=inquiry_SendBufSize();
				dxo.draw(Pint(155,5),dxoStrP(Pint(70,20),(boost::format("%d")%val).str(),getClr(val<5?normal:error,strobj)));

				dxo.draw(Pint(5,30),dxoStrP(Pint(70,20),"Log"));

				val=inquiry_RecvLogSize();
				dxo.draw(Pint(80,30),dxoStrP(Pint(70,20),(boost::format("%d")%val).str(),getClr(val<5?normal:error,strobj)));
				val=inquiry_SendLogSize();
				dxo.draw(Pint(155,30),dxoStrP(Pint(70,20),(boost::format("%d")%val).str(),getClr(val<5?normal:error,strobj)));
				}catch(const hmLib::exceptions::exception& Excp){
				dxo.draw(Pint(0,0),dxoButIO(getSize(),Excp.what(),getClr(error,butobj),true));
				}

				*/

				virtual int extend_draw(dxO& dxo)override{

					unsigned int val;
					// draw

					// outBuf Size ‚É‚Â‚¢‚Ä‚Ìó‘Ô•\Ž¦
					try{
						val = SendBufSizeInq();
						dxo.draw(Pint(5, 5), dxoStrP(Pint(70, 20), "OutBuf", getClr(normal, strobj)));
						dxo.draw(Pint(80, 5), dxoStrP(Pint(70, 20), hmstd::fStr("%d", val), getClr(val < 5 ? normal : error, strobj)));
						if(dxo.draw(Pint(155, 5), dxoBut(Pint(70, 20), "CLR", getClr(normal, strobj))) == 1){
							signal_require_clearSendBuf();
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 5), dxoStr("Inquiry::outBufSize Žæ“¾Ž¸”s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 5), dxoStr(Excp.what()));
					}

					// inBuf Size ‚É‚Â‚¢‚Ä‚Ìó‘Ô•\Ž¦
					try{
						val = RecvBufSizeInq();
						dxo.draw(Pint(5, 30), dxoStrP(Pint(70, 20), "InBuf", getClr(normal, strobj)));
						dxo.draw(Pint(80, 30), dxoStrP(Pint(70, 20), hmstd::fStr("%d", val), getClr(val < 5 ? normal : error, strobj)));
						if(dxo.draw(Pint(155, 30), dxoBut(Pint(70, 20), "CLR", getClr(normal, strobj))) == 1){
							signal_require_clearRecvBuf();
						}
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 30), dxoStr("Inquiry::inBufSize Žæ“¾Ž¸”s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 30), dxoStr(Excp.what()));
					}

					// ŽóMŽžŠÔ‚Ì•\Ž¦
					// Šeó‘Ô‚É‚Â‚¢‚Ä•`‰æ‚·‚é
					try{
						dxo.draw(Pint(5, 55), dxoStrP(Pint(105, 20), "Sended at", getClr(normal, strobj)));
						dxo.draw(Pint(115, 55), dxoStrP(Pint(105, 20), time_to_hms(SendTimeInq()), getClr(normal, strobj)));
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 55), dxoStr("Inquiry::Send Time Žæ“¾Ž¸”s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 55), dxoStr(Excp.what()));
					}

					try{
						dxo.draw(Pint(5, 80), dxoStrP(Pint(105, 20), "Recieved at", getClr(normal, strobj)));
						dxo.draw(Pint(115, 80), dxoStrP(Pint(105, 20), time_to_hms(RecvTimeInq()), getClr(normal, strobj)));
					}
					catch(const hmLib::inquiries::unconnected_exception&){
						dxo.draw(Pint(5, 80), dxoStr("Inquiry::Recv Time Žæ“¾Ž¸”s"));
					}
					catch(const hmLib::exceptions::exception& Excp){
						dxo.draw(Pint(5, 80), dxoStr(Excp.what()));
					}

					return 0;



				}
				virtual status getStatus()const{
					return normal;
				}
			};
		}
	}
}
#
#endif
