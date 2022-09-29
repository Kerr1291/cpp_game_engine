#ifndef SETLOADINGSCREEN_H
#define SETLOADINGSCREEN_H

#include <Command.h>

class MyGame;
class SetLoadingScreen : public Command
{
        public:
                SetLoadingScreen(MyGame* game, bool show);
                virtual ~SetLoadingScreen();
                virtual truth Execute();

        private:
			MyGame* _game;
            bool	_state;
};

#endif
