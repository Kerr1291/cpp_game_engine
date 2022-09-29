#include <PlanetA.h>

#include <RenderObject.h>
#include <GraphicData.h>
#include <RenderManager.h>
#include <PhysicsObject.h>
#include <Physics.h>
#include <ObjectManager.h>

PlanetA::PlanetA()
{
        syncComplete = 0;

        float rockColor[4] = {0,1,0,1};
        gfx = External::renderer->CreateRenderObject( &t, External::renderer->LoadGfx( "./Graphics/planet.gfx" ) );
        //memcpy( gfx->color, rockColor, sizeof(float)*4 );
        for(unsigned i = 0; i < gfx->colors.size(); ++i)
                gfx->SetColor(i, rockColor[0], rockColor[1], rockColor[2]);
        gfx->gfx->solid[0] = 1;
        t.loc = vec2(100,100);
        t.SetAngle( 360.0f * Global::GetRandomFloat() );
        //t.scale = 5.0f + 10.0f * Global::GetRandomFloat();

        //velocity = vec2( -10 + 20*Global::GetRandomFloat(), -10 + 20*Global::GetRandomFloat() );


        phys = new PhysicsObject( this, &t, External::physics->LoadBoundingBox("./Graphics/planet.bb"), b2_kinematicBody );

        phys->body->SetTransform( b2vec2(t.loc.x,t.loc.y), -t.GetAngleRadians() );
        //phys->body->SetLinearVelocity( velocity );
}

PlanetA::~PlanetA()
{
        delete phys;
}

std::string PlanetA::GetTypeName()
{
        return "PlanetA";
}

std::string PlanetA::GetTextName()
{
        return "PlanetA";
}

void PlanetA::Update(float dt)
{
        float a = phys->body->GetAngle();
        a += dt;
        phys->body->SetTransform( b2vec2(t.loc.x,t.loc.y), a );
        //if(!syncComplete)
        SyncTransformToPhysics();
        //syncComplete=1;
}

void PlanetA::DoCollision(BaseObject* objectCollidedWith)
{
}

