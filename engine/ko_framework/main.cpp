#include <globals.h>

#include <SystemInterface.h>

#include <SFML/System.hpp>

SystemInterface* game = 0;

unsigned RANDOM_SEED = 2400050;
float systemfps = 0.016f;
std::string appFPS = "0";
std::string title = "OpenGE";

///////////////////////////////////////////////////////////////////////
//used to change the game's window size
class MainWindowInterface
{
public:
        MainWindowInterface()
        :gameWindow(800,600)
        {}
        void Init(unsigned w, unsigned h)
        { gameWindow.Set(w,h); }
        void Set(unsigned w, unsigned h)
        {
		  gameWindow.Set(w,h);
		  glutReshapeWindow(w,h);
		  glViewport(0,0,w,h);
		  External::system->GenerateScreenTextures(); //we always have to recreate this when we change the screen size
		}
        Global::Window gameWindow;
};
MainWindowInterface windowInterface;
Global::Window* Global::GameWindow = &windowInterface.gameWindow; //declared in GameGlobals.h
//////////////////////////////////////////////////////////////////////

namespace Global
{
    namespace Keyboard
    {
        bool AnyKeyPressed = false;
        bool AnyKeyWasPressed()
        {
            return AnyKeyPressed;
        }
        unsigned char LastPressedKey = 0;
        unsigned char GetLastPressedKey()
        {
            return LastPressedKey;
        }
    }
}

using namespace Global::Keyboard;

void Update();
void Draw();
void KeyboardDown(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);
void MouseFunc(int button, int state, int x, int y);
void MouseWheelFunc(int wheel, int direction, int x, int y);
void MouseMotionFunc(int x, int y);
void ResizeWindow(int w, int h);
void Shutdown();
double calculateFPS();

#include <DataFileIterator.h>
int main(int argc, char* argv[])
{
    unsigned w = 800;
    unsigned h = 600;
    srand( RANDOM_SEED );
    memset( KeyState, 0, 256 );
    memset( KeyStateChange, 0, 256 );

    windowInterface.Init( w, h );

	glutInit ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE );
	glutInitWindowSize ( Global::GameWindow->GetW(), Global::GameWindow->GetH() );
	glutCreateWindow ( title.c_str() );
	glutIgnoreKeyRepeat(1);

    glewInit();

	if( !GLEW_VERSION_2_0 )
	{
		std::cout << "ERROR! OpenGL 2.0 Not supported.\n";
		std::cout << "VENDOR:    " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "VERSION:   " << glGetString(GL_VERSION) << std::endl;
		std::cout << "RENDERER:  " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "GLEW 2.0:  " << (GLEW_VERSION_2_0!=0) << std::endl;
		Shutdown();
		return 0;
	}


#ifdef _WIN32
	std::cout<<"Windows version running...\n";
#endif
#ifdef LINUX_BUILD
	std::cout<<"Linux version running...\n";
#endif
#ifdef MAC_BUILD
	std::cout<<"MAC version running...\n";
#endif

	//Init tweakbar
	TwInit(TW_OPENGL, NULL);
	TwWindowSize( Global::GameWindow->GetW(), Global::GameWindow->GetH() );
	TwGLUTModifiersFunc(glutGetModifiers);
	Debug::InitTweakbarTypes();
	Debug::InitGlobalTweakBar();

	//create the engine
	game = new SystemInterface();

	//init engine, create the game, init the game
    game->Initialize();

	//bind our callbacks and enter the game loop
	glutDisplayFunc ( Draw );
	glutIdleFunc ( Update );
	glutKeyboardFunc( KeyboardDown );
	glutKeyboardUpFunc( KeyboardUp );
	glutMouseFunc( MouseFunc );
	glutMouseWheelFunc( MouseWheelFunc );
	glutMotionFunc( MouseMotionFunc );
	glutPassiveMotionFunc( MouseMotionFunc );
	glutReshapeFunc( ResizeWindow );
	glutCloseFunc( Shutdown );

	//tweakbar setup for glut
    glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT); //TODO: implement this glut handler for special keys normally, right now we're not handling special keypresses at all

	glutMainLoop();

    return 0;
}

void Shutdown()
{
    TwTerminate();

    delete game;
}

void Update()
{
    game->ProcessCommands();

    //Delete any game resources that need deleting
    game->Cleanup();

    game->Update( systemfps );

    //calculate and show fps
    std::stringstream toStr;
    toStr << 1/calculateFPS();
    appFPS = toStr.str();
	glutSetWindowTitle( (title + " - " + appFPS).c_str() );

    //Update mouse buttons
    Button::buttontype& buttonFlags = Global::Mouse::ButtonStates;

    //Clear Mouse Deltas
	Global::Mouse::WheelDelta = 0;
    Global::Mouse::FrameDelta = vec2(0,0);
    memset( KeyStateChange, 0, 256 );
    Global::Keyboard::AnyKeyPressed = 0;

    //See if left button is held
    if( buttonFlags & Button::LEFT_DOWN )
    {
            buttonFlags &= ~(Button::LEFT_DOWN);
            buttonFlags |= Button::LEFT_HELD;
    }
    else if( buttonFlags & Button::LEFT_UP )
    {
            buttonFlags &= ~(Button::LEFT_UP);
    }

    //See if right button is held
    if( buttonFlags & Button::RIGHT_DOWN )
    {
            buttonFlags &= ~(Button::RIGHT_DOWN);
            buttonFlags |= Button::RIGHT_HELD;
    }
    else if( buttonFlags & Button::RIGHT_UP )
    {
            buttonFlags &= ~(Button::RIGHT_UP);
    }

    //See clear down/up states for middle mouse
    if( buttonFlags & Button::MIDDLE_DOWN )
    {
            buttonFlags &= ~(Button::MIDDLE_DOWN);
    }
    else if( buttonFlags & Button::MIDDLE_UP )
    {
            buttonFlags &= ~(Button::MIDDLE_UP);
    }
}

void Draw()
{
    glClearColor(0.0,0.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    game->Draw();

    //TwDraw();

	glutSwapBuffers();
	glutPostRedisplay();
}

void KeyboardDown(unsigned char key, int x, int y)
{
    TwEventKeyboardGLUT(key,x,y);

    AnyKeyPressed = true;
    LastPressedKey = key;

	KeyStateChange[key] = !KeyState[key];
	KeyState[key] = true;
}

void KeyboardUp(unsigned char key, int x, int y)
{
	KeyStateChange[key] = KeyState[key];
	KeyState[key] = false;
}

void MouseFunc(int button, int state, int x, int y)
{
    TwEventMouseButtonGLUT(button,state,x,y);

	Button::buttontype& buttonFlags = Global::Mouse::ButtonStates;

	if( button == GLUT_LEFT_BUTTON )
	{
		if(state == GLUT_DOWN)
		{
			buttonFlags |= Button::LEFT_DOWN;
		}
		else if(state == GLUT_UP)
		{
			buttonFlags |= Button::LEFT_UP;
			buttonFlags &= ~(Button::LEFT_HELD);
		}
	}

	if( button == GLUT_MIDDLE_BUTTON )
	{
		if(state == GLUT_DOWN)
		{
			buttonFlags |= Button::MIDDLE_DOWN;
		}
		else if(state == GLUT_UP)
		{
			buttonFlags |= Button::MIDDLE_UP;
		}
	}

	if( button == GLUT_RIGHT_BUTTON )
	{
		if(state == GLUT_DOWN)
		{
			buttonFlags |= Button::RIGHT_DOWN;
		}
		else if(state == GLUT_UP)
		{
			buttonFlags |= Button::RIGHT_UP;
			buttonFlags &= ~(Button::RIGHT_HELD);
		}
	}
}

void MouseWheelFunc(int wheel, int direction, int x, int y)
{
	Global::Mouse::WheelDelta = -direction;

	static int TweakbarMouseWheelIndex = 0;
	TweakbarMouseWheelIndex += direction;
	TwMouseWheel( TweakbarMouseWheelIndex );
}

void MouseMotionFunc(int x, int y)
{
    TwEventMouseMotionGLUT(x,y);

	vec2 previous = Global::Mouse::ScreenPos;
	Global::Mouse::ScreenPos = vec2(x,y);
	Global::Mouse::FrameDelta = Global::Mouse::ScreenPos - previous;
	Global::Mouse::FrameDelta.y *= -1; //let's make up be positive
}

void ResizeWindow(int w, int h)
{
    TwWindowSize( w, h );
    windowInterface.Set( w, h );
	game->NotifyActiveCameraWindowResize();
}

//-------------------------------------------------------------------------
// Calculates the frames per second
//-------------------------------------------------------------------------
double calculateFPS()
{
	static sf::Clock fps_timer;
    static double fps = 0;

#if CODEBLOCKS == 0
    //Get the time since last clock update
	double timeInterval = fps_timer.restart().asSeconds();
#else
    double timeInterval = 0.016f;
#endif

	//Set and return it
	fps = timeInterval;
    systemfps = timeInterval; //set the global "fps" variable

    return fps;
}

