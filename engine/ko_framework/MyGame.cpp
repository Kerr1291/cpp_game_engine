#include <MyGame.h>

//helpers
#include <Parser.h>
#include <GUIBuilder.h>

//systems
#include <GameSystemInterface.h>
#include <GUIManager.h>
#include <RenderManager.h>
#include <Physics.h>
#include <audio.h>

//engine objects
#include <Camera.h>

//game specific files

MAKE_GAME( MyGame )

MyGame::MyGame()
:_renderer(GetSystemInterface().GetRenderManagerInterface())
,_physics(GetSystemInterface().GetPhysicsManagerInterface())
,_audio(GetSystemInterface().GetAudioManagerInterface())
{
	loading=0;
	inmenu=0;
}

MyGame::~MyGame()
{
	delete _current_camera;
}

void MyGame::Startup()
{
	std::cerr<<"Loading my game!\n";
	_player.Init();

	_current_camera = new Camera();
	
	unsigned w = 1280;
	unsigned h = 800;

    ////////////////////////////////////////////////
    //get config file and setup window dimensions
    if(!config.Load("./config.ini"))
    {
		std::cerr<<"Error loacating config.ini\n";
    }

	//_current_camera->ResizeWindow(w,h); //TODO: see why this causes a crash

    //config->GetValue("ScreenWidth",w);
    //config->GetValue("ScreenHeight",h);

	//Get VSync state from the config file
	int vsync = 1;
	if( config.HasValue("VSync") )
		config.GetValue("VSync", vsync);
	if( vsync )
		Global::SetVSync(true);
	else
		Global::SetVSync(false);
	
	float sfx_volume = 1.0f;
	float music_volume = 1.0f;
	config.GetValue("SoundVolume", sfx_volume);
	config.GetValue("MusicVolume", music_volume);

	_audio.SetSoundEffectVolume(sfx_volume);
	_audio.SetMusicVolume(music_volume);
}

void MyGame::Main(double fps)
{
	//some example test logic
	if( Global::Keyboard::KeyPressed('%') )
	{
		External::gui->AddNode( GUI::Widget::MakeTextBoxWindow( vec2(0,0), vec2(80,50), "hello_world", "Hello!", "Hello World!" ) );
	}

	_current_area.ClearDeleteList();

    if(loading)
    {
        DrawLoadingScreen();
        return;
    }

    if(inmenu)
    {
		mainmenu.Draw();
        return;
    }
		
	//don't pass any input processing around when the gui is active
    if( External::gui->GetFocus() )
        return;

    _player.Input(fps);
    _player.Update(fps);
}


void MyGame::Save()
{
	//nothing
}

void MyGame::Load()
{
	//nothing
}


void MyGame::SetLoading(bool isLoading)
{
	_renderer.EnableGameRendering( false );

	External::gui->SetGUIHidden( true );
    loading = isLoading;
    if( !loading )
    {
		loadingCounter = 0;
		loadingDirection = 0;
		loadingPoint = vec2(100,100);
		_renderer.EnableGameRendering( true );
		External::gui->SetGUIHidden( false );
    }
}

void MyGame::SetMainMenu(bool inMainMenu)
{
	inmenu = inMainMenu;
	_renderer.EnableGameRendering( inmenu );
}

void MyGame::UnloadCurrentArea()
{
	Global::SetDebugMode(0);
    _player.SetControlObject(0);
	_current_area.Unload();

    _player.SetControlObject(0);
    _physics.Cleanup();
}

Area& MyGame::GetCurrentArea()
{
    return _current_area;
}

void MyGame::InitCurrentArea()
{
	_current_area.Init();
}

void MyGame::AddObject( GameObject* game_object )
{
	_current_area.AddObject( game_object );
}

void MyGame::RemoveObject( GameObject* game_object )
{
	GameObject* player = 0;
	if( _player.GetControlObject() )
		player = _player.GetControlObject();

	if( player && player == game_object )
		_player.SetControlObject( 0 );

	_current_area.RemoveObject( game_object );
}


/////////////////////////////////////////////////////////////////////////////
// This function draws a coordinate system cross in the center of the world
// If you want it bigger/smaller just change the 'num' value.
void MyGame::DrawWorldCenteredDebugCross()
{
        const unsigned num = 20;
        const float offset = num;

        glBegin(GL_LINES);

        glColor3f(0,0,1);
        glVertex3f( -1, 0, 0 );
        glVertex3f( 1, 0, 0 );

        for(unsigned i = 0; i < num; ++i)
        {
                glVertex3f( (i*2.0f) - offset, 0, 0 );
                glVertex3f( (i*2.0f+1.0f) - offset, 0, 0 );
        }

        glColor3f(0,1,0);
        glVertex3f( 0, -1, 0 );
        glVertex3f( 0, 1, 0 );

        for(unsigned i = 0; i < num; ++i)
        {
                glVertex3f( 0, (i*2.0f) - offset, 0 );
                glVertex3f( 0, (i*2.0f+1.0f) - offset, 0 );
        }

        glColor3f(1,0,0);
        glVertex3f( 0, 0, -1 );
        glVertex3f( 0, 0, 1 );

        for(unsigned i = 0; i < num; ++i)
        {
                glVertex3f( 0, 0, (i*2.0f) - offset );
                glVertex3f( 0, 0, (i*2.0f+1.0f) - offset );
        }


        glEnd();
}

void MyGame::DrawLoadingScreen()
{
        loadingCounter++;
        if( loadingCounter > 100 )
        {
                loadingDirection++;
                loadingDirection = loadingDirection%4;
                loadingCounter = 0;
        }

        const float distance = 1.0f;
        if( loadingDirection == 0 )
                loadingPoint.x += distance;
        else if( loadingDirection == 1 )
                loadingPoint.y += distance;
        else if( loadingDirection == 2 )
                loadingPoint.x -= distance;
        else if( loadingDirection == 3 )
                loadingPoint.y -= distance;

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        int w = Global::GameWindow->GetW();
        int h = Global::GameWindow->GetH();
        gluOrtho2D(0, w, h, 0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor3f(0.5,0.5,0.5);
        glPointSize(3);
        glBegin(GL_POINTS);
        glVertex2f( loadingPoint.x, loadingPoint.y );
        glEnd();
        glPointSize(1);

        DebugDraw loader;
        if( loadingDirection == 0 )
                loader.DrawString(50, 50, "Loading.");
        else if( loadingDirection == 1 )
                loader.DrawString(50, 50, "Loading..");
        else if( loadingDirection == 2 )
                loader.DrawString(50, 50, "Loading...");
        else if( loadingDirection == 3 )
                loader.DrawString(50, 50, "Loading....");


        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
}

