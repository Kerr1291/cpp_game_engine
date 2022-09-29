#include <SetLoadingScreen.h>
#include <MyGame.h>

SetLoadingScreen::SetLoadingScreen(MyGame* game, bool show)
:Command(CommandType::BLOCKING)
,_game(game)
,_state(show)
{}

SetLoadingScreen::~SetLoadingScreen()
{}

truth SetLoadingScreen::Execute()
{
        _game->SetLoading(_state);
        complete = 1;

        return 1;
}

