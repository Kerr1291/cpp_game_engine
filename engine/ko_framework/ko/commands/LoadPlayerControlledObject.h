#ifndef LOADPLAYERCONTROLLEDOBJECT_H
#define LOADLPAYERCONTROLLEDOBJECT_H

#include <Command.h>

class Area;

template <typename T>
class LoadPlayerControlledObject : public Command
{
	public:
		LoadPlayerControlledObject(Area* loadArea, vec2 loadPosition);
		virtual ~LoadPlayerControlledObject();
		virtual truth Execute();

	private:
		vec2 pos;
		Area* area;
};


#include <Area.h>
#include <BaseObject.h>
#include <PlayerInterface.h>

template <typename T>
LoadPlayerControlledObject<T>::LoadPlayerControlledObject(Area* loadArea, vec2 loadPosition)
:Command(CommandType::BLOCKING)
,pos(loadPosition)
,area(loadArea)
{}

template <typename T>
LoadPlayerControlledObject<T>::~LoadPlayerControlledObject() 
{}

template <typename T>
truth LoadPlayerControlledObject<T>::Execute()
{
	if(area)
	{
		BaseObject* obj = CreateObjectInArea( T, area );
		obj->SetPosition( pos );
		External::player->SetControlTarget( obj );
		obj->SetSleepState(0);
		complete = 1;
		return complete;
	}
	return 1;
}

#endif