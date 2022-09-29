#include <SystemInterface.h>
#include <PlayerInterface.h>
#include <BaseGame.h>
#include <Camera.h>


SystemInterface::SystemInterface()
:_game(0)
,_system_fps(0.016)
,_systems( 0 )
,_game_system_interface( *this )
{
}

SystemInterface::~SystemInterface()
{
	delete _game;
	delete _systems;
}
				
BaseGame& SystemInterface::GetGame()
{
	return *_game;
}

void SystemInterface::Initialize()
{
	//No game defined! Kill the program with an error 
	if( !__make_game__ )
	{
		std::cerr<<"Fatal error: no game defined. \nDid you remember to call the MAKE_GAME( ClassName ) macro after defining your game?\n";
		exit(-1);
	}

	_systems = new Systems();

	//Init all the systems
	_systems->_editor.Init();
    _systems->_renderer.Init();
    _systems->_physics.Init();
	_systems->_audio.Init();
	//_systems->_logic.Init();
	
	//Create our game!
	_game = __make_game__->Create(&_game_system_interface);
	_game->Startup();
}

void SystemInterface::Update(double fps)
{
	_systems->_audio.Update(fps);

	//_systems->_logic.Update(fps);

	_game->Main(fps);

    _systems->_physics.Update(fps);

    _systems->_gui.Update();
    _systems->_gui.Cleanup();
}

void SystemInterface::ProcessCommands()
{
    //Update the editor logic
    _systems->_editor.Update( _system_fps );
	_engine_commands.Process();
}

void SystemInterface::Draw()
{
	_systems->_renderer.RenderMain();

    if(Global::GetDebugMode())
        Debug::Internal::PrintStrings();

	if(_systems->_physics.debugDrawEnabled)
        _systems->_physics.DoDebugDraw();
	
	_systems->_editor.Draw();
    _systems->_gui.Render();
}

void SystemInterface::Cleanup()
{
	//_systems->_logic.Cleanup();
}


void SystemInterface::NotifyActiveCameraWindowResize()
{
	if( _game->GetCurrentCamera() )
	{
		_game->GetCurrentCamera()->UpdateCameraMatrix();
	}
}