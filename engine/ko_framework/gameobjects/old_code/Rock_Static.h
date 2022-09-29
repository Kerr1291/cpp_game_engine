#ifndef ROCK_STATIC_H
#define ROCK_STATIC_H

#include <BaseObject.h>

class StaticRock : public BaseObject
{
        public:
                StaticRock();
                ~StaticRock();
                virtual std::string GetTypeName();
                virtual std::string GetTextName();
                virtual void Update(float dt);
                virtual void DoCollision(BaseObject* objectCollidedWith);
       
        private:
                truth syncComplete;

};



#endif

