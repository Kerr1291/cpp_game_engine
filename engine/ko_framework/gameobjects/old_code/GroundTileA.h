

#ifndef GROUNDTILEA_H
#define GROUNDTILEA_H

#include <BaseObject.h>

class GroundTileA : public BaseObject
{
        public:
                GroundTileA();
                ~GroundTileA();
                virtual std::string GetTypeName();
                virtual std::string GetTextName();
                virtual void Update(float dt);
                virtual void DoCollision(BaseObject* objectCollidedWith);
       
        private:

};



#endif

