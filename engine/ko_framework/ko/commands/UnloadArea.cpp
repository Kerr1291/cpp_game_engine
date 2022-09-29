#include <UnloadArea.h>
#include <BaseGame.h>
#include <Camera.h>

UnloadArea::UnloadArea()
:Command(CommandType::BLOCKING)
{
}

UnloadArea::~UnloadArea()
{}

truth UnloadArea::Execute()
{
        if(External::game->GetCurrentArea())
        {
                External::game->UnloadCurrentArea();
        }

        Global::SetDebugMode(0);
        complete = 1;
        return complete;
}

