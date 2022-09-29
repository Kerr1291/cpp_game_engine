#include <PlayerStates.h>

#include <Physics.h>
#include <PlayerInterface.h>
#include <audio.h>

#include <BaseObject.h>
#include <RenderObject.h>
#include <PhysicsObject.h>
#include <Sprite.h>

/////////////////////////////////
//start state

state* player_start::run()
{
	return new player_idle(o);
}

/////////////////////////////////
//idle state

state* player_idle::run()
{
	Sprite* s = o->GetRenderObject()->sprite;
	if( s )
	{
		s->SetAnimationRate( 1.0f );
		s->SetAnimation( 0, 0 );
	}

	return this;
}

/////////////////////////////////
//walk state

state* player_walk::run()
{
	Sprite* s = o->GetRenderObject()->sprite;
    PhysicsObject* p = o->GetPhysicsObject();
	if(!s || !p)
		return new player_start(o);

	vec2 vel(0,0);
	p->GetLinearVelocity(vel);

	if( s )
	{
		s->SetAnimation( 1, 0 );
		s->SetAnimationDirection( 1 );
	}

	float speed = glm::abs( vel.x );
	s->SetAnimationRate( 0.5f + (speed*10.5f) / External::player->PLATFORMER_MAX_walkSpeed );

	if( speed < 1.0f )
			return new player_idle(o);

	return this;
}

/////////////////////////////////
//jump state

void player_jump::init()
{
	state::init();

	Sprite* s = o->GetRenderObject()->sprite;
    PhysicsObject* p = o->GetPhysicsObject();
	if(!s || !p)
		return;

	s->SetAnimationDirection( 1 );
	s->SetAnimationRate( 1.0f );

	if( External::player->PLATFORMER_jumping )
	{
		s->SetAnimation( 2,0 );
		return;
	}

	s->SetAnimation( 2 );

	vec2 vel(0,0);
	p->GetLinearVelocity(vel);

	vec2 ground_check = o->t.loc + vec2(0,-0.1f);
	External::physics->PickWorld( ground_check );
	std::vector< b2Body* >* bodies = External::physics->GetPickedBodiesList();
	if( !bodies->empty() )
	{
		External::physics->ClearPicked();
		vel.y = vel.y + External::player->PLATFORMER_jumpSpeed;
		External::audio->PlaySound("./audio/jump1.wav");
	}

	p->SetLinearVelocity(vel);
	p->SetAngularVelocity(0);

	External::player->PLATFORMER_jumping = 1;
}

state* player_jump::run()
{
	Sprite* s = o->GetRenderObject()->sprite;
    PhysicsObject* p = o->GetPhysicsObject();

	if(!s || !p)
		return new player_start(o);

	vec2 vel(0,0);
	p->GetLinearVelocity(vel);

	if( s->CurrentAnimationComplete() && vel.y <= 0 )
	{
		vec2 ground_check = o->t.loc + vec2(0,-0.1f);
		External::physics->PickWorld( ground_check );
		std::vector< b2Body* >* bodies = External::physics->GetPickedBodiesList();
		if( !bodies->empty() )
		{
			External::physics->ClearPicked();
			if( glm::abs( vel.x ) > Global::TINY )
			{
				External::player->PLATFORMER_jumping=0;
				return new player_walk(o);
			}
			else
			{
				External::player->PLATFORMER_jumping=0;
				return new player_idle(o);
			}
		}
	}

	return this;
}


/////////////////////////////////
//attack state

void player_attack::init()
{
	state::init();

	Sprite* s = o->GetRenderObject()->sprite;

	if( s && s->GetActiveAnimationName() != "attack" )
    {
        s->SetAnimation( "attack", 1 );
		s->SetAnimationDirection( 1 );
		s->SetAnimationRate( 1.0f );
    }

	vec2 spawnPoint = o->t.loc;
	spawnPoint.x += 0.5f * External::player->PLATFORMER_facing;
	External::physics->PickWorld( spawnPoint );
	std::vector< b2Body* >* bodies = External::physics->GetPickedBodiesList();
	for(unsigned i = 0; i < bodies->size(); ++i)
	{
		b2vec2 force = b2vec2(20.0f * -External::player->PLATFORMER_facing, 0);
		(*bodies)[i]->ApplyForce(force, b2vec2(0,0));
	}
	External::physics->ClearPicked();
}

state* player_attack::run()
{
	Sprite* s = o->GetRenderObject()->sprite;
    PhysicsObject* p = o->GetPhysicsObject();

	if(!s || !p)
		return new player_start(o);

	if( o->GetRenderObject()->sprite->CurrentAnimationComplete() )
	{
		if( External::player->PLATFORMER_jumping )
		{
			vec2 ground_check = o->t.loc + vec2(0,-0.1f);
			External::physics->PickWorld( ground_check );
			std::vector< b2Body* >* bodies = External::physics->GetPickedBodiesList();
			if( bodies->empty() )
			{
				return new player_jump(o);
			}
			External::physics->ClearPicked();
			External::player->PLATFORMER_jumping=0;
		}

		vec2 vel(0,0);
		p->GetLinearVelocity(vel);

		if( glm::abs( vel.x ) > Global::TINY )
		{
			return new player_walk(o);
		}
		else
		{
			return new player_idle(o);
		}
	}

	return this;
}

//
/////////////////////////////////
