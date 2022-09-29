#include <PhysicsBody.h>
#include <Transform.h>
#include <Physics.h>

#include <PhysicsBodyInterface.h>
#include <GameSystemInterface.h>

//////////////////////////////////////////////
//helpers

void CreateDefaultFixture(b2Body* b)
{
	b2vec2 box[4] =
	{
		b2vec2(1, 0), b2vec2(1,1),
		b2vec2(0, 1), b2vec2(0,0)
	};

	b2PolygonShape shape;
    shape.Set( box, 4 );

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1;
    fixtureDef.friction = 0.3f;

    b->CreateFixture(&fixtureDef);
}

//
//////////////////////////////////////////////

PhysicsBody::PhysicsBody(Transform& transform)
:_child(0)
,_owner(0)
,_transform(transform)
,_root_body(0)
,_num_fixtures(0)
{}	

//this copy ctor is used by the component pool class _only_
//this is why the object being "copied" is being modified
//there is no actual copy happening, it's actually a move internally
PhysicsBody::PhysicsBody(const PhysicsBody& other)
:Component(other)
,_child(other._child)
,_owner(0)
,_transform(other._transform)
,_root_body(other._root_body)
,_num_fixtures(other._num_fixtures)
{
	PhysicsBody& non_const = const_cast<PhysicsBody&>( other );
	non_const._child = 0;
	non_const._root_body = 0;
	non_const._num_fixtures = 0;
	
	//if there was an interface pointing to the other component,
	//we need to make the pointer point to this one.
	if( _child )
	{
		_child->_parent = this;
	}
}

void PhysicsBody::KillThisComponent()
{
	if( _child )
		_child->_parent = 0;

	if( _root_body )
		External::system->GetPhysicsManager().world->DestroyBody( _root_body );
	_root_body = 0;

	_parent = 0;
}

void PhysicsBody::SyncGameObjectTransform()
{
    if(_root_body)
	{
	    _transform._pos = Convert::c_v2( _root_body->GetTransform().p ).to_glm;
	    _transform.SetAngleRadian( _root_body->GetAngle() );
	}
}


PhysicsBody::~PhysicsBody()
{
	KillThisComponent();
}
	

void PhysicsBody::CreateRootBody(b2BodyType bodytype)
{
    b2BodyDef bodyDef;
    bodyDef.type = bodytype;
    bodyDef.position.Set( _transform._pos.x, _transform._pos.y );

	if( _root_body )
	{
		External::system->GetPhysicsManager().world->DestroyBody(_root_body);
		_root_body = 0;
	}

    _root_body = External::system->GetPhysicsManager().world->CreateBody(&bodyDef);
    _root_body->SetTransform( b2vec2(_transform._pos.x,_transform._pos.y), _transform.GetAngle() );
    _root_body->SetUserData( GetParent() );

	CreateDefaultFixture(_root_body);
}

void PhysicsBody::AddFixture(std::vector<b2vec2> bounds)
{
	if(!_root_body)
		return;

	//destroy the default fixture
	if( _num_fixtures == 0 )
	{
		b2Fixture* default_fixture = _root_body->GetFixtureList();
		_root_body->DestroyFixture( default_fixture );
	}

	++_num_fixtures;

	b2PolygonShape shape;
    shape.Set( &bounds.front(), bounds.size() );

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1;
    fixtureDef.friction = 0.3f;

    _root_body->CreateFixture(&fixtureDef);
}

unsigned PhysicsBody::GetNumFixtures() const
{
	return _num_fixtures;
}

b2Fixture* PhysicsBody::GetFixture(unsigned fixture_num) const
{
	if( fixture_num >= _num_fixtures )
		return 0;

	if(_root_body)
	{
		unsigned index = 0;
		b2Fixture* fixture = _root_body->GetFixtureList();
		while( fixture && index != fixture_num )
		{
			fixture = fixture->GetNext();
			index++;
		}
		return fixture;
	}

	return 0;
}

bool PhysicsBody::RemoveFixture(unsigned fixture_num)
{
	if( fixture_num >= _num_fixtures )
		return false;
	
	if(_root_body)
	{
		unsigned index = 0;
		b2Fixture* current_fixture = _root_body->GetFixtureList();
		while( current_fixture && index != fixture_num )
		{
			if( fixture_num == index )
			{
				_root_body->DestroyFixture( current_fixture );
				return true;
			}

			current_fixture = current_fixture->GetNext();
			index++;
		}
	}
	return false;
}

bool PhysicsBody::RemoveFixture(b2Fixture* fixture)
{
	if( !fixture )
		return false;
	
	if(_root_body)
	{
		b2Fixture* current_fixture = _root_body->GetFixtureList();
		while( current_fixture )
		{
			if( fixture == current_fixture )
			{
				_root_body->DestroyFixture( current_fixture );
				return true;
			}

			current_fixture = current_fixture->GetNext();
		}
	}
	return false;	
}

void SerializeVertices(std::vector<b2vec2>& verts, SerializedObject& fixture_object)
{
	if( !fixture_object.HasObject("Vertices") )
	{
		std::cerr<<"Fixture '"<<fixture_object.GetName() <<"' requires 'Vertices'.\n";
		std::cerr<<"Cannot create a fixture with no 'Vertices'\n";
		return;
	}

	SerializedObject verts_object;
	fixture_object.GetInnerObject("Vertices", verts_object);

	verts.clear();
	FieldIterator vert_iter(verts_object);
	for(; vert_iter.Good(); vert_iter.Next() )
	{
		float vert[2] = {0,0};
		std::string field = vert_iter.GetKey();
		fixture_object.GetValue(field,vert);
		verts.push_back( b2vec2(vert[0],vert[1]) );
	}
}

void SerializeFixtureDef(b2Body* owner, b2Shape* shape, SerializedObject& fixture_object)
{
    b2FixtureDef fixture_def;
    fixture_def.shape = shape;
	
	SERIALIZE_MEMBER(fixture_object, "Density", fixture_def.density)
	SERIALIZE_MEMBER(fixture_object, "Friction", fixture_def.friction)

    owner->CreateFixture(&fixture_def);
}

void SerializeCircle(b2Body* owner, SerializedObject& fixture_object)
{
	b2CircleShape shape;
	float pos[2] = {0,0};
	
	SERIALIZE_MEMBER(fixture_object, "Radius", shape.m_radius)
	SERIALIZE_MEMBER(fixture_object, "Position", pos)
	shape.m_p = b2vec2(pos[0], pos[1]);
	
	SerializeFixtureDef(owner, &shape, fixture_object);
}

void SerializeBox(b2Body* owner, SerializedObject& fixture_object)
{
	float size[2] = {1.0f,1.0f};
	float pos[2] = {0,0};

	b2PolygonShape shape;

	SERIALIZE_MEMBER(fixture_object, "Scale", size)
	SERIALIZE_MEMBER(fixture_object, "Position", pos)

	b2vec2 box[4] =
	{
		b2vec2(size[0], 0), b2vec2(size[0],size[1]),
		b2vec2(0, size[1]), b2vec2(0,0)
	};

	box[0] += b2vec2(pos[0],pos[1]);
	box[1] += b2vec2(pos[0],pos[1]);
	box[2] += b2vec2(pos[0],pos[1]);
	box[3] += b2vec2(pos[0],pos[1]);

    shape.Set( box, 4 );

	SerializeFixtureDef(owner, &shape, fixture_object);
}

void SerializeChain(b2Body* owner, SerializedObject& fixture_object)
{
	b2ChainShape shape;
	std::vector<b2vec2> vertices;

	truth loop;
	SERIALIZE_MEMBER(fixture_object, "Loop", loop)

	SerializeVertices(vertices, fixture_object);

	//don't build a fixture that has no vertsb  
	if( vertices.empty() )
		return;

	const unsigned k_num_verts = vertices.size();
	b2vec2* verts = new b2vec2[ k_num_verts ];
	memcpy( verts, &vertices[0], k_num_verts );

	if( loop )
		shape.CreateLoop( verts, k_num_verts );
	else
		shape.CreateChain( verts, k_num_verts );
	
	SerializeFixtureDef(owner, &shape, fixture_object);

	delete verts;
}

void SerializeEdge(b2Body* owner, SerializedObject& fixture_object)
{
	b2EdgeShape shape;

	float begin[2] = {0,0};
	float end[2] = {0,0};

	SERIALIZE_MEMBER(fixture_object, "Begin", begin)
	SERIALIZE_MEMBER(fixture_object, "End", end)

	shape.Set( b2vec2(begin[0],begin[1]), b2vec2(end[0],end[1]) );

	SerializeFixtureDef(owner, &shape, fixture_object);
}

void SerializePolygon(b2Body* owner, SerializedObject& fixture_object)
{
	b2PolygonShape shape;
	std::vector<b2vec2> vertices;

	SerializeVertices(vertices, fixture_object);

	//don't build a fixture that has no vertsb  
	if( vertices.empty() )
		return;

	const unsigned k_num_verts = vertices.size();
	b2vec2* verts = new b2vec2[ k_num_verts ];
	memcpy( verts, &vertices[0], k_num_verts );

	shape.Set( verts, k_num_verts );

	SerializeFixtureDef(owner, &shape, fixture_object);

	delete verts;
}

void SerializeFixture(b2Body* owner, SerializedObject& fixture_object)
{
	const std::string k_type = fixture_object.GetName();

	if( k_type == "Circle" )
			SerializeCircle(owner, fixture_object);
	else if( k_type == "Box" )
			SerializeBox(owner, fixture_object);
	else if( k_type == "Chain" )
			SerializeChain(owner, fixture_object);
	else if( k_type == "Edge" )
			SerializeEdge(owner, fixture_object);
	else if( k_type == "Polygon" )
			SerializePolygon(owner, fixture_object);
	else
			std::cerr<<"Unrecognized fixture type "<<k_type <<" in object "<<fixture_object.GetName() <<"\n";
}

void PhysicsBody::Serialize(SerializedObject& obj)
{
	if( _root_body )
	{
		External::system->GetPhysicsManager().world->DestroyBody(_root_body);
		_root_body = 0;
	}

    b2BodyDef bodyDef;

	std::string body_type;
	SERIALIZE_MEMBER(obj, "BodyType", body_type)

	if( body_type == "Static" ) 
		bodyDef.type = b2_staticBody;
	else if( body_type == "Kinematic") 
		bodyDef.type = b2_kinematicBody;
	else if( body_type == "Dynamic" )
		bodyDef.type = b2_dynamicBody; 
	else
	{
		std::cerr<<"BodyType '"<<body_type <<"' in object "<<obj.GetName() <<" not recognized.\n"; 
		return;
	}

    bodyDef.position.Set( _transform._pos.x, _transform._pos.y );
    _root_body = External::system->GetPhysicsManager().world->CreateBody(&bodyDef);
    _root_body->SetTransform( b2vec2(_transform._pos.x,_transform._pos.y), _transform.GetAngle() );
    _root_body->SetUserData( GetParent() );	

	float angle = 0;
	float linear_vel[2] = {0,0};
	float angular_vel = 0;
	float gravity_scale = 1.0f;
	truth fixed_rotation = 0;
	
	SERIALIZE_MEMBER(obj, "Angle", angle)
	SERIALIZE_MEMBER(obj, "LinearVelocity", linear_vel)
	SERIALIZE_MEMBER(obj, "AngularVelocity", angular_vel)
	SERIALIZE_MEMBER(obj, "GravityScale", gravity_scale)
	SERIALIZE_MEMBER(obj, "FixedRotation", fixed_rotation)

	_root_body->SetTransform( _root_body->GetPosition(), angle );
	_root_body->SetLinearVelocity( b2vec2(linear_vel[0],linear_vel[1]) );
	_root_body->SetAngularVelocity( angular_vel );
	_root_body->SetGravityScale( gravity_scale );
	_root_body->SetFixedRotation( (fixed_rotation ? true : false) );

	_num_fixtures = 0;
	if( obj.HasObject("Fixtures") )
	{
		SerializedObject fixtures;
		obj.GetInnerObject("Fixtures",fixtures);
		ObjectIterator<SerializedObject> fixture_iter(fixtures);
		for(; fixture_iter.Good(); fixture_iter.Next() )
		{
			SerializedObject& fixture = fixture_iter.Get();
			ObjectIterator<SerializedObject> fixture_type(fixtures);
			SerializeFixture( _root_body, fixture_type.Get() );
		}
	}

	b2Fixture* fixture = _root_body->GetFixtureList();
	while( fixture )
	{
		fixture = fixture->GetNext();
		++_num_fixtures;
	}

	SyncGameObjectTransform();
}

/*
PhysicsBody
{		
	Fixtures
	{
		Fixture1
		{
			Circle
			{
				Radius = 5
			}
		}
		Fixture2
		{
			Box
			{
				Radius = 5
			}
		}
	}
}
*/
