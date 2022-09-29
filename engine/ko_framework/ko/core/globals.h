#ifndef GLOBALS_H
#define GLOBALS_H

#define DEBUG_BUILD 1

#define WINDOWS_BUILD //change this to #define WINDOWS/LINUX/MAC_BUILD to compile different builds
#define CODEBLOCKS 0

#if CODEBLOCKS
#define GLEW_STATIC
#else
#define __restrict__ __restrict
#endif

//pre-opengGL platform specific stuff
#ifdef _WIN32
#endif
#ifdef LINUX_BUILD
#endif
#ifdef MAC_BUILD
#endif

#include <glew.h>
#include <freeglut.h>

//more general platform specific stuff
#ifdef _WIN32
	#if CODEBLOCKS == 0
	#include <wglew.h>
	#endif
#endif
#ifdef LINUX_BUILD
    #include <glxew.h>
#endif
#ifdef MAC_BUILD
#endif

//3rd party stuff

//math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


//physics
#include <Box2D/Box2D.h>

//TweakBar
#include <AntTweakBar.h>


//Standard libarary stuff
#include <iostream>
#include <math.h>

#include <string>
#include <sstream>

#include <vector>
#include <list>
#include <map>
#include <set>


namespace Path
{
	const std::string GraphicAssets = "./Graphics/";
	const std::string ShaderAssets = "./ko/Shaders/";
}


typedef b2Vec2 b2vec2;
typedef b2Mat22 b2mat2;

typedef glm::uvec2 uvec2;
typedef glm::ivec2 ivec2;

typedef glm::vec2 vec2;
typedef glm::mat2 mat2;
typedef glm::vec3 vec3;
typedef glm::mat3 mat3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;
typedef unsigned char truth;

//Custom Tweakbar types
extern TwType TW_TYPE_VEC2;
extern TwType TW_TYPE_B2VEC2;


class Editor;
class GUIManager;
class BaseGame;
class GameSystemInterface;

namespace External
{
    //Variables that allow access to other systems in the game
	//extern GLFWwindow* mainWindow;
	extern Editor*				editor;
    extern GUIManager*			gui;
	extern BaseGame*			game;
	extern GameSystemInterface* system;
}

namespace Button
{
        typedef unsigned char buttontype;
        const buttontype LEFT_DOWN = 1;
        const buttontype RIGHT_DOWN = 2;
        const buttontype MIDDLE_DOWN = 4;
        const buttontype LEFT_HELD = 8;
        const buttontype RIGHT_HELD = 16;
        const buttontype LEFT_UP = 32;
        const buttontype RIGHT_UP = 64;
        const buttontype MIDDLE_UP = 128;
}

//Conversion types
namespace Convert
{
    ///////////////////////////////////////
    //Use case examples
    //
    // Turn the glm vector2 into a box2d type
    // vec2 a = vec2( 1, 2 );
    // b2vec2 b = Convert::v2(a).to_box2d;
    //
    // Access the box2d vec2 like a glm type
    // b2vec2 c = b2vec2( 1, 2 );
    // Convert::v2(c).to_glm = vec2(3,4);
    //


    //Don't use these directly. See the use case examples
    union _Vec2 {
        _Vec2( vec2* _v ):glm(_v){}
        _Vec2( b2vec2* _v ):box2d(_v){}
        vec2* glm;
        b2vec2* box2d;
    };
    union _CVec2 {
        _CVec2( const vec2* _v ):glm(_v){}
        _CVec2( const b2vec2* _v ):box2d(_v){}
        const vec2* glm;
        const b2vec2* box2d;
    };

    union _Mat2 {
        _Mat2( mat2* _m ):glm(_m){}
        _Mat2( b2mat2* _m ):box2d(_m){}
        mat2* glm;
        b2mat2* box2d;
    };

    //Convert between vec2 and b2vec2
    struct v2 {

    private:
        _Vec2 data;

    public:

        vec2& to_glm;
        b2vec2& to_box2d;

        v2( vec2& _v )
        :data( &_v )
        ,to_glm( *data.glm )
        ,to_box2d( *data.box2d ){}

        v2( b2vec2& _v )
        :data( &_v )
        ,to_glm( *data.glm )
        ,to_box2d( *data.box2d ){}
    };

    //Convert between const vec2 and const b2vec2
    struct c_v2 {

    private:
        _CVec2 data;

    public:

        const vec2& to_glm;
        const b2vec2& to_box2d;

        c_v2( const vec2& _v )
        :data( &_v )
        ,to_glm( *data.glm )
        ,to_box2d( *data.box2d ){}

        c_v2( const b2vec2& _v )
        :data( &_v )
        ,to_glm( *data.glm )
        ,to_box2d( *data.box2d ){}
    };

    //Convert between mat2 and b2mat22
    struct m2 {

    private:
        _Mat2 data;

    public:

        mat2& to_glm;
        b2mat2& to_box2d;

        m2( mat2& _m )
        :data( &_m )
        ,to_glm( *data.glm )
        ,to_box2d( *data.box2d ){}

        m2( b2mat2& _m )
        :data( &_m )
        ,to_glm( *data.glm )
        ,to_box2d( *data.box2d ){}
    };

}


namespace Debug
{

/////////////////////////TWEAKBAR STUFF//////////////////////
#if DEBUG_BUILD

	extern TwBar* globalBar;
	void InitTweakbarTypes();
	void InitGlobalTweakBar();

#endif//DEBUG_BUILD


    namespace Internal
    {
            void PrintStrings();
    }

    //Warning: These are *slow*
    void printf(int x, int y, const char* string, ...);
    void printf3D(float x, float y, float z, const char* string, ...);
}

class MainWindowInterface;
namespace Global
{
		void SetVSync(bool enabled);
		bool GetVSyncState();

        class Window
        {
                friend class ::MainWindowInterface;
                Window():windowW(800),windowH(600){}
                Window(unsigned w, unsigned h)
                        :windowW(w),windowH(h){}
                void Set(unsigned w, unsigned h)
                        { windowW = w; windowH = h; }
        public:
                unsigned GetW() { return windowW; }
                unsigned GetH() { return windowH; }
                const unsigned& GetWRef() { return windowW; }
                const unsigned& GetHRef() { return windowH; }
                vec2 GetDimensions() { return vec2(windowW, windowH); }
        private:
                unsigned windowW;
                unsigned windowH;
        };
        extern Window* GameWindow; //defined in main


        //////////////////////////////////////////////////
        //global consts
        const float PI = 3.141592f;
        const float TINY = 0.001f;
        const float ZERO = 0.000001f;
#ifdef _WIN32
		const std::string FILESLASH = "\\";
#else
		const std::string FILESLASH = "/";
#endif

		//special key consts
        namespace KEY
        {
                const unsigned char ESC = 27;
                const unsigned char DEL = 127;
        }

        //////////////////////////////////////////////////
        ///global variables
        namespace Mouse
        {
                extern vec2 ScreenPos;
                extern vec2 WorldPos;
                extern vec2 FrameDelta;
                extern int WheelDelta;
                extern Button::buttontype ButtonStates;
				bool GetState(Button::buttontype button);
        }

        namespace Keyboard
        {
                bool KeyPressed(unsigned char key);
                bool KeyReleased(unsigned char key);
                bool KeyHeld(unsigned char key);
                extern bool KeyState[256];
                extern bool KeyStateChange[256];

                bool AnyKeyWasPressed();
                unsigned char GetLastPressedKey();
        }

        //////////////////////////////////////////////////
        //global helper functions
        void SetDebugMode(truth state);
        truth GetDebugMode();

        //Get a 0-1 float (This is a pretty mediocre way to do it, but it works fine for now). TODO: update this to be better
        inline float GetRandomFloat() { return (float)(rand()%10001)/10000.0f; }

		//get a number that is garunteed to be unique during the running of the program
		unsigned long long GetUniqueIdNumber();

		//get the extension from a filename
		std::string GetExtension(const std::string& filename);
}




#endif
