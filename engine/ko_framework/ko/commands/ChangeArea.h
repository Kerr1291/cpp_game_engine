

#ifndef CHANGEAREA_H
#define CHANGEAREA_H

#include <Command.h>

class Area;

class ChangeArea : public Command
{
        public:
                ChangeArea(Area* newArea);
                virtual ~ChangeArea();
                virtual truth Execute();

        private:
                Area* area;
};

#endif
