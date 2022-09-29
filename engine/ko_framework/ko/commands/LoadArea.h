#ifndef LOADAREA_H
#define LOADAREA_H

#include <Command.h>
#include <Area.h>

template <typename T>
class LoadArea : public Command
{
	public:
		LoadArea();
		LoadArea(const std::string& name);
		virtual ~LoadArea();
		virtual truth Execute();

	private:
};

template <typename T>
LoadArea<T>::LoadArea()
:Command(CommandType::BLOCKING)
{}

template <typename T>
LoadArea<T>::~LoadArea()
{}

template <typename T>
truth LoadArea<T>::Execute()
{
	Global::SetDebugMode(0);
	External::gi->UnloadCurrentArea();
	External::gi->SetArea(new T());
	External::gi->InitCurrentArea();
	External::gi->SetMainMenu( 0 );

	complete = 1;
	return 1;
}


#endif
