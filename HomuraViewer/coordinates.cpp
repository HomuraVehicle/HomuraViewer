#ifndef HMLIB_COORDINATES_CPP_INC
#define HMLIB_COORDINATES_CPP_INC 100
#
/*===coordinate.cpp ===

ざひょう変換を行うクラス

hmrBufGate:v1_01/130519 amby
	ざひょう変換クラスを実装
	単純な動作チェック済み(結果は正しそう)
	なお、変換と逆変換したものをかけた際に、数値誤差がe^{-10}程度出てしまう。本来は単位行列になるはずだが…

*/


#include "coordinates.hpp"

namespace hmLib{
    namespace coordinates3D{        
		// 関数の実装部分
		angle coordinate_transfer(const position& pole_,const position& plane_){
            // 結果として得られるのは,
            //  新しい座標系で見たときのある点の位置(ベクトル)を x_newcoordinate, 古い座標系で見たときの位置を　x_oldcoordinate としたときに
            //  x_newcoordinate = transfer(angle) * x_oldcoordinate という形で関係づけられることに注意

            // ココでの計算の方針
            //  pole vector を単位ベクトルe1(新座標のx軸相当)にし，plene vector から，e1に直行する単位ベクトルe2(新座標のy軸に対応)を作る．
            //  さらに z軸方向のベクトルe3を求め，これらのベクトルを　新座標のx,y,zにするような変換 T を以下の式で求める
            //    I = T * (e1 e2 e3)　, つまり，　T^{-1} = (e1 e2 e3)
            //  結局　T　が求めるべき transfer(angle) に対応するので，その関係からangle を求めることができる．

            // 単位ベクトル計算
            position e1 = pole_;
            e1 = e1 / sqrt( e1.x*e1.x + e1.y*e1.y + e1.z*e1.z );

            // plane_　から　e2を計算
            position e2 = plane_;
            e2 = e2 - e1 * (e1.x*e2.x + e1.y*e2.y + e1.z*e2.z );
            e2 = e2 / sqrt( e2.x*e2.x + e2.y*e2.y + e2.z*e2.z );

            // 以上から e3 を外積で計算
            position e3;
            e3.x = e1.y*e2.z - e1.z*e2.y;
            e3.y = e1.z*e2.x - e1.x*e2.z;
            e3.z = e1.x*e2.y - e1.y*e2.x;

            // ここまでの結果から，T^{-1}が求まる
            transfer trans;
            trans.x[0][0] = e1.x;
            trans.x[1][0] = e1.y;
            trans.x[2][0] = e1.z;
            trans.x[0][1] = e2.x;
            trans.x[1][1] = e2.y;
            trans.x[2][1] = e2.z;
            trans.x[0][2] = e3.x;
            trans.x[1][2] = e3.y;
            trans.x[2][2] = e3.z;

            // この変換行列のroll pitch yaw を求める
            angle invAngle;
            position invPos;
            trans.getAngleAndPosition(invAngle, invPos);

            // 最後に結果を逆にする
            angle ans = ~invAngle;
            return ans;
        }
	
		position::position(const polar& Pol_){
			x=Pol_.r*sin(Pol_.theta)*cos(Pol_.phi);
			y=Pol_.r*sin(Pol_.theta)*sin(Pol_.phi);
			z=Pol_.r*cos(Pol_.theta);
		}
		polar::polar(const position& Pos_){
			r=sqrt(Pos_.x*Pos_.x+Pos_.y*Pos_.y+Pos_.z*Pos_.z);
			theta=acos(Pos_.z/r);
			phi=atan2(Pos_.y, Pos_.x);
		}

		//軸変換合成
        const angle& angle::operator&=(const angle& Agl_){
            transfer compositeTrans;
            // 自分自身で変換してから，目的の変換関数を掛け合わせる
            compositeTrans = transfer(Agl_) * transfer(*this);
            // 角度変化を計算
            angle angle_;
            position pos_;
            compositeTrans.getAngleAndPosition(angle_, pos_);

            // 代入
            roll = angle_.roll;
            pitch = angle_.pitch;
            yaw = angle_.yaw;

            return *this;
        }
		//逆軸変換
        const angle angle::operator~()const{
            //逆軸変換は結局，R(roll） R(pitch), R(yaw) の変換を逆にやることなので，
            // x_inertial =  R(roll) * R(pitch) * R(yaw) * x_robot
            // これを，
            // x_robot = R(-yaw) * R(-pitch) * R(-roll) * x_inertial
            // として．この変換行列を求めることと同値
            transfer invTrans;
            invTrans = transfer(angle(0., 0., -yaw)) * transfer(angle(0., -pitch, 0.)) * transfer(angle(-roll, 0., 0.));
            // 角度変化を計算
            angle angle_;
            position pos_;
            invTrans.getAngleAndPosition(angle_, pos_);

            // 代入
            return angle_;
        }
		//座標変換
		const position& position::operator*=(const angle& Agl_){
			*this = transfer(Agl_) * (*this);
			return *this;
		}

	}
}
#
#endif
