#include <Rock_Static.h>

#include <RenderObject.h>
#include <GraphicData.h>
#include <RenderManager.h>
#include <PhysicsObject.h>
#include <Physics.h>
#include <ObjectManager.h>

StaticRock::StaticRock()
{
        syncComplete = 0;

        float rockColor[4] = {0,1,0,1};
        gfx = External::renderer->CreateRenderObject( &t, External::renderer->LoadGfx( "./Graphics/rock.gfx" ) );
        //memcpy( gfx->color, rockColor, sizeof(float)*4 );
        for(unsigned i = 0; i < gfx->colors.size(); ++i)
                gfx->SetColor(i, rockColor[0], rockColor[1], rockColor[2]);
        t.loc = vec2(100,100);
        t.SetAngle( 360.0f * Global::GetRandomFloat() );
        //t.scale = 0.5f;

        //velocity = vec2( -10 + 20*Global::GetRandomFloat(), -10 + 20*Global::GetRandomFloat() );


        phys = new PhysicsObject( this, &t, External::physics->LoadBoundingBox("./Graphics/rock.bb"), b2_staticBody );

        phys->body->SetTransform( b2vec2(t.loc.x,t.loc.y), -t.GetAngleRadians() );
        //phys->body->SetLinearVelocity( velocity );
}

StaticRock::~StaticRock()
{
        delete phys;
}

std::string StaticRock::GetTypeName()
{
        return "StaticRock";
}

std::string StaticRock::GetTextName()
{
        return "StaticRock";
}

void StaticRock::Update(float dt)
{
        if(!syncComplete)
                SyncTransformToPhysics();
        syncComplete=1;
        //
        //if( t.loc.x < 0 )
        //      phys->body->SetTransform( vec2(Global::GameWorldSize.x,t.loc.y), -t.GetAngleRadians() );
        //if( t.loc.x > Global::GameWorldSize.x )
        //      phys->body->SetTransform( vec2(0,t.loc.y), -t.GetAngleRadians() );
        //if( t.loc.y < 0 )
        //      phys->body->SetTransform( vec2(t.loc.x,Global::GameWorldSize.y), -t.GetAngleRadians() );
        //if( t.loc.y > Global::GameWorldSize.y )
        //      phys->body->SetTransform( vec2(t.loc.x,0), -t.GetAngleRadians() );
}

void StaticRock::DoCollision(BaseObject* objectCollidedWith)
{
        //std::cout<<"StaticRock at "<<t.loc.x <<" ,"<<t.loc.y <<" collided with a "<<objectCollidedWith->GetTypeName() <<" at "<<objectCollidedWith->t.loc.x <<" ,"<<objectCollidedWith->t.loc.y <<"\n";

        if( objectCollidedWith->GetTypeName() == "EnemyShip" )
        {
                if( objectCollidedWith->prop.HasProperty("HP") )
                {
                        objectCollidedWith->prop.ModifyProperty("HP", -1);
                        std::cerr<<"Object "<<objectCollidedWith->GetTextName()
                                <<" now has "<<objectCollidedWith->prop.GetProperty("HP")
                                <<" HP remaining\n";
                }
                //External::objects->RemoveObject( objectCollidedWith );
        }
}

