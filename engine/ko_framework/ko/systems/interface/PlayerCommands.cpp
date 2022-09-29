#include <PlayerCommands.h>

namespace ControlSets
{
std::map<unsigned char, void (*)(PlayerInterface&)> default_controls;
std::map<unsigned char, void (*)(PlayerInterface&)> default_controls_held;
std::map<unsigned char, void (*)(PlayerInterface&)> default_walk_controls;
std::map<unsigned char, void (*)(PlayerInterface&)> default_walk_controls_held;
std::map<unsigned char, void (*)(PlayerInterface&)> current_controls;
std::map<unsigned char, void (*)(PlayerInterface&)> current_controls_held;
std::map<unsigned char, void (*)(PlayerInterface&)> menu_controls;
}

typedef unsigned char keytype;

namespace Default
{
		//platformer controls
		const keytype MOVE_LEFT = 'a';
		const keytype MOVE_RIGHT = 'd';
		const keytype MOVE_JUMP = 'w';
		const keytype MOVE_JUMP2 = ' ';
		const keytype ATTACK = 'q';
		const keytype ATTACK2 = 'e';
}

namespace PlayerCommands
{	
        GeneratePlayerCommand( Platformer_Move_Left )
        GeneratePlayerCommand( Platformer_Move_Right )
        GeneratePlayerCommand( Platformer_Move_Jump )
        GeneratePlayerCommand( Platformer_Attack )
}

namespace RunOnce
{
        void GenerateControlSets()
        {
				/* //old scheme
                MapKey( default_controls_held, Default::FORWARD, MoveForward )
                MapKey( default_controls_held, Default::TURNLEFT, TurnLeft )
                MapKey( default_controls_held, Default::TURNRIGHT, TurnRight )
                MapKey( default_controls_held, Default::SLOW, SlowDown )
               
                MapKey( default_walk_controls_held, Default::FORWARD, Walk_MoveForward )
                MapKey( default_walk_controls_held, Default::TURNLEFT, Walk_TurnLeft )
                MapKey( default_walk_controls_held, Default::TURNRIGHT, Walk_TurnRight )
                MapKey( default_walk_controls_held, Default::BACKWARD, Walk_MoveBackward )
				*/
				
                MapKey( default_controls_held, Default::MOVE_LEFT, Platformer_Move_Left )
                MapKey( default_controls_held, Default::MOVE_RIGHT, Platformer_Move_Right )
                MapKey( default_controls, Default::MOVE_JUMP, Platformer_Move_Jump )
                MapKey( default_controls, Default::MOVE_JUMP2, Platformer_Move_Jump )
                MapKey( default_controls_held, Default::ATTACK, Platformer_Attack )
                MapKey( default_controls_held, Default::ATTACK2, Platformer_Attack )
				
                MapKey( default_walk_controls_held, Default::MOVE_LEFT, Platformer_Move_Left )
                MapKey( default_walk_controls_held, Default::MOVE_RIGHT, Platformer_Move_Right )
                MapKey( default_walk_controls, Default::MOVE_JUMP, Platformer_Move_Jump )
                MapKey( default_walk_controls, Default::MOVE_JUMP2, Platformer_Move_Jump )
                MapKey( default_walk_controls_held, Default::ATTACK, Platformer_Attack )
                MapKey( default_walk_controls_held, Default::ATTACK2, Platformer_Attack )
        }
}

