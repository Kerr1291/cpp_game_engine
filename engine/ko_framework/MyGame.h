#ifndef MYGAME_H
#define MYGAME_H

#include <globals.h>
#include <BaseGame.h>

#include <Parser.h>

#include <MyPlayerLogic.h>
#include <MainMenu.h>
class GameObject;
class PlayerLogic;

class RenderManagerInterface;
class PhysicsManagerInterface;
class AudioManagerInterface;

class MyGame : public BaseGame
{
	//Commands that are specific to this game
	friend class EnterMenu;
	friend class ExitMenu;
	friend class SetLoadingScreen;

	public:
		MyGame();
		virtual ~MyGame();

		virtual void Startup();
		virtual void Main(double fps);
		virtual void Save();
		virtual void Load();

        virtual void AddObject( GameObject* game_object );
        virtual void RemoveObject( GameObject* game_object );
		
        virtual void UnloadCurrentArea();
        virtual Area& GetCurrentArea();
        virtual void InitCurrentArea();

	private:

        void SetLoading(bool isLoading);
        void SetMainMenu(bool inMainMenu);
        void DrawWorldCenteredDebugCross();
        void DrawLoadingScreen();

        INIFile config;

	private:

		Area _current_area;
		PlayerLogic _player;

        bool inmenu;
        bool loading;
		
        unsigned loadingCounter;
        unsigned loadingDirection;
        vec2 loadingPoint;
        MainMenu mainmenu;
		
		RenderManagerInterface& _renderer;
		PhysicsManagerInterface& _physics;
		AudioManagerInterface& _audio;
};


#endif