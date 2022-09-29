#ifndef DONOTHING_H
#define DONOTHING_H

#include <Command.h>

class DoNothing : public Command
{
        public:
                DoNothing():Command(CommandType::BLOCKING){}
                virtual ~DoNothing(){}
                virtual truth Execute(){complete=1;return 1;}

        private:
};

#endif
