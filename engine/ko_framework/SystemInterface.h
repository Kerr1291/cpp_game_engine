#ifndef SYSTEMINTERFACE_H
#define SYSTEMINTERFACE_H

#include <globals.h>
#include <CommandQueue.h>

#include <Editor.h>
#include <GUIManager.h>
#include <RenderManager.h>
#include <Physics.h>
//#include <LogicManager.h>
#include <audio.h>

#include <GameSystemInterface.h>

class BaseGame;

class SystemInterface
{
        friend class Area;
		friend class Editor;
		friend class GameSystemInterface;
        public:
                SystemInterface();
                ~SystemInterface();
				
				BaseGame& GetGame();

                void Initialize();
                void Update(double fps);
				void ProcessCommands();
                void Draw();
                void Cleanup();

				void NotifyActiveCameraWindowResize();
        private:

				BaseGame*		 _game;

                CommandQueue _engine_commands;
                double _system_fps;

				struct Systems
				{
					PhysicsManager  _physics;
					RenderManager	_renderer;
					//LogicManager  _logic;
					AudioManager	_audio;					
					Editor			_editor;
					GUIManager		_gui;
				};

				Systems* _systems;
				GameSystemInterface _game_system_interface;
};


#endif
