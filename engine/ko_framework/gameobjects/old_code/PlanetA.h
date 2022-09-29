#ifndef PLANETA_H
#define PLANETA_H

#include <BaseObject.h>

class PlanetA : public BaseObject
{
        public:
                PlanetA();
                ~PlanetA();
                virtual std::string GetTypeName();
                virtual std::string GetTextName();
                virtual void Update(float dt);
                virtual void DoCollision(BaseObject* objectCollidedWith);
       
        private:
                truth syncComplete;

};



#endif

