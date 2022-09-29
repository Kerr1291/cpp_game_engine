#ifndef COMPONENT_H
#define COMPONENT_H

#include <DataFileIterator.h>
#include <DataFile.h>

#define BEGIN_COMPONENT( name ) \
class name : public Component { \
    public: \
      const std::string Name() { return "#name"; }\
    private: \

#define END_COMPONENT( name ) \
}; \
  struct __##name##__create__component__ { \
  __##name##__create__component__() { \
  std::cerr<<"adding component " <<#name <<"\n"; \
  __make_component__[ #name ] = new ComponentCreatorType< name >(); } \
  }; static __##name##__create__component__ __add__##name##__to__list;

#define SERIALIZE_MEMBER( serialized_object, key_name, variable ) \
	if( serialized_object.HasValue( key_name ) ) \
		serialized_object.GetValue( key_name, variable ); \
	else \
		std::cerr<< "Cannot find '"<<key_name <<"' in object '"<<serialized_object.GetName() <<"'\n";

class PhysicsBody;
class Renderable;
class GameObject;
class Component
{
	template <typename T> friend class ComponentPool;
	friend class ComponentCreator;
	friend class GameObject;
	friend class PhysicsBody;
	friend class Renderable;
		GameObject* _parent;
	protected:
		Component():_parent(0){};
		Component(const Component& other)
			:_parent(other._parent) {}
		GameObject* GetParent() { return _parent; }
        virtual const std::string Name() = 0;
        virtual void Serialize(SerializedObject& obj) = 0;
		virtual void Initialize(){};
		virtual void RespondToCollision(GameObject* object_collided_with){}
};


class ComponentCreator
{
public:
	ComponentCreator(){}
	void SetParent(GameObject* parent, Component* c) { c->_parent = parent; }
	virtual Component* Create(GameObject* parent) = 0;
};

template <typename T>
class ComponentCreatorType : public ComponentCreator
{
	public:
		ComponentCreatorType(){}
		Component* Create(GameObject* parent) 
		{ 
			T* t = new T();
			SetParent(parent, t);
			return t;
		}
};

static std::map< std::string, ComponentCreator* > __make_component__;

#endif