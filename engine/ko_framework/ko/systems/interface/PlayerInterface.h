#ifndef PLAYERINTERFACE_H
#define PLAYERINTERFACE_H

#include <globals.h>

#include <map>

class state;

class BaseObject;
class PlayerInterface
{
	friend class state;
        public:
                PlayerInterface();
                ~PlayerInterface();

                void Init();
                void Update(float dt = 0.016f);
                void Input(float fps);
                void SetControlTarget(BaseObject* target);
                BaseObject* const GetControlTarget();

				//controls for commands, used for platformer movement
				void Platformer_Move_Left();
				void Platformer_Move_Right();
				void Platformer_Move_Jump();
				void Platformer_Attack();

                int   PLATFORMER_previous_anim;
				int   PLATFORMER_facing;
				float PLATFORMER_walkSpeed;
				float PLATFORMER_MAX_walkSpeed;
				float PLATFORMER_jumpSpeed;
				truth PLATFORMER_jumping;
				int   PLATFORMER_jumping_counter;

        private:
				float previous_dt;
                BaseObject* controlTarget;
                truth initialized;
				state* player_state;

				//unsigned PLATFORMER_walkAnim; //TODO: put something here after implementing sprite.h's set animation function
				//unsigned PLATFORMER_jumpAnim;
				//unsigned PLATFORMER_attackAnim;

                std::map<unsigned char, void (*)(PlayerInterface&)>* currentControlSet;
                std::map<unsigned char, void (*)(PlayerInterface&)>* currentControlSet_held;
                //void (*controlSet)(PlayerInterface&);
};


#endif
