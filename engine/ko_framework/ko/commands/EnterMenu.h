#ifndef ENTERMENU_H
#define ENTERMENU_H

#include <Command.h>

class MyGame;
class EnterMenu : public Command
{
        public:
                EnterMenu( MyGame* game );
                virtual ~EnterMenu();
                virtual truth Execute();

        private:
			MyGame* _game;
};

#endif
