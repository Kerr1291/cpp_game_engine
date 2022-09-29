
#include <Human.h>

#include <RenderObject.h>
#include <GraphicData.h>
#include <RenderManager.h>
#include <PhysicsObject.h>
#include <Physics.h>

Human::Human()
{
        float humanColorA[4] = {0,0,1,1};
        float humanColorB[4] = {0,1,0,1};

        gfx = External::renderer->CreateRenderObject( &t, External::renderer->LoadGfxFromBMP( "./Graphics/Human.bmp" ) );

        gfx->SetColor(0,humanColorA[0],humanColorA[1],humanColorA[2]);
        gfx->SetColor(1,humanColorB[0],humanColorB[1],humanColorB[2]);

        gfx->gfx->solid[0] = 1;
        gfx->gfx->solid[1] = 1;

        t.loc = vec2(10,10);
        t.SetAngle(360.0f * Global::GetRandomFloat());

        //velocity = vec2( 5 + 20*Global::GetRandomFloat(), 5 + 20*Global::GetRandomFloat() );


        phys = new PhysicsObject( this, &t, External::physics->LoadBoundingBox("./Graphics/Human.bb") );
        phys->body->SetTransform( b2Vec2(t.loc.x,t.loc.y), -t.GetAngleRadians() );
        phys->body->SetLinearDamping( 2.5f );
        phys->body->SetAngularDamping( 0.9995f );

        prop.AddProperty("HP", 10);
        prop.AddFlag("Player", 0);
}

Human::~Human()
{
        delete phys;
}

std::string Human::GetTypeName()
{
        return "Human";
}


std::string Human::GetTextName()
{
        return "Humanoid";
}

void Human::Update(float dt)
{
        if(!phys->body->IsAwake())
                return;

        SyncTransformToPhysics();

        if( t.loc.x < 0 )
                phys->body->SetTransform( b2vec2(Global::GameWorldSize.x,t.loc.y), t.GetAngleRadians() );
        if( t.loc.x > Global::GameWorldSize.x )
                phys->body->SetTransform( b2vec2(0,t.loc.y), t.GetAngleRadians() );
        if( t.loc.y < 0 )
                phys->body->SetTransform( b2vec2(t.loc.x,Global::GameWorldSize.y), t.GetAngleRadians() );
        if( t.loc.y > Global::GameWorldSize.y )
                phys->body->SetTransform( b2vec2(t.loc.x,0), t.GetAngleRadians() );
}

void Human::DoCollision(BaseObject* objectCollidedWith)
{
        //std::cout<<"ship at "<<t.loc.x <<" ,"<<t.loc.y <<" collided with a "<<objectCollidedWith->GetTextName() <<" at "<<objectCollidedWith->t.loc.x <<" ,"<<objectCollidedWith->t.loc.y <<"\n";
}
