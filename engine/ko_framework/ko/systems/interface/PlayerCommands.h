#ifndef PLAYERCOMMANDS_H
#define PLAYERCOMMANDS_H

#include <globals.h>
#include <PlayerInterface.h>

#define DefinePlayerCommand( FunctionName ) \
	void FunctionName( PlayerInterface& pi )

#define GeneratePlayerCommand( FunctionName ) \
	void FunctionName( PlayerInterface& pi ) { pi.FunctionName(); }

#define MapKey( Map, Key, Command )                                                                             \
        if( ControlSets::Map.find( Key ) == ControlSets::Map.end() )            \
{       ControlSets::Map[ Key ] = &PlayerCommands::Command;     }                               \
        else {                                                                                                                          \
        std::cerr<<"Error! Key: "<<Key <<" already present in map!\n"; }

namespace ControlSets
{
extern
std::map<unsigned char, void (*)(PlayerInterface&)> default_controls;  //for keys pressed
extern
std::map<unsigned char, void (*)(PlayerInterface&)> default_controls_held; //for keys held down

extern
std::map<unsigned char, void (*)(PlayerInterface&)> default_walk_controls;
extern
std::map<unsigned char, void (*)(PlayerInterface&)> default_walk_controls_held;

extern
std::map<unsigned char, void (*)(PlayerInterface&)> current_controls;
extern
std::map<unsigned char, void (*)(PlayerInterface&)> current_controls_held;

extern
std::map<unsigned char, void (*)(PlayerInterface&)> menu_controls;
}

namespace PlayerCommands
{
        DefinePlayerCommand( Platformer_Move_Left );
        DefinePlayerCommand( Platformer_Move_Right );
        DefinePlayerCommand( Platformer_Move_Jump );
        DefinePlayerCommand( Platformer_Attack );
}
/* //this used to be in the .cpp
namespace PlayerCommands
{
        //All commands take a reference to the player interface called 'pi'
        Command( MoveForward ){pi.MoveForward();}
        Command( TurnLeft ){pi.TurnLeft();}
        Command( TurnRight ){pi.TurnRight();}
        Command( SlowDown ){pi.Slow();}

        Command( Walk_MoveForward ){pi.Walk_MoveForward();}
        Command( Walk_MoveBackward ){pi.Walk_MoveBackward();}
        Command( Walk_TurnLeft ){ pi.Walk_TurnLeft(); }
        Command( Walk_TurnRight ) { pi.Walk_TurnRight(); }

		Command( Platformer_Move_Left ) { pi.Platformer_Move_Left; }
        Command( Platformer_Move_Right ) { pi.Platformer_Move_Right; }
        Command( Platformer_Move_Jump ) { pi.Platformer_Move_Jump; }
        Command( Platformer_Attack ) { pi.Platformer_Attack; }

}
*/

namespace RunOnce
{
        void GenerateControlSets();
}


#endif

