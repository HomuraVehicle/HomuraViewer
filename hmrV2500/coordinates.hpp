#ifndef HMLIB_COORDINATES_INC
#define HMLIB_COORDINATES_INC 101
#
/*===coordinate.hpp ===

ざひょう変換を行うクラス

hmrBufGate:v1_01/130519 amby
	ざひょう変換クラスを実装
	単純な動作チェック済み(結果は正しそう)
	なお、変換と逆変換したものをかけた際に、数値誤差がe^{-10}程度出てしまう。本来は単位行列になるはずだが…

*/


#include<cmath>
#include<boost/operators.hpp>
namespace hmLib{
    namespace coordinates3D{
		struct transfer;
		struct polar;
		struct angle;
		struct position;
		struct scale;

		//回転クラス
		struct angle
			:private boost::equality_comparable<angle>
			,private boost::addable<angle>
			,private boost::subtractable<angle>
			,private boost::multipliable<angle,double>
			,private boost::dividable<angle,double>
			,private boost::andable<angle>{
		public:
            double roll; //
            double pitch; // -PI/2 < pitch < PI/2
            double yaw;   //
		public:
			angle():pitch(0.),roll(0.),yaw(0.){}
			angle(const angle& Agl_):pitch(Agl_.pitch),roll(Agl_.roll),yaw(Agl_.yaw){}
			angle(angle&& Agl_):pitch(Agl_.pitch),roll(Agl_.roll),yaw(Agl_.yaw){}
			angle(double roll_,double pitch_,double yaw_):roll(roll_),pitch(pitch_),yaw(yaw_){}
		public:
			//等価演算
			bool operator==(const angle& Agl_)const{
				return pitch==Agl_.pitch && roll==Agl_.roll && yaw==Agl_.yaw;
			}
			//軸変換加算
			const angle& operator+=(const angle& Agl_){
				pitch+=Agl_.pitch;
				roll+=Agl_.roll;
				yaw+=Agl_.yaw;
				return *this;
			}
			//軸変換加算
			const angle& operator-=(const angle& Agl_){
				pitch-=Agl_.pitch;
				roll-=Agl_.roll;
				yaw-=Agl_.yaw;
				return *this;
			}
			//軸変換積算
			const angle& operator*=(const double& Val_){
				pitch*=Val_;
				roll*=Val_;
				yaw*=Val_;
				return *this;
			}
			//軸変換積算
			const angle& operator/=(const double& Val_){
				pitch/=Val_;
				roll/=Val_;
				yaw/=Val_;
				return *this;
			}
			//軸変換合成　実装部は別
            const angle& operator&=(const angle& Agl_);
			
			//逆軸変換　　実装部は別
            const angle operator~()const;

		};
		//拡大クラス
		struct scale{
		public:
			double rx;
			double ry;
			double rz;
		public:
			scale():rx(1.),ry(1.),rz(1.){}
		};
		//直交座標ベクトル
		struct position
			:private boost::equality_comparable<position>
			,private boost::addable<position>
			,private boost::subtractable<position>
			,private boost::multipliable<position,double>
			,private boost::dividable<position,double>
			,private boost::multipliable<position,angle>{
			double x;
			double y;
			double z;
		public:
			position():x(0.),y(0.),z(0.){}
			position(const position& Pos_):x(Pos_.x),y(Pos_.y),z(Pos_.z){}
			position(position&& Pos_):x(Pos_.x),y(Pos_.y),z(Pos_.z){}
			position(double x_,double y_,double z_):x(x_),y(y_),z(z_){}
			position(const polar& Pol_);
		public:
			//等価演算
			bool operator==(const position& Agl_)const{
				return x==Agl_.x && y==Agl_.y && z==Agl_.z;
			}
			//軸変換加算
			const position& operator+=(const position& Agl_){
				x+=Agl_.x;
				y+=Agl_.y;
				z+=Agl_.z;
				return *this;
			}
			//軸変換加算
			const position& operator-=(const position& Agl_){
				x-=Agl_.x;
				y-=Agl_.y;
				z-=Agl_.z;
				return *this;
			}
			//軸変換積算
			const position& operator*=(const double& Val_){
				x*=Val_;
				y*=Val_;
				z*=Val_;
				return *this;
			}
			//軸変換積算
			const position& operator/=(const double& Val_){
				x/=Val_;
				y/=Val_;
				z/=Val_;
				return *this;
			}
			//座標変換 　実装部は別
            const position& operator*=(const angle& Agl_);

		};
		
		//極座標ベクトル
		// 角殿定義
		// 　theta : z軸からの角度 （z軸からｘｙ方向を正)
		//　 phi   : x軸からの角度【右手系回転方向　正】
		//
		struct polar
			:private boost::equality_comparable<polar>
			,private boost::addable<polar>
			,private boost::subtractable<polar>
			,private boost::multipliable<polar,double>
			,private boost::dividable<polar,double>{
		public:
			double r;
			double phi;
			double theta;
		public:
			polar():r(0.),phi(0.),theta(0.){}
			polar(const polar& Pol_):r(Pol_.r),phi(Pol_.phi),theta(Pol_.theta){}
			polar(polar&& Pol_):r(Pol_.r),phi(Pol_.phi),theta(Pol_.theta){}
			polar(double r_,double phi_,double theta_):r(r_),phi(phi_),theta(theta_){}
			polar(const position& Pos_);
		public:
			//等価演算
			bool operator==(const polar& Pol_)const{
				return r==Pol_.r && phi==Pol_.phi && theta==Pol_.theta;
			}
			//軸変換加算
			const polar& operator+=(const polar& Pol_){
				r+=Pol_.r;
				phi+=Pol_.phi;
				theta+=Pol_.theta;
				return *this;
			}
			//軸変換加算
			const polar& operator-=(const polar& Pol_){
				r-=Pol_.r;
				phi-=Pol_.phi;
				theta-=Pol_.theta;
				return *this;
			}
			//軸変換積算
			const polar& operator*=(const double& Val_){
				r*=Val_;
				phi*=Val_;
				theta*=Val_;
				return *this;
			}
			//軸変換積算
			const polar& operator/=(const double& Val_){
				r/=Val_;
				phi/=Val_;
				theta/=Val_;
				return *this;
			}
		};

		//座標変換行列
		struct transfer{
        public:
			double x[4][4];
		public:
			transfer(){
				for(unsigned int r=0;r<4;++r){
					for(unsigned int c=0;c<4;++c){
						x[r][c]=(r==c?1:0);
					}
				}
			}
			//回転
            // これは　roll pitch yaw であらわされる　ロボット座標系を，慣性座標系に戻す行列
            // x_{inertial frame} = Matrix * x_{robot frame}
            transfer(const angle& Agl_){
                x[0][0] = cos(Agl_.pitch) * cos(Agl_.yaw);
                x[0][1] = - cos(Agl_.pitch) * sin(Agl_.yaw);
                x[0][2] = sin(Agl_.pitch);
                x[0][3] = 0.;

                x[1][0] = sin(Agl_.roll) * sin(Agl_.pitch) * cos(Agl_.yaw) + cos(Agl_.roll) * sin(Agl_.yaw);
                x[1][1] = - sin(Agl_.roll) * sin(Agl_.pitch) * sin(Agl_.yaw) + cos(Agl_.roll) * cos(Agl_.yaw);
                x[1][2] = - sin(Agl_.roll) * cos(Agl_.pitch);
                x[1][3] = 0.;

                x[2][0] = - cos(Agl_.roll) * sin(Agl_.pitch) * cos(Agl_.yaw) + sin(Agl_.roll) * sin(Agl_.yaw);
                x[2][1] = cos(Agl_.roll) * sin(Agl_.pitch) * sin(Agl_.yaw) + sin(Agl_.roll) * cos(Agl_.yaw);
                x[2][2] = cos(Agl_.roll) * cos(Agl_.pitch);
                x[2][3] = 0.;

                x[3][0] = 0.;
                x[3][1] = 0.;
                x[3][2] = 0.;
                x[3][3] = 1.;
            }
			//拡大
            transfer(const scale& Scl_);

            //移動
            // Pos_ で指定される方向に移動された座標系の座標を，慣性座標系に戻す行列
            //  x_{inertial frame} = Matrix * x_{robot frame}
            transfer(const position& Pos_){
                for(int i = 0;i<4;++i){
                    for(int j = 0;j<4;++j){
                        x[i][j] = (i==j?1.:0.);
                    }
                }
                x[0][3] = Pos_.x;
                x[1][3] = Pos_.y;
                x[2][3] = Pos_.z;
            }

        public:
            // 変換行列から，変換角度，平行移動距離を求める
            void getAngleAndPosition(angle& angle_, position& pos_){
                // position 計算
                pos_.x = x[0][3];
                pos_.y = x[1][3];
                pos_.z = x[2][3];

                // angle 計算
                double cos_pitch = fabs(sqrt(x[0][0]*x[0][0] +  x[0][1]*x[0][1]));

                if( cos_pitch > 0.001 )
                //  pitch角　が　+-90度でない場合
                {
                    angle_.pitch = atan2(x[0][2], cos_pitch);
                    angle_.roll = atan2(-x[1][2], x[2][2]);
                    angle_.yaw = atan2(-x[0][1], x[0][0]);
                }else
                // pitch 角が+- 90度の場合は全ての角を一意に決めることあできないので，yaw角を0にする
                {
                    angle_.pitch = atan2(x[0][2], 0.);
                    angle_.roll = atan2(x[2][1], x[1][1]);
                    angle_.yaw = 0.;
                }
                return;
            }

        public:
            //　行列演算オペレータ
            // 変換行列同士の積
            transfer operator*(const transfer& Val_){
                transfer ans;
                for(int i=0;i<4;++i){
                    for(int j=0;j<4;++j){
                        double value = 0.;
                        for(int k=0;k<4;++k){
                            value += x[i][k] * Val_.x[k][j];
                        }
                        ans.x[i][j] = value;
                    }
                }
                return ans;
            }

            // 変換行列同士の割り算（逆行列） 保留
            transfer operator/(const transfer& Val_);

            // 座標との積
            position operator*(const position& Pos_){
                position ans;
                double ansVec[4] = {0.};
                double derVec[4];
                derVec[0] = Pos_.x;
                derVec[1] = Pos_.y;
                derVec[2] = Pos_.z;
                derVec[3] = 1;

                for(int i = 0;i<4;++i){
                    for(int j=0;j<4;++j){
                        ansVec[i] += x[i][j] * derVec[j];
                    }
                }
                ans.x = ansVec[0];
                ans.y = ansVec[1];
                ans.z = ansVec[2];

                return ans;
            }

        };
		
		//軸変換の作成 poleがx軸、planeとpoleがxy平面
        angle coordinate_transfer(const position& pole_,const position& plane_);

		
	}
}
#
#endif
