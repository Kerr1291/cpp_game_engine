#include <Transform.h>


namespace new_component
{
	Transform::Transform()
    :_scale( vec2(1.0f,1.0f) )
	,_rotation( mat2(1.0f) )
	,_pos( vec2(0,0) )
    ,_angle(0)
    {
		UpdateArray();
    }
	
	const float* Transform::rotation_array() const 
	{
		return _rotation_array;
    }

    float Transform::GetAngleRadians() const 
	{ 
		return _angle * Global::PI / 180.0f; 
	}

    float Transform::GetAngle() const 
	{ 
		return _angle; 
	}

	void Transform::SyncRotateToDirection( const vec2& dir )
    {
        //find angle between up vector and dir, then use SetRotate with that angle
        vec2 up = vec2(0,1);
        float dot_result = glm::dot(up, dir);
        float length = dir.length();
        float radian_angle = glm::acos( dot_result / length );
        SetAngleRadian( radian_angle );
    }

    void Transform::SetAngleRadian(float r)
    {
        const float k_angle = r *  180.0f / Global::PI;
        _angle = k_angle;
        UpdateMatrix();
    }

    void Transform::SetAngle(float theta)
    {
        if( theta < 0.0f ) theta += 360.0f;
        if( theta > 360.0f ) theta -= 360.0f;

        _angle = theta;
        UpdateMatrix();
    }

    void Transform::Rotate(const float theta)
    {
        _angle += theta;
        UpdateMatrix();
    }
		
    vec2 Transform::GetDir() const
    {
        vec2 up = vec2(0,1);
        vec2 out = _rotation * up;
        return vec2(out.x, out.y);
    }


	void Transform::Serialize(SerializedObject& obj)
	{
		float scale[2] = {1.0f, 1.0f};
		float pos[2] = {1.0f, 1.0f};
		float angle = 0.0f;

		SERIALIZE_MEMBER( obj, "Scale", scale )
		SERIALIZE_MEMBER( obj, "Position", pos )
		SERIALIZE_MEMBER( obj, "Angle", angle )

		if( obj.HasValue("Scale") )
			_scale = vec2( scale[0], scale[1] );
		
		if( obj.HasValue("Angle") )
			SetAngle( angle );

		if( obj.HasValue("Position") )
			_pos = vec2( pos[0], pos[1] );
	}

	void Transform::UpdateArray()
	{
		_rotation_array[0] = _rotation[0][0];
		_rotation_array[1] = _rotation[1][0];
		_rotation_array[2] = _rotation[0][1];
		_rotation_array[3] = _rotation[1][1];
	}

	void Transform::UpdateMatrix()
	{
		if( _angle < 0.0f ) _angle += 360.0f;
		if( _angle > 360.0f ) _angle -= 360.0f;

		const float k_radians = _angle * Global::PI / 180.0f;
		const float k_cosx = glm::cos( k_radians );
		const float k_sinx = glm::sin( k_radians );

		_rotation[0][0] = k_cosx;
		_rotation[0][1] = k_sinx;
		_rotation[1][0] = -k_sinx;
		_rotation[1][1] = k_cosx;

		UpdateArray();
	}

}
