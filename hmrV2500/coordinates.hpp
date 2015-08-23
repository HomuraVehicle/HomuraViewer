#ifndef HMLIB_COORDINATES_INC
#define HMLIB_COORDINATES_INC 101
#
/*===coordinate.hpp ===

���Ђ傤�ϊ����s���N���X

hmrBufGate:v1_01/130519 amby
	���Ђ傤�ϊ��N���X������
	�P���ȓ���`�F�b�N�ς�(���ʂ͐�������)
	�Ȃ��A�ϊ��Ƌt�ϊ��������̂��������ۂɁA���l�덷��e^{-10}���x�o�Ă��܂��B�{���͒P�ʍs��ɂȂ�͂������c

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

		//��]�N���X
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
			//�������Z
			bool operator==(const angle& Agl_)const{
				return pitch==Agl_.pitch && roll==Agl_.roll && yaw==Agl_.yaw;
			}
			//���ϊ����Z
			const angle& operator+=(const angle& Agl_){
				pitch+=Agl_.pitch;
				roll+=Agl_.roll;
				yaw+=Agl_.yaw;
				return *this;
			}
			//���ϊ����Z
			const angle& operator-=(const angle& Agl_){
				pitch-=Agl_.pitch;
				roll-=Agl_.roll;
				yaw-=Agl_.yaw;
				return *this;
			}
			//���ϊ��ώZ
			const angle& operator*=(const double& Val_){
				pitch*=Val_;
				roll*=Val_;
				yaw*=Val_;
				return *this;
			}
			//���ϊ��ώZ
			const angle& operator/=(const double& Val_){
				pitch/=Val_;
				roll/=Val_;
				yaw/=Val_;
				return *this;
			}
			//���ϊ������@�������͕�
            const angle& operator&=(const angle& Agl_);
			
			//�t���ϊ��@�@�������͕�
            const angle operator~()const;

		};
		//�g��N���X
		struct scale{
		public:
			double rx;
			double ry;
			double rz;
		public:
			scale():rx(1.),ry(1.),rz(1.){}
		};
		//�������W�x�N�g��
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
			//�������Z
			bool operator==(const position& Agl_)const{
				return x==Agl_.x && y==Agl_.y && z==Agl_.z;
			}
			//���ϊ����Z
			const position& operator+=(const position& Agl_){
				x+=Agl_.x;
				y+=Agl_.y;
				z+=Agl_.z;
				return *this;
			}
			//���ϊ����Z
			const position& operator-=(const position& Agl_){
				x-=Agl_.x;
				y-=Agl_.y;
				z-=Agl_.z;
				return *this;
			}
			//���ϊ��ώZ
			const position& operator*=(const double& Val_){
				x*=Val_;
				y*=Val_;
				z*=Val_;
				return *this;
			}
			//���ϊ��ώZ
			const position& operator/=(const double& Val_){
				x/=Val_;
				y/=Val_;
				z/=Val_;
				return *this;
			}
			//���W�ϊ� �@�������͕�
            const position& operator*=(const angle& Agl_);

		};
		
		//�ɍ��W�x�N�g��
		// �p�a��`
		// �@theta : z������̊p�x �iz�����炘��������)
		//�@ phi   : x������̊p�x�y�E��n��]�����@���z
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
			//�������Z
			bool operator==(const polar& Pol_)const{
				return r==Pol_.r && phi==Pol_.phi && theta==Pol_.theta;
			}
			//���ϊ����Z
			const polar& operator+=(const polar& Pol_){
				r+=Pol_.r;
				phi+=Pol_.phi;
				theta+=Pol_.theta;
				return *this;
			}
			//���ϊ����Z
			const polar& operator-=(const polar& Pol_){
				r-=Pol_.r;
				phi-=Pol_.phi;
				theta-=Pol_.theta;
				return *this;
			}
			//���ϊ��ώZ
			const polar& operator*=(const double& Val_){
				r*=Val_;
				phi*=Val_;
				theta*=Val_;
				return *this;
			}
			//���ϊ��ώZ
			const polar& operator/=(const double& Val_){
				r/=Val_;
				phi/=Val_;
				theta/=Val_;
				return *this;
			}
		};

		//���W�ϊ��s��
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
			//��]
            // ����́@roll pitch yaw �ł���킳���@���{�b�g���W�n���C�������W�n�ɖ߂��s��
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
			//�g��
            transfer(const scale& Scl_);

            //�ړ�
            // Pos_ �Ŏw�肳�������Ɉړ����ꂽ���W�n�̍��W���C�������W�n�ɖ߂��s��
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
            // �ϊ��s�񂩂�C�ϊ��p�x�C���s�ړ����������߂�
            void getAngleAndPosition(angle& angle_, position& pos_){
                // position �v�Z
                pos_.x = x[0][3];
                pos_.y = x[1][3];
                pos_.z = x[2][3];

                // angle �v�Z
                double cos_pitch = fabs(sqrt(x[0][0]*x[0][0] +  x[0][1]*x[0][1]));

                if( cos_pitch > 0.001 )
                //  pitch�p�@���@+-90�x�łȂ��ꍇ
                {
                    angle_.pitch = atan2(x[0][2], cos_pitch);
                    angle_.roll = atan2(-x[1][2], x[2][2]);
                    angle_.yaw = atan2(-x[0][1], x[0][0]);
                }else
                // pitch �p��+- 90�x�̏ꍇ�͑S�Ă̊p����ӂɌ��߂邱�Ƃ��ł��Ȃ��̂ŁCyaw�p��0�ɂ���
                {
                    angle_.pitch = atan2(x[0][2], 0.);
                    angle_.roll = atan2(x[2][1], x[1][1]);
                    angle_.yaw = 0.;
                }
                return;
            }

        public:
            //�@�s�񉉎Z�I�y���[�^
            // �ϊ��s�񓯎m�̐�
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

            // �ϊ��s�񓯎m�̊���Z�i�t�s��j �ۗ�
            transfer operator/(const transfer& Val_);

            // ���W�Ƃ̐�
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
		
		//���ϊ��̍쐬 pole��x���Aplane��pole��xy����
        angle coordinate_transfer(const position& pole_,const position& plane_);

		
	}
}
#
#endif
