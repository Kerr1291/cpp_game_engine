#ifndef UNLOADAREA_H
#define UNLOADAREA_H

#include <Command.h>

class UnloadArea : public Command
{
        public:
                UnloadArea();
                virtual ~UnloadArea();
                virtual truth Execute();

        private:
};

#endif

