#ifndef __PHYSICS_BODY_INTERFACE_H
#define __PHYSICS_BODY_INTERFACE_H

#include <globals.h>

class PhysicsBody;

class PhysicsBodyInterface
{
	friend class PhysicsBody;

		PhysicsBody* _parent;

	public:

		PhysicsBodyInterface();
		~PhysicsBodyInterface();
		
		void SetPhysicsBody(PhysicsBody* parent);

		void SetBodyTransform( const vec2& pos, float angle );
		void SetPosition( const vec2& pos );
		
		void GetPosition( vec2& out_pos ) const;
		void GetAngle( float& out_angle ) const;

		void MovePosition( const vec2& delta );

		void SetLinearVelocity( const vec2& vel );
		void GetLinearVelocity( vec2& out_vel ) const;
		void SetAngularVelocity( float a );

		void ApplyForceAtCenter( const vec2& force );

		void SetSleepingState(bool can_sleep);
		bool IsAwake() const;

		void SetFixedRotation(bool is_fixed);
		void SetGravityScale(float scale);
		void GetGravityScale(float& out_scale) const;
};

#endif