#include <PlayerInterface.h>

#include <BaseObject.h>
#include <Camera.h>
#include <PhysicsObject.h>
#include <RenderObject.h>
#include <Sprite.h>

#include <PlayerCommands.h>
#include <PlayerStates.h>


#define IfHasPhysics \
        if(controlTarget && controlTarget->GetPhysicsObject())

#define IfHasSprite \
        if(controlTarget && controlTarget->gfx->sprite)


PlayerInterface::PlayerInterface()
:controlTarget(0)
,initialized(0)
,currentControlSet(0)
{
        External::player = this;
		previous_dt = 0;

		//TODO: move these into object properties for the control target
		PLATFORMER_walkSpeed = 30.0f;
		PLATFORMER_MAX_walkSpeed = 30.0f;
		PLATFORMER_jumpSpeed = 10.0f;
		PLATFORMER_facing = 1;
		PLATFORMER_jumping = 0;
		PLATFORMER_jumping_counter = 0;

		player_state = 0;
}

PlayerInterface::~PlayerInterface()
{
}

void PlayerInterface::Init()
{
        initialized = 1;
        RunOnce::GenerateControlSets();
        currentControlSet = &ControlSets::default_walk_controls;
        currentControlSet_held = &ControlSets::default_walk_controls_held;
}

void PlayerInterface::Update(float dt)
{
	//save the dt
	previous_dt = dt;

	fsm::run_fsm( &player_state );

	//do linear damping on the player's movement
	IfHasPhysics
    {
		PhysicsObject* p = controlTarget->GetPhysicsObject();
        
		vec2 vel(0,0);
		p->GetLinearVelocity(vel);

		if( glm::abs( vel.x ) > Global::TINY )
		{
			vel.x *= glm::pow<float>(0.065f, dt);
            p->SetLinearVelocity(vel);
		}
	}

	IfHasSprite
	{
		Sprite* s = controlTarget->gfx->sprite;
		
		if( External::player->PLATFORMER_facing > 0 )
			s->flipped = 0;
		else
			s->flipped = 1;
	}
}

void PlayerInterface::Input(float fps)
{
        if(!currentControlSet)
        {
                std::cerr<<"Error! No controls currently bound!\n";
                return;
        }

        std::map<unsigned char, void (*)(PlayerInterface&)>::iterator iter;
        iter = currentControlSet->begin();
        for(; iter != currentControlSet->end(); ++iter)
        {
			if( Global::Keyboard::KeyPressed( iter->first ) )
                        iter->second(*this);
        }

        std::map<unsigned char, void (*)(PlayerInterface&)>::iterator iter2;
        iter2 = currentControlSet_held->begin();
        for(; iter2 != currentControlSet_held->end(); ++iter2)
        {
			if( Global::Keyboard::KeyHeld( iter2->first ) )
                        iter2->second(*this);
        }
}

void PlayerInterface::SetControlTarget(BaseObject* target)
{
        controlTarget = target;
		External::gi->GetCamera()->SetTarget(target);

		if( target && target->GetPhysicsObject() )
		{
			target->GetPhysicsObject()->SetFixedRotation(1);
		}
		
		//start the state machine running on the new target
		if( player_state )
			delete player_state;

		if( target )
			player_state = new player_start(target);
		else
			player_state = new null(0);//do nothing
}


BaseObject* const PlayerInterface::GetControlTarget()
{
        return controlTarget;
}

////////////////////////////////////////////////////////////////////////
// controls

void DoMovement(PhysicsObject* p, float dt)
{
	vec2 vel(0,0);
	p->GetLinearVelocity(vel);
	vel.x = vel.x + External::player->PLATFORMER_walkSpeed * External::player->PLATFORMER_facing * dt;
	if( glm::abs( vel.x ) > glm::abs( External::player->PLATFORMER_MAX_walkSpeed ) )
			vel.x = External::player->PLATFORMER_MAX_walkSpeed * External::player->PLATFORMER_facing;
	p->SetLinearVelocity(vel);
	p->SetAngularVelocity(0);
}

//controls for commands, used for platformer movement
void PlayerInterface::Platformer_Move_Left()
{
	PLATFORMER_facing = -1;
	if( !controlTarget )
		return;
		
	if( player_state->name() != "player_walk" && player_state->name() != "player_jump" && player_state->name() != "player_attack" )
		fsm::change_state( &player_state, new player_walk(controlTarget) );

    PhysicsObject* p = controlTarget->GetPhysicsObject();
	if( p )
	{
		DoMovement(p, previous_dt);
	}
}

void PlayerInterface::Platformer_Move_Right()
{
	PLATFORMER_facing = 1;
	if( !controlTarget )
		return;
		
	if( player_state->name() != "player_walk" && player_state->name() != "player_jump" && player_state->name() != "player_attack" )
		fsm::change_state( &player_state, new player_walk(controlTarget) );

    PhysicsObject* p = controlTarget->GetPhysicsObject();
	if( p )
	{
		DoMovement(p, previous_dt);
	}
}

void PlayerInterface::Platformer_Move_Jump()
{
	if( controlTarget && player_state->name() != "player_jump" )
		fsm::change_state( &player_state, new player_jump(controlTarget) );
}

void PlayerInterface::Platformer_Attack()
{
	if( controlTarget && player_state->name() != "player_attack")
		fsm::change_state( &player_state, new player_attack(controlTarget) );
}
