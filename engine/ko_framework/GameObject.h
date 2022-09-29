#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <Transform.h>

bool IsEngineComponent(const std::string& component_name);

class PhysicsBody;
class Renderable;

class GameObject
{
	friend class Camera;
	friend class RenderManager;
	friend class ComponentPool<PhysicsBody>;
	friend class ComponentPool<Renderable>;
	friend class Component;
	public:
        GameObject(const std::string name);
        //GameObject(const GameObject& other);
        //GameObject& operator=(const GameObject& other);
		~GameObject();
        std::string GetName() const;
        void SerializeComponents(SerializedObject& obj);
		Component* GetComponent(const std::string& name);
		bool AddComponent(const std::string& component_name);
		bool RemoveComponent(const std::string& component_name);
		void RegisterCollisionComponent(const std::string& component_name);
		void DoCollision(GameObject* object_collided_with);
		void Initialize();
	private:

		void CreateEngineComponent(const std::string& component_name);
		void RemoveEngineComponent(const std::string& component_name);

		void SetEngineComponent(PhysicsBody* physics_body);
		void SetEngineComponent(Renderable* renderable);

		const std::string	 _name;
		int					 _render_layer;
		Transform			 _transform;
		PhysicsBody*		 _physics_body;
		Renderable*			 _renderable;
		std::map<std::string,Component*> _components;
		std::set<Component*> _collision_response_components;
};


#endif