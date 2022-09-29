#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include <globals.h>

struct Transform;
struct GraphicData;
class BaseObject;

struct PhysicsObject
{
	public:
		BaseObject& parent;
        Transform& transform;
		
	private:
        b2Body* root_body;
        unsigned num_fixtures;

	public:
        PhysicsObject(BaseObject* b);
        ~PhysicsObject();

		void SetBodyTransform( const vec2& pos, float angle );
		void SetPosition( const vec2& pos );
		
		void GetPosition( vec2& out_pos ) const;
		void GetAngle( float& out_angle ) const;

		void MovePosition( const vec2& delta );

		void SetLinearVelocity( const vec2& vel );
		void GetLinearVelocity( vec2& out_vel ) const;
		void SetAngularVelocity( float a );

		void ApplyForceAtCenter( const vec2& force );

		unsigned GetNumFixtures() const;
		b2Fixture* GetFixture(unsigned fixture_num) const;

		void SyncParentTransform();
		void SetSleepingState(truth can_sleep);
		truth IsAwake() const;

		void SetFixedRotation(truth is_fixed);
		void SetGravityScale(float scale);
		void GetGravityScale(float& out_scale) const;

		void CreateRootBody(b2BodyType bodytype);
		void AddFixture(std::vector<b2vec2> bounds);
		
		void ExposeToTweakbar( TwBar* bar, const std::string& parent );
		void RemoveFromTweakbar( TwBar* bar, const std::string& parent );
};

#endif

