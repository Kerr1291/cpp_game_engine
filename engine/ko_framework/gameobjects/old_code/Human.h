#ifndef HUMAN_H
#define HUMAN_H

#include <BaseObject.h>

class Human : public BaseObject
{
        public:
                Human();
                ~Human();
                virtual std::string GetTypeName();
                virtual std::string GetTextName();
                virtual void Update(float dt);
                virtual void DoCollision(BaseObject* objectCollidedWith);
       
        private:

};



#endif

