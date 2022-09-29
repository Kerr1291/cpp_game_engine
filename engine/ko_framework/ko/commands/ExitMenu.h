#ifndef EXITMENU_H
#define EXITMENU_H

#include <Command.h>

class MyGame;
class ExitMenu : public Command
{
        public:
                ExitMenu( MyGame* game );
                virtual ~ExitMenu();
                virtual truth Execute();

        private:
			MyGame* _game;
};

#endif

