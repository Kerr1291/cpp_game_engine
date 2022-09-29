#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <globals.h>

#include <Transform.h>
#include <ObjectProperties.h>

#include <tweakbar.h>

struct RenderObject;
struct PhysicsObject;

class BaseObject
{
        friend class ObjectManager;
        friend class PlayerInterface;
		friend class Editor;
        public:

				BaseObject():gfx(0),phys(0),unique_id(Global::GetUniqueIdNumber()){}
                virtual ~BaseObject(){}

				virtual void Init(){}
				//virtual void LoadFromFile(){}
				//virtual void SaveToFile(){}

                virtual std::string GetTypeName() = 0;
                virtual std::string GetTextName() = 0;
                virtual void Update(float dt) = 0;
                virtual void DoCollision(BaseObject* objectCollidedWith){}//by default, no action on a collision
               
                void SetColor(float r, float g, float b, float a = 1.0f);
                void SetPosition(vec2 pos);
                void SyncTransformToPhysics();
                void SetSleepState(truth canSleep);

				RenderObject* GetRenderObject() { return gfx; }
				PhysicsObject* GetPhysicsObject() { return phys; }
				const unsigned& ID() const { return unique_id; }
				const std::string StringID() const;

                Transform t;
                ObjectProperties prop;
                vec2 direction;
                vec2 velocity;

				void ExposeToTweakbar( TwBar* bar );
				void RemoveFromTweakbar( TwBar* bar );

        protected:

                RenderObject* gfx;
                PhysicsObject* phys;

		private:
			
				const unsigned unique_id;
};


#endif

