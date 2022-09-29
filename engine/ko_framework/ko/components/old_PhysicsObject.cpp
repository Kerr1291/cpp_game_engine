#include <PhysicsObject.h>
#include <BaseObject.h>
#include <Transform.h>

#include <Physics.h>

#include <GraphicData.h>

//////////////////////////////////////////////
//helpers

void CreateDefaultFixture(b2Body* b)
{
	b2vec2 box[4] =
	{
		b2vec2(1, 0), b2vec2(1,1),
		b2vec2(0, 1), b2vec2(0,0)
	};

	b2PolygonShape shape;
    shape.Set( box, 4 );

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1;
    fixtureDef.friction = 0.3f;

    b->CreateFixture(&fixtureDef);
}

//
//////////////////////////////////////////////


//////////////////////////////////////////////
//tweakbar functions
namespace TB_PhysicsObject
{
	void TW_CALL tbSetGravityScale(const void* data, void* physics_object)
	{
		((PhysicsObject*)physics_object)->SetGravityScale( *(const float*)data );
	}

	void TW_CALL tbGetGravityScale(void* data, void* physics_object)
	{
		((PhysicsObject*)physics_object)->GetGravityScale( *(float*)data );
	}
}
//
//////////////////////////////////////////////

PhysicsObject::PhysicsObject(BaseObject* b)
:parent(*b)
,transform((b->t))
,root_body(0)
,num_fixtures(0)
{
    if( !b )
		throw 0;
}

PhysicsObject::~PhysicsObject()
{
        //body->DestroyFixture( bodyFixture ); //TODO: maybe use this?
        //body->SetTransform( b2Vec2(-10000,-10000),0 );
        //External::physics->world->DestroyBody(body);
	External::physics->world->DestroyBody( root_body );
}

void PhysicsObject::SetBodyTransform( const vec2& pos, float angle )
{
    if(root_body)
		root_body->SetTransform( Convert::c_v2(pos).to_box2d, angle );
                          //b2vec2(pos.x,pos.y), angle );
}

void PhysicsObject::SetPosition( const vec2& pos )
{
    if(root_body)
		root_body->SetTransform( Convert::c_v2(pos).to_box2d, transform.GetAngleRadians() );
                          //b2vec2(pos.x,pos.y), transform.GetAngleRadians() );
}

void PhysicsObject::GetPosition( vec2& out_pos ) const
{
    if(root_body)
		out_pos = Convert::c_v2( root_body->GetPosition() ).to_glm;
		//vec2( root_body->GetPosition().x, root_body->GetPosition().y );
}

void PhysicsObject::GetAngle( float& out_angle ) const
{
    if(root_body)
		out_angle = root_body->GetAngle();
}

void PhysicsObject::MovePosition( const vec2& delta )
{
    if(root_body)
		root_body->SetTransform( root_body->GetPosition() + Convert::c_v2(delta).to_box2d, root_body->GetAngle() );
                          //b2vec2(delta.x,delta.y), root_body->GetAngle() );
}

void PhysicsObject::SetLinearVelocity( const vec2& vel )
{
    if(root_body)
		root_body->SetLinearVelocity(Convert::c_v2(vel).to_box2d);
                               //b2vec2(vel.x,vel.y));
}

void PhysicsObject::GetLinearVelocity( vec2& out_vel ) const
{
    if(root_body)
		out_vel = Convert::c_v2( root_body->GetLinearVelocity() ).to_glm;
		//vec2(root_body->GetLinearVelocity().x, root_body->GetLinearVelocity().y);
}

void PhysicsObject::SetAngularVelocity( float a )
{
    if(root_body)
		root_body->SetAngularVelocity(a);
}


void PhysicsObject::ApplyForceAtCenter( const vec2& force )
{
    if(root_body)
		root_body->ApplyForce(Convert::c_v2(force).to_box2d, b2vec2(0,0));
                        //b2vec2(force.x,force.y), b2vec2(0,0));
}

unsigned PhysicsObject::GetNumFixtures() const
{
	return num_fixtures;
}

b2Fixture* PhysicsObject::GetFixture(unsigned fixture_num) const
{
	if( fixture_num >= num_fixtures )
		return 0;

	if(root_body)
	{
		unsigned index = 0;
		b2Fixture* fixture = root_body->GetFixtureList();
		while( fixture && index != fixture_num )
		{
			fixture = fixture->GetNext();
			index++;
		}
		return fixture;
	}

	return 0;
}

void PhysicsObject::SyncParentTransform()
{
    if(root_body)
	{
	    Transform* __restrict__ t = &transform;
	    BaseObject* __restrict__ p = &parent;
	    t->loc = Convert::c_v2( root_body->GetTransform().p ).to_glm;
	    t->SetAngleRadian( root_body->GetAngle() );
	    p->velocity = Convert::c_v2( root_body->GetLinearVelocity() ).to_glm;

        //transform.loc = Convert::c_v2( root_body->GetTransform().p ).to_glm;
        //transform.SetAngleRadian( root_body->GetAngle() );
        //parent.velocity = Convert::c_v2( root_body->GetLinearVelocity() ).to_glm;

        //vec2( root_body->GetTransform().p.x, root_body->GetTransform().p.y );
        //vec2( root_body->GetLinearVelocity().x, root_body->GetLinearVelocity().y );
	}
}

void PhysicsObject::SetSleepingState(truth can_sleep)
{
    if(root_body)
	{
		bool result = (can_sleep ? true : false);
		root_body->SetSleepingAllowed(result);
	}
}

truth PhysicsObject::IsAwake() const
{
    if(root_body)
	{
		if(root_body->IsAwake())
			return 1;
	}

	return 0;
}


void PhysicsObject::SetFixedRotation(truth is_fixed)
{
    if(root_body)
	{
		bool result = (is_fixed ? true : false);
		root_body->SetFixedRotation( result );
	}
}

void PhysicsObject::SetGravityScale(float scale)
{
    if(root_body)
		root_body->SetGravityScale(scale);
}

void PhysicsObject::GetGravityScale(float& out_scale) const
{
    if(root_body)
		out_scale = root_body->GetGravityScale();
}

void PhysicsObject::CreateRootBody(b2BodyType bodytype)
{
    b2BodyDef bodyDef;
    bodyDef.type = bodytype;
    bodyDef.position.Set( transform.loc.x, transform.loc.y );

	if( root_body )
	{
		External::physics->world->DestroyBody(root_body);
		root_body = 0;
	}

    root_body = External::physics->world->CreateBody(&bodyDef);
    root_body->SetTransform( b2vec2(transform.loc.x,transform.loc.y), transform.GetAngle() );
    root_body->SetUserData( &parent );

	CreateDefaultFixture(root_body);
}

void PhysicsObject::AddFixture(std::vector<b2vec2> bounds)
{
	if(!root_body)
		return;

	//destroy the default fixture
	if( num_fixtures == 0 )
	{
		b2Fixture* default_fixture = root_body->GetFixtureList();
		root_body->DestroyFixture( default_fixture );
	}

	++num_fixtures;

	b2PolygonShape shape;
    shape.Set( &bounds.front(), bounds.size() );

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1;
    fixtureDef.friction = 0.3f;

    root_body->CreateFixture(&fixtureDef);
}

void PhysicsObject::ExposeToTweakbar( TwBar* bar, const std::string& parent )
{
	TwAddSeparator(bar, ("physics_object"+parent).c_str(), (" group='"+parent+"'").c_str());
	TwAddVarCB(bar, ("GravityScale"+parent).c_str(), TW_TYPE_FLOAT, TB_PhysicsObject::tbSetGravityScale, TB_PhysicsObject::tbGetGravityScale, this, (" group='"+parent+"' label='Gravity Scale:'").c_str() );
}

void PhysicsObject::RemoveFromTweakbar( TwBar* bar, const std::string& parent )
{
	TwRemoveVar(bar, ("physics_object"+parent).c_str());
	TwRemoveVar(bar, ("GravityScale"+parent).c_str());
}
