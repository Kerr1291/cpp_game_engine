#include <GameObject.h>
#include <DataFile.h>
#include <DataFileIterator.h>
#include <Component.h>

#include <SystemInterface.h>
#include <PhysicsBody.h>
#include <Renderable.h>

//Helper function
bool IsEngineComponent(const std::string& component_name)
{
	if( component_name == "PhysicsBody" )
		return true;
	if( component_name == "Renderable" )
		return true;

	return false;
}

GameObject::GameObject(const std::string name)
:_name(name)
,_render_layer(0)
,_physics_body(0)
,_renderable(0)
{
}

/* //TODO: implement copying later
GameObject::GameObject(const GameObject& other)
:_name(other.GetName())
,_components(other._components)
{
}

GameObject& GameObject::operator=(const GameObject& other)
{
	(*const_cast<std::string*>(&_name)) = other._name; 
	_components = other._components;
	return *this; 
}
*/

GameObject::~GameObject()
{
	std::map<std::string,Component*>::iterator iter = _components.begin();
	for(; iter != _components.end(); ++iter)
	{
		delete iter->second;
	}

	if( _renderable )
		_renderable->_owner->KillComponent( _renderable );
	
	if( _physics_body )
		_physics_body->_owner->KillComponent( _physics_body );

	//TODO: tell the owner pool to 'kill' this component
	//delete _physics_body;
	//delete _renderable;
}

std::string GameObject::GetName() const
{
	return _name; 
} 

void GameObject::SerializeComponents(SerializedObject& obj)
{
	if( !obj.HasObject("Transform") )
	{
		std::cerr<<"All objects require a transform. Aborting serialize of object "<<obj.GetName() <<"\n";
		return;
	}

    SerializedObject transform_obj;
	obj.GetInnerObject("Transform",transform_obj);
	_transform.Serialize(transform_obj);

    ObjectIterator<SerializedObject> iter(obj);
    for(; iter.Good(); iter.Next() )
    {
        SerializedObject& current_component = iter.Get();
        const std::string& component_name = current_component.GetName();

        //pass the seralizer object to the component so it can serialize itself
		if( AddComponent(component_name) )
			_components[ component_name ]->Serialize(current_component);
    }
}
		
Component* GameObject::GetComponent(const std::string& name)
{
	if( name == "Transform" )
		return &_transform;

	std::map<std::string,Component*>::iterator finder = _components.find( name );
	if( finder == _components.end() )
		return 0;

	return finder->second;
}

bool GameObject::AddComponent(const std::string& component_name)
{
	if( component_name == "Transform" )
		return false; //skip transform always

    //get ready to allocate the component
    Component* new_component = 0;

	//for engine defined components we manually create them
	//to allow them to be created to their specific needs
	//engine components are not added to the internal component map
	if( IsEngineComponent( component_name ) )
	{
		CreateEngineComponent( component_name );
		return true;
	}	
	
	if( __make_component__.find( component_name ) == __make_component__.end() )
		return false; //unrecognized component (TODO: print error for unrecognized component)

	if( _components.find( component_name ) != _components.end() )
		return false; //don't add two of the same component to this object (TODO: print error for double component add)

	__make_component__[ component_name ]->Create(this);

    //add it to the object
    _components[ component_name ] = new_component;
	return true;
}

bool GameObject::RemoveComponent(const std::string& component_name)
{
	if( IsEngineComponent(component_name) )
	{
		RemoveEngineComponent(component_name);
		return true;
	}

	std::map<std::string,Component*>::iterator finder = _components.find( component_name );
	if( finder == _components.end() )
		return false;

	//remove the component from the collision map if it's there
	std::set<Component*>::iterator set_finder = _collision_response_components.find( finder->second );
	if( set_finder != _collision_response_components.end() )
		_collision_response_components.erase( set_finder );

	delete finder->second;
	_components.erase( finder );
	return true;
}

//If you want a component to respond to collision it needs to be added to the
//collision response set on this object after it has been added to the object
void GameObject::RegisterCollisionComponent(const std::string& component_name)
{
	std::map<std::string,Component*>::iterator finder = _components.find( component_name );
	if( finder == _components.end() )
		return; //TODO: report that the component doesn't exist on this object

	_collision_response_components.insert( finder->second );
}

//call the collision response function for all components
//that care about collisions
void GameObject::DoCollision(GameObject* object_collided_with)
{
	std::set<Component*>::iterator iter = _collision_response_components.begin();
	for(; iter != _collision_response_components.end(); ++iter)
	{
		(*iter)->RespondToCollision(object_collided_with);
	}
}

void GameObject::Initialize()
{
	std::map<std::string,Component*>::iterator iter = _components.begin();
	for(; iter != _components.end(); ++iter)
	{
		iter->second->Initialize();
	}
}

void GameObject::CreateEngineComponent(const std::string& component_name)
{
	if( component_name == "PhysicsBody" )
	{
		if( _physics_body )
			_physics_body->_owner->KillComponent( _physics_body );
		External::system->GetPhysicsManager().MakePhysicsBody(this);
	}	
	if( component_name == "Renderable" )
	{
		if( _renderable )
			_renderable->_owner->KillComponent( _renderable );
		//_renderable = //TODO: call MakeComponent from render manager //new Renderable( _transform, External::system->GetRenderManager() );
		_renderable->_parent = this;
	}	
	else
	{
		std::cerr<<"Unknown component. This is not an engine component\n";
	}
}

		
void GameObject::RemoveEngineComponent(const std::string& component_name)
{
	if( component_name == "PhysicsBody" )
	{
		if( _physics_body )
			_physics_body->_owner->KillComponent( _physics_body );
		_physics_body=0;
	}	
	if( component_name == "Renderable" )
	{
		if( _renderable )
			_renderable->_owner->KillComponent( _renderable );
		_renderable=0;
	}	
	else
	{
		std::cerr<<"Unknown component. This is not an engine component\n";
	}
}


void GameObject::SetEngineComponent(PhysicsBody* physics_body)
{
	_physics_body = physics_body;
}

void GameObject::SetEngineComponent(Renderable* renderable)
{
	_renderable = renderable;
}

