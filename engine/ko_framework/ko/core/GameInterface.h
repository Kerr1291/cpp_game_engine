#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

#include <globals.h>

#include <Physics.h>
#include <RenderManager.h>
#include <ObjectManager.h>
#include <PlayerInterface.h>
//#include <HUDManager.h>
#include <audio.h>

#include <MainMenu.h>
#include <list>

class INIFile;
class Camera;
class Area;
class BaseObject;
class Command;
class GridInfo;

class GameInterface
{
        friend class Area;
		friend class Editor;
        public:
                GameInterface(INIFile* configFile);
                ~GameInterface();

                void Initialize();
                void SetLoading(truth isLoading);
                void SetMainMenu(truth inMainMenu);
                void UnloadCurrentArea();
                void SetArea(Area* area);
                Area* GetCurrentArea();
                void InitCurrentArea();
                void Input(double fps);
                void Update(double fps);
                void ProcessCommands();
                void Draw();
                void AddCommand( Command* c );
                void AddObject( BaseObject* c );
                void RemoveObject( BaseObject* c );
                void Cleanup();

                //Used to see if a specific command is pending execution in the game's command queue
                truth HasPendingCommand( Command* c );

                Camera* GetCamera() const { return currentCam; }
				vec2 ScreenToWorld(vec2 screen_pos);
				void ToggleHUD();

                INIFile* config;

        private:
                void DrawWorldCenteredDebugCross();
                void DrawLoadingScreen();

                std::list<Command*> commands;

                truth showHUD;
                truth inmenu;
                truth loading;

                unsigned loadingCounter;
                unsigned loadingDirection;
                vec2 loadingPoint;
                double gamefps;
                Camera* currentCam;

                Area* currentArea;
                PhysicsManager physics;
                RenderManager renderer;
                ObjectManager gameObjects;
				AudioManager audio;

                PlayerInterface player;
                HUDManager hudDisplay;

                MainMenu mainmenu;

};


#endif
