#include <EnterMenu.h>

#include <MyGame.h>

EnterMenu::EnterMenu(MyGame* game)
:Command(CommandType::BLOCKING)
{
	_game = game;
}

EnterMenu::~EnterMenu()
{}

truth EnterMenu::Execute()
{
        _game->SetMainMenu(1);
        complete = 1;

        return 1;
}
