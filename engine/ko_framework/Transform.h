#ifndef __TRANSFORM_H
#define __TRANSFORM_H

#include <Component.h>
#include <globals.h>


BEGIN_COMPONENT( Transform )

	public:

		vec2 _scale;
		mat2 _rotation;
		vec2 _pos;

		Transform();

		const float* rotation_array() const;

		float GetAngleRadians() const;
		float GetAngle() const;
		void SyncRotateToDirection( const vec2& dir );
        void SetAngleRadian(float r);
        void SetAngle(float theta);
        void Rotate(const float theta);
        vec2 GetDir() const;

        virtual void Serialize(SerializedObject& obj);

    private:

		float _rotation_array[4];
		float _angle;

		void UpdateArray();
        void UpdateMatrix();

};

#endif