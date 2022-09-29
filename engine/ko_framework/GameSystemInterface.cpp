#include <GameSystemInterface.h>
#include <SystemInterface.h>
#include <globals.h>


PhysicsManager& GameSystemInterface::GetPhysicsManager()
{
	return _system_interface._systems->_physics;
}

RenderManager& GameSystemInterface::GetRenderManager()
{
	return _system_interface._systems->_renderer;
}
				
AudioManager& GameSystemInterface::GetAudioManager()
{
	return _system_interface._systems->_audio;
}

void GameSystemInterface::GenerateScreenTextures()
{
	_system_interface._systems->_renderer.CreateScreenTexture();
}

GameSystemInterface::GameSystemInterface(SystemInterface& system)
:_system_interface(system)
,_audio_manager_interface(_system_interface._systems->_audio)
,_render_manager_interface(_system_interface._systems->_renderer)
,_physics_manager_interface(_system_interface._systems->_physics)
{
	External::system = this;
}

GameSystemInterface::~GameSystemInterface()
{}

AudioManagerInterface& GameSystemInterface::GetAudioManagerInterface()
{
	return _audio_manager_interface;
}
				
RenderManagerInterface& GameSystemInterface::GetRenderManagerInterface()
{
	return _render_manager_interface;
}
				
PhysicsManagerInterface& GameSystemInterface::GetPhysicsManagerInterface()
{
	return _physics_manager_interface;
}

void GameSystemInterface::AddCommand( Command* command )
{
	_system_interface._engine_commands.Add( command );
}
bool GameSystemInterface::HasPendingCommand( Command* command )
{
	return _system_interface._engine_commands.Find( command );
}

BaseGame& GameSystemInterface::GetGame()
{
	return _system_interface.GetGame();
}
