#ifndef COMMAND_H
#define COMMAND_H

#include <globals.h>

namespace CommandType
{
        const truth NON_BLOCKING = 0;
        const truth BLOCKING = 1;
        const truth NON_MANAGED = 0;
        const truth MANAGED = 1;
}

class Command
{
        public:
                Command(truth isblocking);
                virtual ~Command();
                virtual truth Execute() = 0;
                truth IsComplete();
                void Reset();
                //void AddCommandToChain(Command* cmd);

                const truth blocking;
                truth managed; //managed commands are auto-deleted by the game
        protected:
                truth complete;
                //std::list<Command*> chain;
};

#endif

