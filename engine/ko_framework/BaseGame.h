#ifndef BASEGAME_H
#define BASEGAME_H

#include <globals.h>
#include <GameSystemInterface.h>
#include <Area.h>

class Camera;
class Command;
class GridInfo;
class GameObject;

class BaseGame
{
	friend class ChangeArea;
	friend class BaseGameCreator;
	public:
		BaseGame()
			:_system_interface(0)
			,_current_camera(0){External::game=this;}
		virtual ~BaseGame(){}

		virtual void Startup() = 0;
		virtual void Main(double fps) = 0;
		virtual void Save(){}
		virtual void Load(){}

		Camera* GetCurrentCamera() { return _current_camera; }
		GameSystemInterface& GetSystemInterface() { return *_system_interface; }

        virtual void UnloadCurrentArea() = 0;
        virtual Area& GetCurrentArea() = 0;
		virtual void InitCurrentArea() = 0;
		
        virtual void AddObject( GameObject* game_object ) = 0;
        virtual void RemoveObject( GameObject* game_object ) = 0;

	protected:
		Camera* _current_camera;

	private:
		GameSystemInterface* _system_interface;
};

class BaseGameCreator
{
public:
	BaseGameCreator(){}
	void SetGameSystemInterface(GameSystemInterface* system_interface, BaseGame* game) { game->_system_interface = system_interface; }
	virtual BaseGame* Create(GameSystemInterface* system_interface) = 0;
};


template <typename T>
class BaseGameCreatorType : public BaseGameCreator
{
	public:
		BaseGameCreatorType(){}
		BaseGame* Create(GameSystemInterface* system_interface)
		{ 
			T* t = new T();
			SetGameSystemInterface(system_interface, t);
			return t;
		}
};


extern BaseGameCreator* __make_game__;

#define MAKE_GAME( name ) \
	BaseGameCreator* __make_game__; \
	struct __##name##__GAME_TYPE_CREATOR__   { \
	__##name##__GAME_TYPE_CREATOR__() { \
    __make_game__ = new BaseGameCreatorType< name >(); } \
	~__##name##__GAME_TYPE_CREATOR__() { delete __make_game__; } \
	}; static __##name##__GAME_TYPE_CREATOR__ __CREATE__##name##__GAME__TYPE__;


#endif