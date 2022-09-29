#include <PhysicsBodyInterface.h>
#include <PhysicsBody.h>
#include <Transform.h>

PhysicsBodyInterface::PhysicsBodyInterface()
:_parent(0)
{
}

PhysicsBodyInterface::~PhysicsBodyInterface()
{
	if( _parent )
		_parent->_child = 0;
}
		

void PhysicsBodyInterface::SetBodyTransform( const vec2& pos, float angle )
{
    if(_parent && _parent->_root_body)
		_parent->_root_body->SetTransform( Convert::c_v2(pos).to_box2d, angle );
                          //b2vec2(pos.x,pos.y), angle );
}

void PhysicsBodyInterface::SetPosition( const vec2& pos )
{
    if(_parent && _parent->_root_body)
		_parent->_root_body->SetTransform( Convert::c_v2(pos).to_box2d, _parent->_transform.GetAngleRadians() );
}

void PhysicsBodyInterface::GetPosition( vec2& out_pos ) const
{
    if(_parent && _parent->_root_body)
		out_pos = Convert::c_v2( _parent->_root_body->GetPosition() ).to_glm;
}

void PhysicsBodyInterface::GetAngle( float& out_angle ) const
{
    if(_parent && _parent->_root_body)
		out_angle = _parent->_root_body->GetAngle();
}

void PhysicsBodyInterface::MovePosition( const vec2& delta )
{
    if(_parent && _parent->_root_body)
		_parent->_root_body->SetTransform( _parent->_root_body->GetPosition() + Convert::c_v2(delta).to_box2d, _parent->_root_body->GetAngle() );
}

void PhysicsBodyInterface::SetLinearVelocity( const vec2& vel )
{
    if(_parent && _parent->_root_body)
		_parent->_root_body->SetLinearVelocity(Convert::c_v2(vel).to_box2d);
                               //b2vec2(vel.x,vel.y));
}

void PhysicsBodyInterface::GetLinearVelocity( vec2& out_vel ) const
{
    if(_parent && _parent->_root_body)
		out_vel = Convert::c_v2( _parent->_root_body->GetLinearVelocity() ).to_glm;
}

void PhysicsBodyInterface::SetAngularVelocity( float a )
{
    if(_parent && _parent->_root_body)
		_parent->_root_body->SetAngularVelocity(a);
}


void PhysicsBodyInterface::ApplyForceAtCenter( const vec2& force )
{
    if(_parent && _parent->_root_body)
		_parent->_root_body->ApplyForce(Convert::c_v2(force).to_box2d, b2vec2(0,0));
                        //b2vec2(force.x,force.y), b2vec2(0,0));
}

void PhysicsBodyInterface::SetSleepingState(bool can_sleep)
{
    if(_parent && _parent->_root_body)
	{
		_parent->_root_body->SetSleepingAllowed(can_sleep);
	}
}

bool PhysicsBodyInterface::IsAwake() const
{
    return (_parent && _parent->_root_body && _parent->_root_body->IsAwake());
}


void PhysicsBodyInterface::SetFixedRotation(bool is_fixed)
{
    if(_parent && _parent->_root_body)
	{
		_parent->_root_body->SetFixedRotation( is_fixed );
	}
}

void PhysicsBodyInterface::SetGravityScale(float scale)
{
    if(_parent && _parent->_root_body)
		_parent->_root_body->SetGravityScale(scale);
}

void PhysicsBodyInterface::GetGravityScale(float& out_scale) const
{
    if(_parent && _parent->_root_body)
		out_scale = _parent->_root_body->GetGravityScale();
}