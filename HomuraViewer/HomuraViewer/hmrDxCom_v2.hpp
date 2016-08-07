#ifndef HMR_DXCOM_INC
#define HMR_DXCOM_INC 200
#
/*===hmrDxCom===
Com�֘A��DxLib�ˑ��̕`��N���X���`

hmrDxCom:v2_00/130622 hmito
	ID�ɂ���ĕ\���̗L����I���ł��铒�悤�ɕύX
	�z�C�[���X�N���[���ɑΉ�
hmrDxCom:v1_01/130615 hmIto
	Packet/Datum�̎��ʂ�time����no_type�Ɉڍs
		time���ƁA����p�P�b�g���ł͑S�ē������Ԃ������ꍇ������B
hmrDxCom v1_00/130410 hmIto
	dxoPacketList��ǉ�
*/
#include"hmLibVer.hpp"
#include<hmLib/config_vc.h>
#include<mutex>
#include<vector>
#include<chrono>
#include<iomanip>
#include<sstream>
#include<boost/format.hpp>
#include<boost/signals2.hpp>
#include<hmLib/signals.hpp>
#include<hmLib/inquiries.hpp>
#include<hmLib/any_iterator.hpp>
#include<hmLib_v2/dxObject.hpp>
#include<hmLib_v2/dxArea.hpp>
#include<hmLib_v2/dxColorSet.hpp>
#include <HomuraViewer/chrono.hpp>
#include <HomuraViewer/Message/PacketData.hpp>
#include<HomuraViewer/IO/IOLogGate.hpp>
namespace hmr{
	namespace viewer{
		//�f�[�^��id:�擪��������`�悷��AdxButIO�^�̕`��N���X
		class dxoCatDatum :public dxReObject, public hmoBox{
			using datum = message::datum;
		private:
			const datum& rDatum;
			unsigned int CatNum;
			dxHLS Clr;
			bool ON;
		public:
			dxoCatDatum(Pint Size_, const datum& Datum_, unsigned int CatNum_, const dxRGB& Clr_, bool ON_)
				:hmoBox(Size_), rDatum(Datum_), CatNum(CatNum_), Clr(Clr_), ON(ON_){}
			int redraw(dxO& dxo){
				std::string Str;
				Str += rDatum.ID;
				Str += ":";
				for(unsigned int Cnt = 0; Cnt < CatNum; ++Cnt){
					if(Cnt >= rDatum.Str.size())Str += "--";
					else Str += (boost::format("%02x") % (static_cast<unsigned int>(static_cast<unsigned char>(rDatum.Str.at(Cnt))))).str();
				}
				Clr.lgt = std::min(240, Clr.lgt + rDatum.Size);
				return dxo.draw(Pint(0, 0), dxoButIO(size, Str, Clr, ON, rDatum.ErrFlag ? CLR::LightSoftRed : CLR::White));
			}
		};
		//�f�[�^�̒��g��`�悷��`��֐��N���X
		class dxFnPacketListCtrl :public dxFn{
			using datum = message::datum;
		public:
			boost::signals2::signal<void(bool)> signal_setLinkNewPacket;
			std::vector<datum::id_type>& IDList;			//�\���̗L���𐧌䂷�邽�߂�ID�̃��X�g
			const std::vector<datum::id_type>& SwIDList;	//�X�C�b�`�Ƃ��ĕ\���̗L���𐧌䂵����ID�̃��X�g
			bool Link;
		public:
			dxFnPacketListCtrl(bool Link_, std::vector<datum::id_type>& IDList_, const std::vector<datum::id_type>& SwIDList_)
				:Link(Link_)
				, IDList(IDList_)
				, SwIDList(SwIDList_){}
			int fndraw(dxO& dxo){
				//dxNormFrame����
				try{
					//�`�敝�m��
					dxO::Acs<dxNormFrame> Acs(dxo);
					int Size = Acs().size.y;

					//Link�{�^��
					if(dxo.draw(Pint(0, 0), dxoButIO(Pint(60, Size), "Sync", CLR::SoftGreen, Link)) == 1){
						signal_setLinkNewPacket(!Link);
					}

					int PosX = 60;
					for(auto itr = SwIDList.begin(); itr != SwIDList.end(); ++itr){
						bool OnOff = false;
						auto sitr = IDList.begin();
						for(; sitr != IDList.end(); ++sitr){
							if(*itr == *sitr){
								OnOff = true;
								break;
							}
						}
						//Sw�{�^��
						if(dxo.draw(Pint(PosX, 0), dxoButIO(Pint(30, Size), (boost::format("%c") % (*itr)).str(), CLR::DarkSoftGreen, OnOff)) == 1){
							if(OnOff){
								IDList.erase(sitr);
							} else{
								IDList.push_back(*itr);
							}
						}
						PosX += 30;
					}
				}
				catch(const std::exception& Except){
					dxo.draw(Pint(0, 0), dxoStr(std::string("***ERROR***") + Except.what()));
				}
				catch(const std::string& Str){
					dxo.draw(Pint(0, 0), dxoStr(std::string("***ERROR***") + Str));
				}

				return 0;
			}
		};
		//�p�P�b�g�̗����`�悷��`��֐��N���X
		template<typename iterator>
		class dxFnPacketList :public dxFn{
			using datum = message::datum;
			using packet = message::packet;

			typedef std::pair<packet::no_type, datum::no_type> seltype;
		private:
			iterator Begin;									//PacketLog�̐擪
			iterator End;									//PacketLog�̖���
			const packet* pInPacket;						//�ŐV����Packet
			const packet* pOutPacket;						//�ŐV�o��Packet
			const dxRGB& InputClr;							//���͑��̐F
			const dxRGB& OutputClr;							//�o�͑��̐F
			const seltype& SelData;							//�I������Ă���f�[�^ Log���肵�Ă��Ȃ��f�[�^��clocl::time_point::max
			const std::vector<datum::id_type>& SelID;		//ID�I��\�����Ɏg�p
		public:
			boost::signals2::signal<void(seltype)> signal_pushedDatum;		//�I������Ă���f�[�^���ύX���ꂽ�ꍇ�ɔ��M
		public:
			dxFnPacketList(iterator Begin_, iterator End_, const packet* pInPacket_, const packet* pOutPacket_, const seltype& SelData_, const std::vector<datum::id_type>& SelID_, const dxRGB& InputClr_ = CLR::SoftRed, const dxRGB& OutputClr_ = CLR::SoftBlue)
				:Begin(std::move(Begin_))
				, End(std::move(End_))
				, pInPacket(pInPacket_)
				, pOutPacket(pOutPacket_)
				, SelData(SelData_)
				, SelID(SelID_)
				, InputClr(InputClr_)
				, OutputClr(OutputClr_){}
			int fndraw(dxO& dxo){
				/*ScrlFrame����*/
				try{
					//�`��ʒu�m��
					dxO::Acs<dxScrlFrame> Acs(dxo);

					//ID�̑I�����s���Ă��Ȃ��ꍇ
					if(SelID.size() == 0){
						int ViewPos = Acs().viewpos.y;
						int ViewSize = Acs().size.y;

						//�`��͈͂Ɋ܂܂��packet��iterator���v�Z
						auto DBegin = Begin + std::min(std::max(0, (-ViewPos - ViewSize) / 25 - 1), End - Begin);
						auto DEnd = Begin + std::min(std::max(0, (-ViewPos) / 25 + 1), End - Begin);

						//�ŉ����̃f�[�^�̕`��ʒu�ƂȂ�悤�ɏ����n�ݒ�
						int DrawPos = -25 * ((DBegin - Begin) + 1);

						//�`��ʒu�̂ݕ`��
						for(auto pitr = DBegin; pitr != DEnd; ++pitr){
							dxRGB Clr;
							if(pitr->first)Clr = InputClr;
							else Clr = OutputClr;

							int DrawXPos = 125;

							//�����`��
							dxo.draw(Pint(0, DrawPos), dxoStrP(Pint(120, 25), time_to_hms(pitr->second.Time), pitr->second.ErrFlag ? CLR::LightSoftRed : CLR::LightGray));

							//Packet���f�[�^�����ɕ`��
							for(auto itr = pitr->second.Data.begin(); itr != pitr->second.Data.end(); ++itr){
								bool IsSelected = (SelData.first == pitr->second.No && SelData.second == itr->No);
								if(dxo.draw(Pint(DrawXPos, DrawPos), dxoCatDatum(Pint(70, 20), *itr, 2, Clr, IsSelected)) < 0){
									if(IsSelected)signal_pushedDatum(seltype(0, 0));
									else signal_pushedDatum(seltype(pitr->second.No, itr->No));
								}
								DrawXPos += 75;
							}
							DrawPos -= 25;
						}

						//�`��ʒu��{���̈ʒu�Ɉړ�
						DrawPos = -25 * ((End - Begin) + 1);

						//OutPacket��
						if(pOutPacket != nullptr){
							//�`��͈͓��̎�
							if(ViewPos <= DrawPos + 20 && DrawPos < ViewPos + ViewSize){
								dxo.draw(Pint(0, DrawPos), dxoTriangle(Pint(0, 20), Pint(20, 10), OutputClr));
								int DrawXPos = 125;
								//Packet���f�[�^�����ɕ`��
								for(auto itr = pOutPacket->Data.begin(); itr != pOutPacket->Data.end(); ++itr){
									bool IsSelected = (SelData.first == 0 && SelData.second == itr->No);
									if(dxo.draw(Pint(DrawXPos, DrawPos), dxoCatDatum(Pint(70, 20), *itr, 2, OutputClr, IsSelected)) < 0){
										if(IsSelected)signal_pushedDatum(seltype(0, 0));
										else signal_pushedDatum(seltype(0, itr->No));
									}
									DrawXPos += 75;
								}
							}
							DrawPos -= 25;
						}

						//InPacket��
						if(pInPacket != nullptr){
							//�`��͈͓��̎�
							if(ViewPos <= DrawPos + 20 && DrawPos < ViewPos + ViewSize){
								dxo.draw(Pint(0, DrawPos), dxoTriangle(Pint(0, 20), Pint(20, 10), InputClr));
								int DrawXPos = 125;
								//Packet���f�[�^�����ɕ`��
								for(auto itr = pInPacket->Data.begin(); itr != pInPacket->Data.end(); ++itr){
									bool IsSelected = (SelData.first == 0 && SelData.second == itr->No);
									if(dxo.draw(Pint(DrawXPos, DrawPos), dxoCatDatum(Pint(70, 20), *itr, 2, InputClr, IsSelected)) < 0){
										if(IsSelected)signal_pushedDatum(seltype(0, 0));
										else signal_pushedDatum(seltype(0, itr->No));
									}
									DrawXPos += 75;
								}
							}
							DrawPos -= 25;
						}

						//�Ō�ɕ`��ʒu�̎w����s���Ă���
						dxo.draw(Pint(0, 0), dxoNull());
						dxo.draw(Pint(0, DrawPos + 25), dxoNull());
					}//ID���I������Ă���ꍇ
					else{
						//�`��͈͂��擾
						int ViewPos = Acs().viewpos.y;
						int ViewSize = Acs().size.y;
						//�`��ʒu���s���Ȃ�ŁA�����n����X�^�[�g
						int DrawPos = -25;

						//��������Œ��ׂĂ���
						for(auto pitr = Begin; pitr != End; ++pitr){
							dxRGB Clr;
							if(pitr->first)Clr = InputClr;
							else Clr = OutputClr;

							int DrawXPos = 125;

							bool DrawFlag = false;
							bool ESCFlag = false;

							//�����`��
							dxo.draw(Pint(0, DrawPos), dxoStrP(Pint(120, 25), time_to_hms(pitr->second.Time), CLR::LightGray));

							//Packet���f�[�^�����ɕ`��
							for(auto itr = pitr->second.Data.begin(); itr != pitr->second.Data.end(); ++itr){
								//IDList�ƏƂ炵���킹�Ă݂�
								for(auto selitr = SelID.begin(); selitr != SelID.end(); ++selitr){
									//IDList��datum::ID���܂܂�Ă���ꍇ
									if(*selitr == itr->ID){
										DrawFlag = true;
										//�`��͈͂ɓ����Ă��Ȃ���΁A�`�F�b�N�������ĒE�o
										if(DrawPos < ViewPos - 25 || ViewPos + ViewSize + 25 < DrawPos){
											//Packet����for������E�o����悤�Ƀt���O�𗧂ĂĂ���
											ESCFlag = true;
										} else{
											bool IsSelected = (SelData.first == pitr->second.No && SelData.second == itr->No);
											if(dxo.draw(Pint(DrawXPos, DrawPos), dxoCatDatum(Pint(70, 20), *itr, 2, Clr, IsSelected)) < 0){
												if(IsSelected)signal_pushedDatum(seltype(0, 0));
												else signal_pushedDatum(seltype(pitr->second.No, itr->No));
											}
											DrawXPos += 75;
										}
										break;
									}
								}
								//�`��͈͊O�̏ꍇ�A�p�P�b�g���ɕ`��f�[�^�����邱�Ƃ����킩��΂����̂ŒE�o
								if(ESCFlag)break;
							}
							//�`��Ώۂ������ꍇ�A�`��ʒu���L�^
							if(DrawFlag)DrawPos -= 25;
						}

						//�Ō�ɕ`��ʒu�̎w����s���Ă���
						dxo.draw(Pint(0, 0), dxoNull());
						dxo.draw(Pint(0, DrawPos + 25), dxoNull());
					}


				}
				catch(const std::exception& Except){
					dxo.draw(Pint(5, 5), dxoStr("***ERROR***"));
					dxo.draw(Pint(5, 25), dxoStr(Except.what()));
				}
				catch(const std::string& Str){
					dxo.draw(Pint(5, 5), dxoStr("***ERROR***"));
					dxo.draw(Pint(5, 25), dxoStr(Str));
				}

				return 0;
			}
		};
		//�f�[�^�̒��g��`�悷��`��֐��N���X
		class dxFnDatumView :public dxFn{
			using datum = message::datum;
			using packet = message::packet;
		private:
			const datum* Ptr;
		public:
			dxFnDatumView(const datum* Ptr_) :Ptr(Ptr_){}
			int fndraw(dxO& dxo){
				/*ScrlFrame����*/
				try{
					//�I������Ă��Ȃ��Ƃ��ɂ́A�����\�����Ȃ��B
					if(Ptr == nullptr){
						dxo.draw(Pint(5, 5), dxoStr("No Data"));
						return 0;
					}

					int DrawPos = 5;

					//�`�敝�m��
					dxO::Acs<dxScrlFrame> Acs(dxo);
					//5�����P�ʂ�StrBlock�̉������̐�
					//	25*5-5=120pixel��StrBlock
					//	���[5+�ʒu���45=55���Œ肵�ĕK�v�Ȓ���
					int StrBlockNum = (Acs().size.x - 55) / 145;

					//�X�N���[���m��
					dxo.draw(Pint(0, 0), dxoNull());
					dxo.draw(Pint(0, 50 + 25 * StrBlockNum), dxoNull());

					//�`��@�܂���b���
					dxo.draw(Pint(5, DrawPos), dxoStr((boost::format("ID:%c Size:%d(%d) Err:%d") % Ptr->ID%Ptr->Size%Ptr->Str.size() % Ptr->ErrFlag).str()));
					DrawPos += 20;
					dxo.draw(Pint(5, DrawPos), dxoStr(time_to_hms(Ptr->Time)));

					//���ɕ�������
					int DrawPosX = 5;
					unsigned int Cnt = 0;
					for(auto itr = Ptr->Str.begin(); itr != Ptr->Str.end(); ++itr){
						if((Cnt % (StrBlockNum * 5)) == 0){
							DrawPos += 45;
							DrawPosX = 5;
							dxo.draw(Pint(DrawPosX, DrawPos + 10), dxoStr((boost::format("%4d") % Cnt).str()));
							DrawPosX += 40 + 5;	//������40pixel+��5pixel
						} else if(Cnt % 5 == 0){
							DrawPosX += 10;
						}
						dxo.draw(Pint(DrawPosX, DrawPos), dxoStr((boost::format("%02x") % static_cast<unsigned int>(static_cast<unsigned char>(*itr))).str()));
						dxo.draw(Pint(DrawPosX, DrawPos + 20), dxoStr((boost::format("%c") % (*itr)).str()));
						DrawPosX += 25;
						++Cnt;
					}

				}
				catch(const std::exception& Except){
					dxo.draw(Pint(5, 5), dxoStr("***ERROR***"));
					dxo.draw(Pint(5, 25), dxoStr(Except.what()));
				}
				catch(const std::string& Str){
					dxo.draw(Pint(5, 5), dxoStr("***ERROR***"));
					dxo.draw(Pint(5, 25), dxoStr(Str));
				}

				return 0;
			}
		};
		//dxFnPacketList��dxFnDatumView�𓝍������`��N���X
		template<typename iterator>
		class dxosPacketList_withView :public dxReObject, public hmoBox{
			using datum = message::datum;
			using packet = message::packet;
			typedef std::pair<packet::no_type, datum::no_type> seltype;
		private:
			seltype SelData;							//���ݑI������Ă���f�[�^
			bool LinkNewPacket;							//NewPacket�Ƀ����N�����邩�̃t���O					
			std::vector<datum::id_type> IDList;			//�\���̗L���𐧌䂷�邽�߂�ID�̃��X�g
			const std::vector<datum::id_type> SwIDList;	//�X�C�b�`�Ƃ��ĕ\���̗L���𐧌䂵����ID�̃��X�g
			dxRGB Clr;									//�w�i�J���[
			int DatumCtrlSize;							//Datum�R���g���[����ʂ�y�����T�C�Y
			int DatumViewSize;							//Datum�\����ʂ�y�����̃T�C�Y
		private:
			hmLib::signals::unique_connections SignalConenctions;
		private:
			void setSelData(seltype SelData_){ SelData = SelData_; }
			void setLinkNewPacket(bool LinkNewPacket_){ LinkNewPacket = LinkNewPacket_; }
			void syncDatumViewSize(int DatumViewSize_){ DatumViewSize = DatumViewSize_; }
		public:
			hmLib::inquiries::range_inquiry<iterator> inquiryPacketLog;
			//Inquiry�̒��g��begin/end���Ă�ł���̂ŁA����Inquiry�͎Q�Ǝ擾�^(T&)���ׂ�
			//	�ʏ�̃^�C�v(inquiry<T>)���g���ƁA�R�s�[���ꂽData��begin/end���g���Ă��܂��B
			hmLib::inquiries::inquiry<const packet&> inquiryNewInPacket;
			hmLib::inquiries::inquiry<const packet&> inquiryNewOutPacket;
			void slot_setSelData(boost::signals2::signal<void(seltype)>& Signal_){
				SignalConenctions(hmLib::signals::connect(Signal_, [&](seltype SelData_)->void{this->setSelData(SelData_); }));
			}
			void slot_setLinkNewPacket(boost::signals2::signal<void(bool)>& Signal_){
				SignalConenctions(hmLib::signals::connect(Signal_, [&](bool LinkNewPacket_)->void{this->setLinkNewPacket(LinkNewPacket_); }));
			}
		private:
			dxNormFrame PacketListCtrlFrame;
			dxScrlFrame PacketListFrame;
			dxScrlFrame DatumViewFrame;
		public:
			int redraw(dxO& dxo)override{
				if(!inquiryPacketLog.is_connect()){
					dxo.draw(Pint(0, 0), dxoStr("NoConnection of RangeInquiry"));
					return 0;
				}

				try{
					//�I�����Ă���Datum���擾
					const datum* pSelDatum = nullptr;
					std::pair<bool, packet> TargetP;
					datum Target;
					TargetP.second.No = SelData.first;
					Target.No = SelData.second;

					//DatumView�̐ݒ�l���ُ�łȂ��ꍇ�ADatum��`�悷�邽�߂ɑI������Ă���f�[�^���܂��T���Ă݂�
					if(0 < DatumViewSize && DatumViewSize <= size.y){
						//Packet::Time���s��l���w���Ă���Ƃ��́A���݂̃p�P�b�g����T��
						if(SelData.first == 0){
							if(inquiryNewInPacket.is_connect() && inquiryNewInPacket().Data.size()){
								auto rditr = std::equal_range(inquiryNewInPacket().Data.begin(), inquiryNewInPacket().Data.end(), Target);
								if(rditr.first != rditr.second)pSelDatum = &(*(rditr.first));
							}
							if(inquiryNewOutPacket.is_connect() && inquiryNewOutPacket().Data.size()){
								auto sditr = std::equal_range(inquiryNewOutPacket().Data.begin(), inquiryNewOutPacket().Data.end(), Target);
								if(sditr.first != sditr.second)pSelDatum = &(*(sditr.first));
							}
						}//�w�肳��Ă���p�P�b�g�����݂��Ă���ꍇ�́A���̃p�P�b�g���܂��T��
						else{
							auto pitr = std::equal_range(inquiryPacketLog.begin(), inquiryPacketLog.end(), TargetP, [](const std::pair<bool, packet>& Pair1_, const std::pair<bool, packet>& Pair2_)->bool{return Pair1_.second.No < Pair2_.second.No; });
							if(pitr.first != pitr.second){
								auto ditr = std::equal_range((*pitr.first).second.Data.begin(), (*pitr.first).second.Data.end(), Target, [&](const datum& d1, const datum& d2)->bool{return d1.No < d2.No; });
								if(ditr.first != ditr.second)pSelDatum = &(*(ditr.first));
							}
						}
					}

					//�ŐVPacket���擾
					const packet* pInPacket = nullptr;
					if(inquiryNewInPacket.is_connect()){
						if(inquiryNewInPacket().Data.size())pInPacket = &inquiryNewInPacket();
					}
					const packet* pOutPacket = nullptr;
					if(inquiryNewOutPacket.is_connect()){
						if(inquiryNewOutPacket().Data.size())pOutPacket = &inquiryNewOutPacket();
					}

					//Datum���I������Ă��Ȃ��Ƃ�
					if(pSelDatum == nullptr){
						//�p�P�b�g�`�搧��
						dxFnPacketListCtrl FnPacketListCtrl(LinkNewPacket, IDList, SwIDList);
						slot_setLinkNewPacket(FnPacketListCtrl.signal_setLinkNewPacket);
						PacketListCtrlFrame.set(FnPacketListCtrl, Pint(size.x, DatumCtrlSize), Clr);

						//�p�P�b�g�`��̐ݒ�
						auto Begin = inquiryPacketLog.begin();
						auto End = inquiryPacketLog.end();
						dxFnPacketList<iterator> FnPacketList(Begin, End, pInPacket, pOutPacket, SelData, IDList);
						slot_setSelData(FnPacketList.signal_pushedDatum);
						PacketListFrame.set(FnPacketList, Pint(size.x, size.y - DatumCtrlSize), Clr);
						PacketListFrame.resetMoveArea(ALI::all);
						//Link�t���O�������Ă���ꍇ�A�`��ʒu���C��
						if(LinkNewPacket){
							PacketListFrame.viewpos.y = -25 * (End - Begin);
							if(pOutPacket != nullptr)PacketListFrame.viewpos.y -= 25;
							if(pInPacket != nullptr)PacketListFrame.viewpos.y -= 25;
						}
						Pint MPos = dxo.GtoD(dx::getMosPos());
						if(PacketListFrame.pIn(Pint(0, DatumCtrlSize), MPos)){
							PacketListFrame.viewpos.y -= 25 * dx::getMosH()*(dx::getKey(KEY::SHIFT) > 0 ? 3 : 1);
						}

						//�`��
						dxo.draw(Pint(0, 0), PacketListCtrlFrame);
						dxo.draw(Pint(0, DatumCtrlSize), PacketListFrame);
					}//Datum���I������Ă��鎞
					else{
						//�p�P�b�g�`�搧��
						dxFnPacketListCtrl FnPacketListCtrl(LinkNewPacket, IDList, SwIDList);
						slot_setLinkNewPacket(FnPacketListCtrl.signal_setLinkNewPacket);
						PacketListCtrlFrame.set(FnPacketListCtrl, Pint(size.x, DatumCtrlSize), Clr);

						//�p�P�b�g�`��̐ݒ�
						auto Begin = inquiryPacketLog.begin();
						auto End = inquiryPacketLog.end();
						dxFnPacketList<iterator> FnPacketList(Begin, End, pInPacket, pOutPacket, SelData, IDList);
						slot_setSelData(FnPacketList.signal_pushedDatum);
						PacketListFrame.set(FnPacketList, Pint(size.x, size.y - DatumViewSize - DatumCtrlSize), Clr);
						PacketListFrame.resetMoveArea(ALI::all);
						//Link�t���O�������Ă���ꍇ�A�`��ʒu���C��
						if(LinkNewPacket){
							PacketListFrame.viewpos.y = -25 * (End - Begin);
							if(pOutPacket != nullptr)PacketListFrame.viewpos.y -= 25;
							if(pInPacket != nullptr)PacketListFrame.viewpos.y -= 25;
						}
						Pint MPos = dxo.GtoD(dx::getMosPos());
						if(PacketListFrame.pIn(Pint(0, DatumCtrlSize), MPos)){
							PacketListFrame.viewpos.y -= 25 * dx::getMosH()*(dx::getKey(KEY::SHIFT) > 0 ? 3 : 1);
						}

						//�f�[�^�`��pFrame�̐ݒ�
						dxFnDatumView FnDatumView(pSelDatum);
						DatumViewFrame.set(FnDatumView, Pint(size.x, DatumViewSize), Clr);

						//�`��
						dxo.draw(Pint(0, 0), PacketListCtrlFrame);
						dxo.draw(Pint(0, DatumCtrlSize), PacketListFrame);
						dxo.draw(Pint(0, size.y - DatumViewSize), DatumViewFrame);
					}
				}
				catch(const std::exception& Except){
					dxo.draw(Pint(5, 5), dxoStr("***ERROR***"));
					dxo.draw(Pint(5, 25), dxoStr(Except.what()));
					printfDx(Except.what());
					printfDx("\n");
				}
				catch(const std::string& Str){
					dxo.draw(Pint(5, 5), dxoStr("***ERROR***"));
					dxo.draw(Pint(5, 25), dxoStr(Str));
				}

				return 0;
			}
			dxosPacketList_withView(Pint Size_, int DatumCtrlSize_, int DatumViewSize_, const std::vector<datum::id_type>& SwIDList_, const dxRGB& Clr_)
				:hmoBox(Size_)
				, SelData(0, 0)
				, LinkNewPacket(false)
				, DatumCtrlSize(DatumCtrlSize_)
				, DatumViewSize(DatumViewSize_)
				, Clr(Clr_)
				, SwIDList(SwIDList_){
				PacketListFrame.resetMoveArea(ALI::all);
				PacketListFrame.setMoveArea(ALI::bottom | ALI::left, Pint(0, 0));
				PacketListFrame.viewpos.y = -Size_.y;
			}
		};
	}
}
#
#endif
