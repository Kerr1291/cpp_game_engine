#include <Rock.h>

#include <RenderObject.h>
#include <GraphicData.h>
#include <RenderManager.h>
#include <PhysicsObject.h>
#include <Physics.h>
#include <ObjectManager.h>
#include <Sprite.h>
#include <GridInfo.h>

Rock::Rock()
{
        float rockColor[4] = {1,1,1,1};
        gfx = External::renderer->CreateRenderObject( &t, External::renderer->LoadGfx( "./Graphics/rock.gfx" ) );
        //memcpy( gfx->color, rockColor, sizeof(float)*4 );
        for(unsigned i = 0; i < gfx->colors.size(); ++i)
                gfx->SetColor(i, rockColor[0], rockColor[1], rockColor[2]);
        gfx->gfx->solid[0] = 1;
        gfx->gfx->solid[1] = 1;

        t.loc = vec2(100,100);
        t.SetAngle( 0 );
       // t.SetAngle( 360.0f * Global::GetRandomFloat() );

        gfx->texture = External::renderer->LoadTexture("./Graphics/testTexture.png"); // now unused, should remove

		Sprite* s = new Sprite( &t );
		s->LoadSprite("./Graphics/atlas_mobs.anim");
		s->SetAnimation("skeleton_small");
		gfx->sprite = s;

        //velocity = vec2( -10 + 20*Global::GetRandomFloat(), -10 + 20*Global::GetRandomFloat() );
		//t.scale = vec2(0.9f,1.5f);

        //phys = new PhysicsObject( this, &t, External::physics->LoadBoundingBox("./Graphics/Human.bb") );
        //phys->body->SetTransform( b2vec2(t.loc.x,t.loc.y), -t.GetAngleRadians() );
        //phys->body->SetLinearVelocity( velocity );

		t.scale = vec2(1.0f,1.5);
			
		phys = new PhysicsObject( this );
		phys->CreateRootBody( b2_dynamicBody );
		phys->SetBodyTransform( t.loc, -t.GetAngleRadians() );
		phys->AddFixture( External::physics->LoadBoundingBox(Path::GraphicAssets+"Human.bb") );
}

Rock::~Rock()
{
        delete phys;
}

std::string Rock::GetTypeName()
{
        return "Rock";
}

std::string Rock::GetTextName()
{
        return "Rock";
}

void Rock::Update(float dt)
{
		if( gfx->sprite )
			gfx->sprite->Update( dt );

        if(phys->IsAwake())
		{
			SyncTransformToPhysics();
			
			if( External::physics->GetCurrentGrid()->IsInGrid( t.loc ) == 0 )
                phys->SetBodyTransform( vec2(10,10), -t.GetAngleRadians() );
		}


        //if( t.loc.x < 0 )
		//
        //        phys->body->SetTransform( b2vec2(Global::GameWorldSize.x,t.loc.y), -t.GetAngleRadians() );
        //if( t.loc.x > Global::GameWorldSize.x )
        //        phys->body->SetTransform( b2vec2(0,t.loc.y), -t.GetAngleRadians() );
        //if( t.loc.y < 0 )
        //        phys->body->SetTransform( b2vec2(t.loc.x,Global::GameWorldSize.y), -t.GetAngleRadians() );
        //if( t.loc.y > Global::GameWorldSize.y )
        //        phys->body->SetTransform( b2vec2(t.loc.x,0), -t.GetAngleRadians() );
}

void Rock::DoCollision(BaseObject* objectCollidedWith)
{
        //std::cout<<"rock at "<<t.loc.x <<" ,"<<t.loc.y <<" collided with a "<<objectCollidedWith->GetTypeName() <<" at "<<objectCollidedWith->t.loc.x <<" ,"<<objectCollidedWith->t.loc.y <<"\n";

        //if( objectCollidedWith->GetTypeName() == "EnemyShip" )
        //{
        //        //External::objects->RemoveObject( objectCollidedWith );
        //}
}

