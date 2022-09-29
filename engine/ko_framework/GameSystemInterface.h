#ifndef GAME_SYSTEM_INTERFACE_H
#define GAME_SYSTEM_INTERFACE_H

class Command;
class SystemInterface;
class BaseGame;

class RenderManager;
class PhysicsManager;
class AudioManager;

#include <AudioManagerInterface.h>
#include <RenderManagerInterface.h>
#include <PhysicsManagerInterface.h>

class GameSystemInterface
{
        friend class Area;
		friend class Editor;
		friend class MainWindowInterface;
		friend class GameObject;

		friend class TextureAtlas;
		friend class PhysicsBody;
		friend class Renderable;

				PhysicsManager& GetPhysicsManager();
				RenderManager& GetRenderManager();
				AudioManager& GetAudioManager();
				void GenerateScreenTextures();

        public:
                GameSystemInterface(SystemInterface& system);
                ~GameSystemInterface();

				AudioManagerInterface& GetAudioManagerInterface();
				RenderManagerInterface& GetRenderManagerInterface();
				PhysicsManagerInterface& GetPhysicsManagerInterface();

				void AddCommand( Command* command );
				bool HasPendingCommand( Command* command );
				
				BaseGame& GetGame();

        private:

				SystemInterface& _system_interface;
			
				AudioManagerInterface _audio_manager_interface;
				RenderManagerInterface _render_manager_interface;
				PhysicsManagerInterface _physics_manager_interface;
};

#endif