#ifndef INC_HMRMANIPULATOR
#define INC_HMRMANIPULATOR 100
#

/*=====hmrManipurator======

*/

#include "hmr.h"
#include "hmrCom.h"
#include "hmrComCtr.hpp"
#include <hmLib/hmPos.hpp>
#include <cmath>
//#define pi() 3.1415926535897932386
class hmrManipulator{
public:
	typedef short time_type;
	class servo{
	private:
		time_type MaxUs;
		time_type MinUs;
		double MaxAngle;
		double MinAngle;
		bool IsNegativeLogic;
	public:
		servo(time_type MinUs_,time_type MaxUs_,double MinAngle_,double MaxAngle_,bool IsNegativeLogic_)
			:MaxUs(MaxUs_)
			,MinUs(MinUs_)
			,MaxAngle(MaxAngle_)
			,MinAngle(MinAngle_)
			,IsNegativeLogic(IsNegativeLogic_){
		}
		//ある指定の角度Angleを実現するようなサーボのON持続時間に変換する
		time_type operator()(double Angle_)const{
			double Rate=(double)(MaxUs-MinUs)/(MaxAngle-MinAngle);
			if(IsNegativeLogic)Angle_=MaxAngle-Angle_;
			if(Angle_>MaxAngle)return MaxUs;
			else if(Angle_<MinAngle)return MinUs;
			else return (time_type)(Rate*Angle_-Rate*MinUs+MinUs);
		}
		double getMaxAngle()const{return MaxAngle;}
		double getMinAngle()const{return MinAngle;}
	};
	class manipulator{
	private:
		double Arm1;	//腕1の長さ
		double Arm2;	//腕2の長さ
		double Arm3;	//腕3の長さ
		servo Servo1;	//サーボ1
		servo Servo2;	//サーボ2
		servo Servo3;	//サーボ3
	public:
		manipulator(double Arm1_,servo Servo1_,double Arm2_,servo Servo2_,double Arm3_,servo Servo3_)
			:Arm1(Arm1_)
			,Servo1(Servo1_)
			,Arm2(Arm2_)
			,Servo2(Servo2_)
			,Arm3(Arm3_)
			,Servo3(Servo3_){}
		void getAngle(Pint Pos,double& Angle1,double& Angle2,double& Angle3)const{

			Pos.x+=static_cast<int>(Arm2-Arm3);

			double Distance=(Pos.x*Pos.x)+(Pos.y*Pos.y);
			//2,3を動かすモードと1,2を動かすモードの境界の距離
			double Border=(Arm2*Arm2)+(Arm3*Arm3)-2*Arm2*Arm3*cos(Servo3.getMaxAngle()*pi()/180.);
			//アームの付け根からの距離の2乗
			double R=(Pos.x-Arm1)*(Pos.x-Arm1)+(Pos.y*Pos.y);
			double MaxDistance=Border+Arm1*Arm1-2*Arm1*sqrt(Border)*cos(Servo3.getMaxAngle()*pi()/180.);

			//アームの長さよりも遠い時
			if(R>MaxDistance){
				double X,Y;
				X=Pos.x;
				Y=Pos.y;
				if(X!=Arm1){
					Pos.x=static_cast<int>(  Arm1+sqrt(MaxDistance/(1+(Y/((X-Arm1)*(X-Arm1)))))  );
					Pos.y=static_cast<int>(  (Y/(X-Arm1))*(Pos.x-Arm1)  );
				}else{
					Pos.x=static_cast<int>(  Arm1  );
					Pos.y=static_cast<int>(  MaxDistance  );
				}
			}

			//2,3を動かすモード
			if(Distance<=Border){	
				Angle1=0;
				if(Distance!=0){
					Angle2=acos(((Arm2*Arm2)-(Arm3*Arm3)+Distance)/(2*Arm2*sqrt(Distance)))+acos(Pos.x/sqrt(Distance));
					Angle3=acos((Arm2*Arm2+Arm3*Arm3-Distance)/(2*Arm2*Arm3));
				}
				else{
					Angle2=0;
					Angle3=0;
				}
				//単位をradianから°へ
				Angle2=Angle2*180./pi();
				Angle3=Angle3*180./pi();
			}//1,2を動かすモード
			else{					
				Angle3=Servo3.getMaxAngle();
				if(R!=0){
					Angle1=acos((Arm1-Pos.x)/sqrt(R))-acos((Arm1*Arm1+R-Border)/(2*Arm1*sqrt(R)));
					Angle2=acos((Arm1*Arm1+Border-R)/(2*sqrt(Border)*Arm1))+acos((Arm2*Arm2+Border-(Arm3*Arm3))/(2*sqrt(Border)*Arm2));
				}
				else{
					Angle1=0;
					Angle2=acos((Arm2*Arm2+Border-Arm3*Arm3)/(2*sqrt(Border)*Arm2));
				}
				//単位をradianから°へ
				Angle1=Angle1*180./pi();
				Angle2=Angle2*180./pi();
			}
		}
		//Posを実現するようなPwmのON持続時間をPwm1,Pwm2,Pwm3にそれぞれ参照渡しで返す
		void operator()(Pint Pos,time_type& Pwm1,time_type& Pwm2,time_type& Pwm3)const{
			double Angle1,Angle2,Angle3;
			getAngle(Pos,Angle1,Angle2,Angle3);
			Pwm1=Servo1(Angle1);
			Pwm2=Servo2(Angle2);
			Pwm3=Servo3(Angle3);
		}
		double getArm1()const{return Arm1;}
		double getArm2()const{return Arm2;}
		double getArm3()const{return Arm3;}
		const servo& getServo1()const{return Servo1;}
		const servo& getServo2()const{return Servo2;}
		const servo& getServo3()const{return Servo3;}
	};
private:
	const manipulator& Manipu;
public:
	hmrManipulator(const manipulator& Manipu_):Manipu(Manipu_){return;}
public:
	class ComType:public hmrCom::abtComType{
	private:
		hmrManipulator& Ref;
		bool ResetMode;
		Pdouble Pos;
		Pdouble Vary;
	public:
		ComType(hmrManipulator& Ref_)
			:hmrCom::abtComType("Manipu")
			,Ref(Ref_)
			,Pos(0,0)
			,Vary(0,0){return;}
	public:
		void reset(){ResetMode=true;}
	protected:
		virtual bool listen(const hmTime& _time,bool _errFlag,const std::string& _data){return true;}
		virtual bool talk(std::string& Str){
			time_type T1,T2,T3;
			Ref.Manipu(static_cast<Pint>(Pos),T1,T2,T3);
			Str="";
			Str.push_back(static_cast<uchar>(T1%256));
			Str.push_back(static_cast<uchar>(T1/256));
			Str.push_back(static_cast<uchar>(T2%256));
			Str.push_back(static_cast<uchar>(T2/256));
			Str.push_back(static_cast<uchar>(T3%256));
			Str.push_back(static_cast<uchar>(T3/256));

			return false;
		}
		virtual void draw(dxO& _dxo,const std::string& _data){return;}
		virtual std::string str(const std::string& _data){return "";}
	public:
		Pint getPos()const{return static_cast<Pint>(Pos);}
		Pint getVary()const{return static_cast<Pint>(Vary);}
		void work(){
			if(ResetMode){
				if(Pos.x>0.1)Vary.x=-0.1;
				else if(Pos.x<-0.1)Vary.x=+0.1;
				else Vary.x=-Pos.x;

				if(Pos.y>0.1)Vary.y=-0.1;
				else if(Pos.y<-0.1)Vary.y=+0.1;
				else Vary.y=-Pos.y;

				if(Pos.x==0 && Pos.y==0)ResetMode=false;
			}
			Pos+=Vary;
		}
	public:
		class PadCtr:public hmrPad::abtPadCtr{
		private:
			ComType& rComType;
		public:
			PadCtr(ComType& _ComType):rComType(_ComType){return;}
		private:
			virtual void work(int sw){
				rComType.TalkFlag=(sw>0);
				if(sw<0){
					rComType.Vary.set(0,0);
				}
			}
		};
		class PadCtrX:public hmrPad::abtPadCtr{
			typedef PadCtrX my_type;
		private:
			ComType& rComType;
		public:
			PadCtrX(ComType& _ComType):rComType(_ComType){return;}
		private:
			virtual void work(int sw){
				rComType.Vary.x=static_cast<double>(sw/10000.);
			}
		};
		class PadCtrY:public hmrPad::abtPadCtr{
			typedef PadCtrY my_type;
		private:
			ComType& rComType;
		public:
			PadCtrY(ComType& _ComType):rComType(_ComType){return;}
		private:
			virtual void work(int sw){
				rComType.Vary.y=static_cast<double>(sw/10000.);
			}
		};
		class ComCtr:public hmrComCtr::abtComCtr{
		private:
			hmrManipulator::ComType& rComType;
		protected:
			virtual int imdraw(dxO &dxo){
				if(dxo.draw(Pint(5,0),dxoButIO(Pint(40,20),hmstd::fStr("reset"),CLR::DarkSkyblue,rComType.ResetMode))==1){
					rComType.reset();
				}

				int buttL =  dxo.draw(Pint(50,0),dxoButLR(Pint(50,20),hmstd::fStr("%.2f",rComType.Vary.x),CLR::DarkSkyblue));
				if( buttL == 10){
					if(rComType.Vary.x>0)rComType.Vary.x = 0;
					else rComType.Vary.x = -0.1;
				}else if(buttL == 1){
					if(rComType.Vary.x<0)rComType.Vary.x = 0;
					else rComType.Vary.x = +0.1;
				}

				int buttR = dxo.draw(Pint(105,0),dxoButLR(Pint(50,20),hmstd::fStr("%.2f",rComType.Vary.y),CLR::DarkSkyblue));
				if( buttR == 10){
					if(rComType.Vary.y>0)rComType.Vary.y = 0;
					else rComType.Vary.y = -0.1;
				}else if(buttR == 1){
					if(rComType.Vary.y<0)rComType.Vary.y = 0;
					else rComType.Vary.y = +0.1;
				}

				return 0;
			}
		public:
			ComCtr(hmrManipulator::ComType &_ComType):hmrComCtr::abtComCtr(_ComType),rComType(_ComType){}
		};
		class dxfManipuStatus{
		private:
			const ComType& Ref;
		public:
			dxfManipuStatus(const ComType& Ref_):Ref(Ref_){}
		};
	};
};
#
#endif
