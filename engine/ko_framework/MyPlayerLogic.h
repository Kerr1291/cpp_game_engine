#ifndef MY_PLAYER_LOGIC_H
#define MY_PLAYER_LOGIC_H

class GameObject;

class PlayerLogic
{
	public:
		PlayerLogic();
		~PlayerLogic();

        void Init();
        void Input(double fps);
        void Update(double fps);

        void SetControlObject(GameObject* _object);
        GameObject* GetControlObject();

	private:

		double previous_fps;
        GameObject* _control_target;
};

#endif