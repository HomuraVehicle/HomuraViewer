#ifndef HMR_DXCOM_INC
#define HMR_DXCOM_INC 101
#
/*===hmrDxCom===
Com�֘A��DxLib�ˑ��̕`��N���X���`

hmrDxCom:v1_01/130615 hmIto
	Packet/Datum�̎��ʂ�time����no_type�Ɉڍs
		time���ƁA����p�P�b�g���ł͑S�ē������Ԃ������ꍇ������B
hmrDxCom v1_00/130410 hmIto
	dxoPacketList��ǉ�
*/
#include"hmLibVer.hpp"
#include<hmLib_v3_06/config_vc.h>
#include<mutex>
#include<vector>
#include<chrono>
#include<iomanip>
#include<sstream>
#include<boost/format.hpp>
#include<boost/signals2.hpp>
#include<hmLib_v3_06/signals.hpp>
#include<hmLib_v3_06/inquiries.hpp>
#include<hmLib_v3_06/any_iterator.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxArea.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include"hmrData.hpp"
#include"iologgate.hpp"
namespace hmr{

	//������
	// IDList�p�̃V�O�i��
	//	clear
	//	add
	//�f�[�^��id:�擪��������`�悷��AdxButIO�^�̕`��N���X
	class dxoCatDatum:public dxReObject,public hmoBox{
	private:
		const datum& rDatum;
		unsigned int CatNum;
		dxHLS Clr;
		bool ON;
	public:
		dxoCatDatum(Pint Size_,const datum& Datum_,unsigned int CatNum_,const dxRGB& Clr_,bool ON_)
			:hmoBox(Size_),rDatum(Datum_),CatNum(CatNum_),Clr(Clr_),ON(ON_){
		}
		int redraw(dxO& dxo){
			std::string Str;
			Str+=rDatum.ID;
			Str+=":";
			for(unsigned int Cnt=0;Cnt<CatNum;++Cnt){
				if(Cnt>=rDatum.Str.size())Str+="--";
				else Str+=(boost::format("%02x")%(static_cast<unsigned int>(static_cast<unsigned char>(rDatum.Str.at(Cnt))))).str();
			}
			Clr.lgt=std::min(240,Clr.lgt+rDatum.Size);
			return dxo.draw(Pint(0,0),dxoButIO(size,Str,Clr,ON,rDatum.ErrFlag?CLR::LightSoftRed:CLR::White));
		}
	};
	//�p�P�b�g�̗����`�悷��`��֐��N���X
	template<typename iterator>
	class dxFnPacketList:public dxFn{
		typedef std::pair<packet::no_type,datum::no_type> seltype;
	private:
		iterator Begin;									//PacketLog�̐擪
		iterator End;									//PacketLog�̖���
		const packet* pInPacket;						//�ŐV����Packet
		const packet* pOutPacket;						//�ŐV�o��Packet
		const dxRGB& InputClr;							//���͑��̐F
		const dxRGB& OutputClr;							//�o�͑��̐F
		const seltype& SelData;							//�I������Ă���f�[�^ Log���肵�Ă��Ȃ��f�[�^��clocl::time_point::max
		bool LinkNewPacket;								//�ŐV�f�[�^����ɍŏ㕔�ɕ\������悤�A�\���ʒu���Œ肷�邩�ǂ����H
		const std::vector<datum::id_type>& SelID;		//ID�I��\�����Ɏg�p
	public:
		boost::signals2::signal<void(seltype)> signal_pushedDatum;		//�I������Ă���f�[�^���ύX���ꂽ�ꍇ�ɔ��M
	public:
		dxFnPacketList(iterator Begin_,iterator End_,const packet* pInPacket_,const packet* pOutPacket_,const seltype& SelData_,bool LinkNewPacket_,const std::vector<datum::id_type>& SelID_,const dxRGB& InputClr_=CLR::SoftRed,const dxRGB& OutputClr_=CLR::SoftBlue)
			:Begin(std::move(Begin_))
			,End(std::move(End_))
			,pInPacket(pInPacket_)
			,pOutPacket(pOutPacket_)
			,SelData(SelData_)
			,LinkNewPacket(LinkNewPacket_)
			,SelID(SelID_)
			,InputClr(InputClr_)
			,OutputClr(OutputClr_){
		}
		int fndraw(dxO& dxo){
			/*ScrlFrame����*/
			try{
//				//�ŐV�p�P�b�g�̈ʒu������@�i-1����=�ŌÃp�P�b�g�Ƃ��A�ǂ�ǂ�}�C�i�Xy�����ɕ`�悵�Ă����`�j
//				int TotalDrawSize=25*(End-Begin)+10;
//				if(pInPacket!=nullptr)TotalDrawSize+=25;
//				if(pOutPacket!=nullptr)TotalDrawSize+=25;

				//�`��ʒu�m��
				dxO::Acs<dxScrlFrame> Acs(dxo);
				int ViewPos=Acs().viewpos.y;
				int ViewSize=Acs().size.y;

				dx::draw(Acs().AtoF(Pint(5,5),dxo),dxoStr((boost::format("Top:%d Bottom:%d")%ViewPos%(ViewPos+ViewSize)).str()));

				//y�����̕`��ʒu���o���Ă����ϐ�
				int DrawPos;

				//PacketLog��
				{
					//ID�̑I�����s���Ă��Ȃ��ꍇ
					if(SelID.size()==0){
						//�܂��A�`�悷��O����m��
						auto DBegin=Begin+std::min(std::max(0,(-ViewPos-ViewSize)/25-1),End-Begin);
						auto DEnd=Begin+std::min(std::max(0,(-ViewPos)/25+1),End-Begin);
						//�`��ʒu�ƂȂ�悤�ɕ\��
						if((-ViewPos-ViewSize)/25-1<0)DrawPos=-25;
						else if((-ViewPos-ViewSize)/25-1>End-Begin)DrawPos=-25*(End-Begin);
						else DrawPos=((ViewPos+ViewSize)/25)*25;


						//�`��ʒu�̂ݕ`��
						for(auto pitr=DBegin;pitr!=DEnd;++pitr){
							dxRGB Clr;
							if(pitr->first)Clr=InputClr;
							else Clr=OutputClr;

							int DrawXPos=120;

							//�����`��
							dxo.draw(Pint(0,DrawPos),dxoStrP(Pint(120,25),hmr::time_to_hms(pitr->second.Time),pitr->second.ErrFlag?CLR::LightSoftRed:CLR::LightGray));

							//Packet���f�[�^�����ɕ`��
							for(auto itr=pitr->second.Data.begin();itr!=pitr->second.Data.end();++itr){
								bool IsSelected=(SelData.first==pitr->second.No && SelData.second==itr->No);
								if(dxo.draw(Pint(DrawXPos,DrawPos),dxoCatDatum(Pint(70,20),*itr,2,Clr,IsSelected))<0){
									if(IsSelected)signal_pushedDatum(seltype(0,0));
									else signal_pushedDatum(seltype(pitr->second.No,itr->No));
								}
								DrawXPos+=75;
							}
							DrawPos-=25;
						}

						//�`��ʒu��{���̈ʒu�Ɉړ�
						DrawPos=(End-Begin)*-25-25;
					}//ID���I������Ă���ꍇ
					else{
						//�`��ʒu���s���Ȃ�ŁA�����n����X�^�[�g
						DrawPos=-25;

						//��������Œ��ׂĂ���
						for(auto pitr=Begin;pitr!=End;++pitr){
							dxRGB Clr;
							if(pitr->first)Clr=InputClr;
							else Clr=OutputClr;

							int DrawXPos=125;

							bool DrawFlag=false;
							bool ESCFlag=false;

							//�����`��
							dxo.draw(Pint(0,DrawPos),dxoStrP(Pint(120,25),hmr::time_to_hms(pitr->second.Time),CLR::LightGray));

							//Packet���f�[�^�����ɕ`��
							for(auto itr=pitr->second.Data.begin();itr!=pitr->second.Data.end();++itr){
								//IDList�ƏƂ炵���킹�Ă݂�
								for(auto selitr=SelID.begin();selitr!=SelID.end();++selitr){
									//IDList��datum::ID���܂܂�Ă���ꍇ
									if(*selitr==itr->ID){
										DrawFlag=true;
										//�`��͈͂ɓ����Ă��Ȃ���΁A�`�F�b�N�������ĒE�o
										if(DrawPos<ViewPos-25 || ViewPos+ViewSize+25<DrawPos){
											//Packet����for������E�o����悤�Ƀt���O�𗧂ĂĂ���
											ESCFlag=true;
										}else{
											bool IsSelected=(SelData.first==pitr->second.No && SelData.second==itr->No);
											if(dxo.draw(Pint(DrawXPos,DrawPos),dxoCatDatum(Pint(70,20),*itr,2,Clr,IsSelected))<0){
												if(IsSelected)signal_pushedDatum(seltype(0,0));
												else signal_pushedDatum(seltype(pitr->second.No,itr->No));
											}
											DrawXPos+=75;
										}
										break;
									}
								}
								//�`��͈͊O�̏ꍇ�A�p�P�b�g���ɕ`��f�[�^�����邱�Ƃ����킩��΂����̂ŒE�o
								if(ESCFlag)break;
							}
							if(DrawFlag)DrawPos-=25;
						}
					}
				}

				//OutPacket��
				if(pOutPacket!=nullptr && SelID.size()==0){
					//�`��͈͓��̎�
					if(ViewPos<=DrawPos+20 && DrawPos<ViewPos+ViewSize){
						dxo.draw(Pint(0,DrawPos),dxoTriangle(Pint(0,20),Pint(5,10),OutputClr));
						int DrawXPos=5;
						//Packet���f�[�^�����ɕ`��
						for(auto itr=pOutPacket->Data.begin();itr!=pOutPacket->Data.end();++itr){
							bool IsSelected=(SelData.first==0 && SelData.second==itr->No);
							if(dxo.draw(Pint(DrawXPos,DrawPos),dxoCatDatum(Pint(70,20),*itr,2,OutputClr,IsSelected))<0){
								if(IsSelected)signal_pushedDatum(seltype(0,0));
								else signal_pushedDatum(seltype(0,itr->No));
							}
							DrawXPos+=75;
						}
						DrawPos-=25;
					}
				}

				//InPacket��
				if(pInPacket!=nullptr && SelID.size()==0){
					//�`��͈͓��̎�
					if(ViewPos<=DrawPos+20 && DrawPos<ViewPos+ViewSize){
						dxo.draw(Pint(0,DrawPos),dxoTriangle(Pint(0,20),Pint(5,10),InputClr));
						int DrawXPos=5;
						//Packet���f�[�^�����ɕ`��
						for(auto itr=pInPacket->Data.begin();itr!=pInPacket->Data.end();++itr){
							bool IsSelected=(SelData.first==0 && SelData.second==itr->No);
							if(dxo.draw(Pint(DrawXPos,DrawPos),dxoCatDatum(Pint(70,20),*itr,2,InputClr,IsSelected))<0){
								if(IsSelected)signal_pushedDatum(seltype(0,0));
								else signal_pushedDatum(seltype(0,itr->No));
							}
							DrawXPos+=75;
						}
						DrawPos-=25;
					}
				}

				//�Ō�ɕ`��ʒu�̎w����s���Ă���
				dxo.draw(Pint(0,0),dxoNull());
				dxo.draw(Pint(0,DrawPos+25),dxoNull());
			}catch(const std::exception& Except){
				dxo.draw(Pint(5, 5),dxoStr("***ERROR***"));
				dxo.draw(Pint(5,25),dxoStr(Except.what()));
			}catch(const std::string& Str){
				dxo.draw(Pint(5, 5),dxoStr("***ERROR***"));
				dxo.draw(Pint(5,25),dxoStr(Str));
			}

			return 0;
		}
	};
	//�f�[�^�̒��g��`�悷��`��֐��N���X
	class dxFnDatumView:public dxFn{
	private:
		const datum* Ptr;
	public:
		dxFnDatumView(const datum* Ptr_):Ptr(Ptr_){}
		int fndraw(dxO& dxo){
			/*ScrlFrame����*/
			try{
				//�I������Ă��Ȃ��Ƃ��ɂ́A�����\�����Ȃ��B
				if(Ptr==nullptr){
					dxo.draw(Pint(5,5),dxoStr("No Data"));
					return 0;
				}

				int DrawPos=5;

				//�`�敝�m��
				dxO::Acs<dxScrlFrame> Acs(dxo);
				//5�����P�ʂ�StrBlock�̉������̐�
				//	25*5-5=120pixel��StrBlock
				//	���[5+�ʒu���45=55���Œ肵�ĕK�v�Ȓ���
				int StrBlockNum=(Acs().size.x-55)/145;

				//�X�N���[���m��
				dxo.draw(Pint(0,0),dxoNull());
				dxo.draw(Pint(0,50+25*StrBlockNum),dxoNull());

				//�`��@�܂���b���
				dxo.draw(Pint(5,DrawPos),dxoStr((boost::format("ID:%c Size:%d(%d) Err:%d")%Ptr->ID%Ptr->Size%Ptr->Str.size()%Ptr->ErrFlag).str()));
				DrawPos+=20;
				dxo.draw(Pint(5,DrawPos),dxoStr(hmr::time_to_hms(Ptr->Time)));

				//���ɕ�������
				int DrawPosX=5;
				unsigned int Cnt=0;
				for(auto itr=Ptr->Str.begin();itr!=Ptr->Str.end();++itr){
					if((Cnt%(StrBlockNum*5))==0){
						DrawPos+=45;
						DrawPosX=5;
						dxo.draw(Pint(DrawPosX,DrawPos+10),dxoStr((boost::format("%4d")%Cnt).str()));
						DrawPosX+=40+5;	//������40pixel+��5pixel
					}else if(Cnt%5==0){
						DrawPosX+=10;
					}
					dxo.draw(Pint(DrawPosX,DrawPos),dxoStr((boost::format("%02x")%static_cast<unsigned int>(static_cast<unsigned char>(*itr))).str()));
					dxo.draw(Pint(DrawPosX,DrawPos+20),dxoStr((boost::format("%c")%(*itr)).str()));
					DrawPosX+=25;
					++Cnt;
				}

			}catch(const std::exception& Except){
				dxo.draw(Pint(5, 5),dxoStr("***ERROR***"));
				dxo.draw(Pint(5,25),dxoStr(Except.what()));
			}catch(const std::string& Str){
				dxo.draw(Pint(5, 5),dxoStr("***ERROR***"));
				dxo.draw(Pint(5,25),dxoStr(Str));
			}

			return 0;
		}
	};
	//dxFnPacketList��dxFnDatumView�𓝍������`��N���X
	template<typename iterator>
	class dxosPacketList_withView:public dxReObject,public hmoBox{
		typedef std::pair<packet::no_type,datum::no_type> seltype;
	private:
		seltype SelData;	//���ݑI������Ă���f�[�^
		bool LinkNewPacket;	//NewPacket�Ƀ����N�����邩�̃t���O
		bool ActiveDatumView;
		std::vector<datum::id_type> IDList;
		dxRGB Clr;
	public:
		int DatumViewSize;	//Datum�\����ʂ�y�����̃T�C�Y
	private:
		hmLib::signals::unique_connections SignalConenctions;
	private:
		void setSelData(seltype SelData_){
			SelData=SelData_;
			signal_setSelData(SelData);
		}
		void setLinkNewPacket(bool LinkNewPacket_){LinkNewPacket=LinkNewPacket_;}
		void syncDatumViewSize(int DatumViewSize_){DatumViewSize=DatumViewSize_;}
		void addIDList(datum::id_type ID_){
			auto itr=IDList.begin();
			for(;itr!=IDList.end();++itr){
				if(*itr!=ID_)break;
			}
			if(itr==IDList.end())IDList.push_back(ID_);
		}
		void clearIDList(datum::id_type ID_){
			auto itr=IDList.begin();
			for(;itr!=IDList.end();++itr){
				if(*itr!=ID_)break;
			}
			if(itr!=IDList.end())IDList.erase(itr);
		}
	public:
		boost::signals2::signal<void(seltype)> signal_setSelData;
		hmLib::inquiries::range_inquiry<iterator> inquiryPacketLog;
		hmLib::inquiries::inquiry<const packet&> inquiryNewInPacket;
		hmLib::inquiries::inquiry<const packet&> inquiryNewOutPacket;
		void slot_addIDList(boost::signals2::signal<void(datum::id_type)>& Signal_){
			SignalConenctions(hmLib::signals::connect(Signal_,[&](datum::id_type ID_)->void{this->addIDList(ID_);}));
		}
		void slot_clearIDList(boost::signals2::signal<void(datum::id_type)>& Signal_){
			SignalConenctions(hmLib::signals::connect(Signal_,[&](datum::id_type ID_)->void{this->clearIDList(ID_);}));
		}
		void slot_setSelData(boost::signals2::signal<void(seltype)>& Signal_){
			SignalConenctions(hmLib::signals::connect(Signal_,[&](seltype SelData_)->void{this->setSelData(SelData_);}));
		}
		void slot_setLinkNewPacket(boost::signals2::signal<void(bool)>& Signal_){
			SignalConenctions(hmLib::signals::connect(Signal_,[&](bool LinkNewPacket_)->void{this->setLinkNewPacket(LinkNewPacket_);}));
		}
	private:
		dxScrlFrame PacketListFrame;
		dxScrlFrame DatumViewFrame;
	public:
		int redraw(dxO& dxo)override{
			if(!inquiryPacketLog.is_connect()){
				dxo.draw(Pint(0,0),dxoStr("NoConnection of RangeInquiry"));
				return 0;
			}
			//Mutex�������b�N
//			std::lock(inquiryPacketLog,inquiryNewInPacket,inquiryNewOutPacket);

			try{
				//DatumView�̐ݒ�l���ȏ�łȂ��ꍇ
				const datum* pSelDatum=nullptr;
				std::pair<bool,packet> TargetP;
				datum Target;
				TargetP.second.No=SelData.first;
				Target.No=SelData.second;
				if(0<=DatumViewSize && DatumViewSize<=size.y){
					//Packet::Time���s��l���w���Ă���Ƃ��́A���݂̃p�P�b�g����T��
					if(SelData.first==0){
						if(inquiryNewInPacket.is_connect() && inquiryNewInPacket().Data.size()){

							//Inquiry�̒��g��begin/end���Ă�ł���̂ŁA����Inquiry�͎Q�Ǝ擾�^(T&)���ׂ�
							//	�ʏ�̃^�C�v(inquiry<T>)���g���ƁA�R�s�[���ꂽData��begin/end���g���Ă��܂��B
							auto rditr=std::equal_range(inquiryNewInPacket().Data.begin(),inquiryNewInPacket().Data.end(),Target);
							
							if(rditr.first!=rditr.second){
								pSelDatum=&(*(rditr.first));
							}
						}
						if(inquiryNewOutPacket.is_connect() && inquiryNewOutPacket().Data.size()){
							// ����ł̓G���[���o�Ă��܂��̂ŕύX
							auto sditr=std::equal_range(inquiryNewOutPacket().Data.begin(),inquiryNewOutPacket().Data.end(),Target);
							
							if(sditr.first!=sditr.second){
								pSelDatum=&(*(sditr.first));
							}
						}
					}else{
						// ����ł̓G���[���o�Ă��܂��̂ŕύX
						 auto pitr=std::equal_range(inquiryPacketLog.begin(),inquiryPacketLog.end(),TargetP,[](const std::pair<bool,packet>& Pair1_,const std::pair<bool,packet>& Pair2_)->bool{return Pair1_.second.No<Pair2_.second.No;});
						
						if(pitr.first!=pitr.second){
							// ����ł̓G���[���o��̂ŕύX
							auto ditr=std::equal_range((*pitr.first).second.Data.begin(),(*pitr.first).second.Data.end(),Target,[&](const datum& d1,const datum& d2)->bool{return d1.No<d2.No;});

							if(ditr.first!=ditr.second){
								pSelDatum=&(*(ditr.first));
							}
						}
					}
				}
				
				//Datum���I������Ă��Ȃ��Ƃ�
				if(pSelDatum==nullptr){
					const packet* pInPacket=nullptr;
					const packet* pOutPacket=nullptr;

					if(inquiryNewInPacket.is_connect()){
						if(inquiryNewInPacket().Data.size()){
							pInPacket=&inquiryNewInPacket();
						}
					}
					if(inquiryNewOutPacket.is_connect()){
						if(inquiryNewOutPacket().Data.size()){
							pOutPacket=&inquiryNewOutPacket();
						}
					}

					//�p�P�b�g�`��̐ݒ�
					dxFnPacketList<iterator> FnPacketList(inquiryPacketLog.begin(),inquiryPacketLog.end(),pInPacket,pOutPacket,SelData,LinkNewPacket,IDList);
					slot_setSelData(FnPacketList.signal_pushedDatum);
					PacketListFrame.set(FnPacketList,Pint(size.x,size.y),Clr);
					PacketListFrame.resetMoveArea(ALI::all);

//					if(ActiveDatumView){
//						ActiveDatumView=false;
//						PacketListFrame.viewpos.y+=DatumViewSize;
//					}

					//�`��
//					dxo.draw(Pint(0,0),dxoStr("PacketList"));
					dxo.draw(Pint(0,0),PacketListFrame);
				}//Datum���I������Ă��鎞
				else{
					const packet* pInPacket=nullptr;
					const packet* pOutPacket=nullptr;

					if(inquiryNewInPacket.is_connect()){
						if(inquiryNewInPacket().Data.size()){
							pInPacket=&inquiryNewInPacket();
						}
					}
					if(inquiryNewOutPacket.is_connect()){
						if(inquiryNewOutPacket().Data.size()){
							pOutPacket=&inquiryNewOutPacket();
						}
					}

					//�p�P�b�g�`��̐ݒ�
					dxFnPacketList<iterator> FnPacketList(inquiryPacketLog.begin(),inquiryPacketLog.end(),pInPacket,pOutPacket,SelData,LinkNewPacket,IDList);
					slot_setSelData(FnPacketList.signal_pushedDatum);
					PacketListFrame.set(FnPacketList,Pint(size.x,size.y-DatumViewSize),Clr);
					PacketListFrame.resetMoveArea(ALI::all);

//					if(!ActiveDatumView){
//						ActiveDatumView=true;
//						PacketListFrame.viewpos.y-=DatumViewSize;
//					}

					//�f�[�^�`��pFrame�̐ݒ�
					dxFnDatumView FnDatumView(pSelDatum);
					DatumViewFrame.set(FnDatumView,Pint(size.x,DatumViewSize),Clr);

					//�`��
					dxo.draw(Pint(0,0),PacketListFrame);
					dxo.draw(Pint(0,DatumViewSize),DatumViewFrame);
				}
			}catch(const std::exception& Except){
				dxo.draw(Pint(5, 5),dxoStr("***ERROR***"));
				dxo.draw(Pint(5,25),dxoStr(Except.what()));
				printfDx(Except.what());
				printfDx("\n");
			}catch(const std::string& Str){
				dxo.draw(Pint(5, 5),dxoStr("***ERROR***"));
				dxo.draw(Pint(5,25),dxoStr(Str));
			}

			//Mutex�A�����b�N
//			inquiryPacketLog.unlock();
	//		inquiryNewInPacket.unlock();
		//	inquiryNewOutPacket.unlock();

			return 0;
		}
		dxosPacketList_withView(Pint Size_,int DatumViewSize_,const dxRGB& Clr_)
			:hmoBox(Size_),SelData(0,0),LinkNewPacket(false),DatumViewSize(DatumViewSize_),Clr(Clr_),ActiveDatumView(false){
			PacketListFrame.resetMoveArea(ALI::all);
			PacketListFrame.setMoveArea(ALI::bottom|ALI::left,Pint(0,0));
			PacketListFrame.viewpos.y=-Size_.y;
		}
	};
}
#
#endif
