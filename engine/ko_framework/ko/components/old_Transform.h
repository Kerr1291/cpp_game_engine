#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <globals.h>
#include <tweakbar.h>

namespace DEPRECATED__
{

namespace TB_Transform
{		
	void TW_CALL SetRotation(const void* data, void* transform);
	void TW_CALL GetRotation(void* data, void* transform);
}

struct Transform
{
        vec2 scale;
        mat2 rot;
        vec2 loc;
		vec2& pos;

        private:
         float r_array[4];
         void UpdateArray()
         {
            r_array[0] = rot[0][0];
            r_array[1] = rot[1][0];
            r_array[2] = rot[0][1];
            r_array[3] = rot[1][1];
         }
        public:

        inline const float* rot_array() const {
            return r_array;
        }

        inline float GetAngleRadians() const { return angle * Global::PI / 180.0f; }
        inline float GetAngle() const { return angle; }

        Transform()
        :scale( vec2(1.0f,1.0f) )
		,rot( mat2(1.0f) )
		,loc( vec2(0,0) )
		,pos( loc )
        ,angle(0)
        {
			UpdateArray();
        }

        inline void SyncRotateToDirection( const vec2 dir )
        {
                //find angle between up vector and dir, then use SetRotate with that angle
                vec2 up = vec2(0,1);
                float dot_result = glm::dot(up, dir);
                float length = dir.length();
                float radian_angle = glm::acos( dot_result / length );
                SetAngleRadian( radian_angle );
        }

        //inline b2vec2 GetDir()
        //{
        //        vec2 up = vec2(0,1);
        //        vec2 out = rot * up;
        //        return b2vec2(out.x, out.y);
        //}

        template <typename T>
        inline T GetDir() const
        {
                vec2 up = vec2(0,1);
                vec2 out = rot * up;
                return T(out.x, out.y);
        }


        inline void SetAngleRadian(float r)
        {
                const float ANGLE = r *  180.0f / Global::PI;
                angle = ANGLE;
                UpdateMatrix();
        }

        inline void SetAngle(float theta)
        {
                if( theta < 0.0f ) theta += 360.0f;
                if( theta > 360.0f ) theta -= 360.0f;

                angle = theta;
                UpdateMatrix();
        }

        inline void Rotate(const float theta)
        {
                angle += theta;
                UpdateMatrix();
        }

		void ExposeToTweakbar( TwBar* bar, const std::string& parent )
		{
			tb_pos_name = parent + "pos";
			tb_rot_name = parent + "rot";
			tb_scale_name = parent + "scale"; 
			
			TwAddVarRW(bar, tb_pos_name.c_str(), TW_TYPE_VEC2, &loc, (" group='"+parent+"' label='Position'").c_str() );
			TwAddVarCB(bar, tb_rot_name.c_str(), TW_TYPE_FLOAT, TB_Transform::SetRotation, TB_Transform::GetRotation, this, (" group='"+parent+"' label='Rotation'").c_str() );
			TwAddVarRW(bar, tb_scale_name.c_str(), TW_TYPE_VEC2, &scale, (" group='"+parent+"' label='Scale'").c_str() );
		}

		void RemoveFromTweakbar( TwBar* bar, const std::string& parent ) const
		{
			TwRemoveVar(bar, tb_pos_name.c_str());
			TwRemoveVar(bar, tb_rot_name.c_str());
			TwRemoveVar(bar, tb_scale_name.c_str());
		}

        private:

			std::string tb_pos_name;
			std::string tb_rot_name;
			std::string tb_scale_name;

            float angle;
            inline void UpdateMatrix()
            {
                if( angle < 0.0f ) angle += 360.0f;
                if( angle > 360.0f ) angle -= 360.0f;

                const float RADIANS = angle * Global::PI / 180.0f;
                float cosx = glm::cos( RADIANS );
                float sinx = glm::sin( RADIANS );

                rot[0][0] = cosx;
                rot[0][1] = sinx;
                rot[1][0] = -sinx;
                rot[1][1] = cosx;
                UpdateArray();
            }
};

}

#endif
