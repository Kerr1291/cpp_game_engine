#include <CustomObject.h>

#include <RenderObject.h>
#include <Sprite.h>

#include <PhysicsObject.h>
#include <Physics.h>

CustomObject::CustomObject()
:object_type("CustomObject")
,object_name("Custom Object")
{
	t.loc = Global::Mouse::WorldPos;
    t.SetAngle( 0 );
	t.scale = vec2(1.0f,1.0f);

    gfx = new RenderObject( &t );
	Sprite* s = new Sprite( &t );
	s->LoadSprite(Path::GraphicAssets+"atlas_blank.anim");
	gfx->sprite = s;

    //phys = new PhysicsObject( this, &t, External::physics->LoadBoundingBox(Path::GraphicAssets+"default.bb") );
    //phys->body->SetTransform( b2vec2(t.loc.x,t.loc.y), -t.GetAngleRadians() );

	phys = new PhysicsObject( this );
	phys->CreateRootBody( b2_dynamicBody );
	phys->SetBodyTransform( t.loc, -t.GetAngleRadians() );
	phys->AddFixture( External::physics->LoadBoundingBox(Path::GraphicAssets+"Human.bb") );
}

CustomObject::~CustomObject()
{
    delete phys;
}

std::string CustomObject::GetTypeName()
{
    return object_type;
}

std::string CustomObject::GetTextName()
{
    return object_name;
}

void CustomObject::Update(float dt)
{
	if( phys && phys->IsAwake() )
		SyncTransformToPhysics();

	if( gfx->sprite )
		gfx->sprite->Update( dt );
}

void CustomObject::DoCollision(BaseObject* objectCollidedWith)
{
	//TODO: pass collided bodies to attached "scripts"/components

        //std::cout<<"rock at "<<t.loc.x <<" ,"<<t.loc.y <<" collided with a "<<objectCollidedWith->GetTypeName() <<" at "<<objectCollidedWith->t.loc.x <<" ,"<<objectCollidedWith->t.loc.y <<"\n";

        //if( objectCollidedWith->GetTypeName() == "EnemyShip" )
        //{
                //External::objects->RemoveObject( objectCollidedWith );
        //}
}

