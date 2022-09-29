#include <GroundTileA.h>

#include <RenderObject.h>
#include <GraphicData.h>
#include <RenderManager.h>
#include <ObjectManager.h>

GroundTileA::GroundTileA()
{

        float GroundColor[4] = {0.4,0.2,0.0,1};
        //float GroundColor[4] = {0.0,1,0.0,1};
        gfx = External::renderer->CreateRenderObject( &t, External::renderer->LoadGfxFromBMP( "./Graphics/GroundTile.bmp" ) );
        gfx->SetColor(0, GroundColor[0], GroundColor[1], GroundColor[2]);
        gfx->gfx->solid[0] = 1;

        t.loc = vec2(100,100);
        //t.scale = 10.0f;

        //velocity = vec2( -10 + 20*Global::GetRandomFloat(), -10 + 20*Global::GetRandomFloat() );


        phys = 0;
}

GroundTileA::~GroundTileA()
{
}

std::string GroundTileA::GetTypeName()
{
        return "GroundTileA";
}

std::string GroundTileA::GetTextName()
{
        return "GroundTileA";
}

void GroundTileA::Update(float dt)
{
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

void GroundTileA::DoCollision(BaseObject* objectCollidedWith)
{
        //std::cout<<"GroundTileA at "<<t.loc.x <<" ,"<<t.loc.y <<" collided with a "<<objectCollidedWith->GetTypeName() <<" at "<<objectCollidedWith->t.loc.x <<" ,"<<objectCollidedWith->t.loc.y <<"\n";


}
