#ifndef CUSTOMOBJECT_H
#define CUSTOMOBJECT_H

#include <BaseObject.h>

class CustomObject : public BaseObject
{
		friend class Editor;
        public:
                CustomObject();
                ~CustomObject();
                virtual std::string GetTypeName();
                virtual std::string GetTextName();
                virtual void Update(float dt);
                virtual void DoCollision(BaseObject* objectCollidedWith);
       
        private:
			std::string object_type;
			std::string object_name;
};



#endif

