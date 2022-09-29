#include <globals.h>

#include <GameSystemInterface.h>
#include <BaseGame.h>
#include <Editor.h>
#include <GUIManager.h>

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <stddef.h>

//Create the tweakbar types
TwType TW_TYPE_VEC2;
TwType TW_TYPE_B2VEC2;

namespace External
{
    //Variables that allow access to objects created other places in the game
	Editor*					editor      = 0;
    GUIManager*				gui         = 0;
	BaseGame*				game		= 0;
	GameSystemInterface*	system		= 0;
}


namespace Debug
{
    struct DebugString
    {
            vec3 pos;
            std::string data;
    };
    std::vector<DebugString> debugStrings;
    std::vector<DebugString> debugStrings3D;

    namespace Internal
    {
            void PrintStrings()
            {
                    glColor3f(0.9f, 0.6f, 0.6f);
                    if( !debugStrings.empty() )
                    {
                            glMatrixMode(GL_PROJECTION);
                            glPushMatrix();
                            glLoadIdentity();
                            int w = Global::GameWindow->GetW();
                            int h = Global::GameWindow->GetH();
                            gluOrtho2D(0, w, h, 0);
                            glMatrixMode(GL_MODELVIEW);
                            glPushMatrix();
                            glLoadIdentity();

                            for(unsigned i = 0; i < debugStrings.size(); ++i)
                            {
                                    glRasterPos2i((GLint)debugStrings[i].pos.x, (GLint)debugStrings[i].pos.y);
                                    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)debugStrings[i].data.c_str());

                                    //std::string::iterator iter = debugStrings[i].data.begin();
                                    //for(; iter != debugStrings[i].data.end(); ++iter)
                                    //{
                                    //        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *iter);
                                    //}

                            }
                            debugStrings.clear();

                            glPopMatrix();
                            glMatrixMode(GL_PROJECTION);
                            glPopMatrix();
                    }

                    if( !debugStrings3D.empty() )
                    {
                            for(unsigned i = 0; i < debugStrings3D.size(); ++i)
                            {
                                    glRasterPos3f(debugStrings3D[i].pos.x, debugStrings3D[i].pos.y, debugStrings3D[i].pos.z);
                                    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)debugStrings3D[i].data.c_str());

                                    //std::string::iterator iter = debugStrings3D[i].data.begin();
                                    //for(; iter != debugStrings3D[i].data.end(); ++iter)
                                    //{
                                    //        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *iter);
                                    //}

                            }
                            debugStrings3D.clear();
                    }
            }
    }

    void printf(int x, int y, const char* string, ...)
    {
            if(!Global::GetDebugMode())
                    return;

            char buffer[256];

            va_list arg;
            va_start(arg, string);
#if CODEBLOCKS == 0
        vsprintf_s(buffer, string, arg); //vsprintf_s
#else
        sprintf(buffer, string, arg); //vsprintf_s
#endif
            va_end(arg);

            std::string out_buffer( buffer );
            DebugString dstr = { vec3(x,y,0), out_buffer };
            debugStrings.push_back( dstr );
    }

    void printf3D(float x, float y, float z, const char* string, ...)
    {
            if(!Global::GetDebugMode())
                    return;

            char buffer[256];

            va_list arg;
            va_start(arg, string);
#if CODEBLOCKS == 0
            vsprintf_s(buffer, string, arg); //vsprintf_s
#else
            sprintf(buffer, string, arg); //vsprintf_s
#endif
            va_end(arg);

            std::string out_buffer( buffer );
            DebugString dstr = { vec3(x,y,z), out_buffer };
            debugStrings3D.push_back( dstr );
    }
}


namespace Global
{
	bool VSyncState = false;
	void SetVSync(bool enabled)
	{
		unsigned int newstate = 1;
		VSyncState = true;
		if(!enabled)
		{
			VSyncState = false;
			newstate = 0;
		}

		#ifdef _WIN32
            #if CODEBLOCKS == 0
			wglSwapIntervalEXT(newstate);
            #endif
        #endif
		#ifdef LINUX_BUILD
			glXSwapIntervalSGI(newstate);
        #endif
		#ifdef MAC_BUILD
			AGLSetInteger(newstate);
        #endif
	}

	bool GetVSyncState()
	{
		return VSyncState;
	}

    ///global variables
    namespace Mouse
    {
        vec2 ScreenPos(0,0);
        vec2 WorldPos(0,0);
        vec2 FrameDelta(0,0);
        int WheelDelta = 0;
        Button::buttontype ButtonStates = 0;

		//0 = left, 1 = right, 2 = middle
		bool GetState(Button::buttontype button)
		{
			if( button & ButtonStates )
				return true;
			return false;
		}
    }

    namespace Keyboard
    {
            bool KeyPressed(unsigned char key)
            {
                    return KeyStateChange[key] & KeyState[key];
            }

            bool KeyReleased(unsigned char key)
            {
                    return KeyStateChange[key] & !KeyState[key];
            }

            bool KeyHeld(unsigned char key)
            {
                    return KeyState[key];
            }

            bool KeyState[256];
            bool KeyStateChange[256];
    }

    static truth DebugState = 1;

    void SetDebugMode(truth state)
    {
        DebugState = state;
    }

    truth GetDebugMode()
    {
        return DebugState;
    }

	unsigned long long GetUniqueIdNumber()
	{
		static unsigned long long unique_id_counter = 0;
		return unique_id_counter++;
	}
	
	std::string GetExtension(const std::string& filename)
	{
		std::string ext;
		std::string ext_r;
		std::string::const_reverse_iterator iter = filename.rbegin();
		ext_r.push_back( *iter++ );
		ext_r.push_back( *iter++ );
		ext_r.push_back( *iter );
		ext.push_back( ext_r.at(2) );
		ext.push_back( ext_r.at(1) );
		ext.push_back( ext_r.at(0) );
		return ext;
	}
}

/////////////////////////TWEAKBAR STUFF//////////////////////
#if DEBUG_BUILD

namespace Debug
{
	void TW_CALL ToggleDebugMode(void* null)
	{
		Global::SetDebugMode(( Global::GetDebugMode() ? 0 : 1 ));
	}
	//void TW_CALL ToggleHUD(void* null)
	//{
	//	External::gi->ToggleHUD();
	//}

	/*
	void TW_CALL SetCameraPos(const void* data, void* null)
	{
		float x = (*(const vec2*)data).x;
		float y = (*(const vec2*)data).y;
		External::game->GetCurrentCamera()->SetWindowPos(x,y);
	}
	void TW_CALL GetCameraPos(void* data, void* null)
	{
		//*(vec2*)data = External::game->GetCurrentCamera()->GetPos();
		*(vec2*)data = vec2(0,0);
	}


	void TW_CALL SetCameraZoom(const void* data, void* null)
	{
		float z = *(const float*)data;
		External::game->GetCurrentCamera()->SetWindowZoom(z);
	}
	void TW_CALL GetCameraZoom(void* data, void* null)
	{
		//*(float*)data = External::game->GetCurrentCamera()->GetWindowZoom();
		*(float*)data = 1.0f; 
	}

	void TW_CALL tbSetVSync(const void* data, void* null)
	{
		bool state = *(const bool*)data;
		Global::SetVSync(state);
	}
	void TW_CALL tbGetVSync(void* data, void* null)
	{
		*(bool*)data = Global::GetVSyncState();
	}
	*/



	TwBar* globalBar = 0;

	void InitTweakbarTypes()
	{
		//vec2
		TwStructMember vec2_x = { "x", TW_TYPE_FLOAT, offsetof(vec2,x), 0 };
		TwStructMember vec2_y = { "y", TW_TYPE_FLOAT, offsetof(vec2,y), 0 };
		TwStructMember VEC2[2] = { vec2_x, vec2_y };
		TW_TYPE_VEC2 = TwDefineStruct("TW_TYPE_VEC2", VEC2, 2, sizeof( vec2 ), 0, 0);

		//b2vec2
		TwStructMember b2vec2_x = { "x", TW_TYPE_FLOAT, offsetof(b2vec2,x), 0 };
		TwStructMember b2vec2_y = { "y", TW_TYPE_FLOAT, offsetof(b2vec2,y), 0 };
		TwStructMember B2VEC2[2] = { b2vec2_x, b2vec2_y };
		TW_TYPE_B2VEC2 = TwDefineStruct("TW_TYPE_B2VEC2", B2VEC2, 2, sizeof( b2vec2 ), 0, 0);
	}

	void InitGlobalTweakBar()
	{
		//globalBar = TwNewBar("Globals");
		//TwAddVarRO(globalBar, "Mouse-ScreenPos", TW_TYPE_VEC2, &Global::Mouse::ScreenPos, " group='Mouse' label='Screen Position'");
		//TwAddVarRO(globalBar, "Mouse-WorldPos", TW_TYPE_VEC2, &Global::Mouse::WorldPos, " group='Mouse' label='World Position'");

		//TwAddButton(globalBar, "ToggleDebugMode", ToggleDebugMode, 0, " label='Toggle Debug Mode' ");
		//TwAddButton(globalBar, "ToggleHDU", ToggleHUD, 0, " label='Toggle HUD' ");

		//TwAddVarCB(globalBar, "Camera-Pos", TW_TYPE_VEC2, SetCameraPos, GetCameraPos, 0, " group='Camera' label='Position'");
		//TwAddVarCB(globalBar, "Camera-Zoom", TW_TYPE_FLOAT, SetCameraZoom, GetCameraZoom, 0, " group='Camera' label='Zoom'");

		//TwAddVarCB(globalBar, "VSYNC", TW_TYPE_BOOL32, tbSetVSync, tbGetVSync, 0, " group='Graphics' label='Toggle VSync'");
	}

}
#endif//DEBUG_BUILD
