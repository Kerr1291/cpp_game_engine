#ifndef ROCK_H
#define ROCK_H

#include <BaseObject.h>

class Rock : public BaseObject
{
        public:
                Rock();
                ~Rock();
                virtual std::string GetTypeName();
                virtual std::string GetTextName();
                virtual void Update(float dt);
                virtual void DoCollision(BaseObject* objectCollidedWith);
       
        private:

};



#endif

