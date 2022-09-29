#include <ExitMenu.h>
#include <MyGame.h>


ExitMenu::ExitMenu(MyGame* game)
:Command(CommandType::BLOCKING)
{
	_game = game;
}

ExitMenu::~ExitMenu()
{}

truth ExitMenu::Execute()
{
        _game->SetMainMenu(0);
        complete = 1;

        return 1;
}

