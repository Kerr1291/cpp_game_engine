#include <ChangeArea.h>
#include <BaseGame.h>
#include <Area.h>
#include <Camera.h>

ChangeArea::ChangeArea(Area* newArea)
:Command(CommandType::BLOCKING)
,area(newArea)
{
}

ChangeArea::~ChangeArea()
{}

truth ChangeArea::Execute()
{
    if(area)
    {
		if(External::game->GetCurrentArea())
		{
			External::game->UnloadCurrentArea();
		}

        External::game->SetArea(area);
        External::game->InitCurrentArea();
        Global::SetDebugMode(0);
        complete = 1;
        return complete;
    }
    return 1;
}

