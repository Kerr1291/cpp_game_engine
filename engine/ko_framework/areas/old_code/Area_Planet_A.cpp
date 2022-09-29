#include <Area_Planet_A.h>

#include <GameInterface.h>

#include <Camera.h>
#include <PlayerInterface.h>

#include <AreaLoader.h>

#include <LoadGameObject.h>
#include <SetLoadingScreen.h>
#include <ImportAreaData.h>


////////////////////////////////////////
// Objects in this area

AreaPlanetA::AreaPlanetA()
{
}

AreaPlanetA::~AreaPlanetA()
{
}

std::string AreaPlanetA::GetName()
{
        return "AreaPlanetA";
}

void AreaPlanetA::Init()
{
        External::gi->AddCommand( new SetLoadingScreen( 1 ) );
        //Global::GameWorldSize = vec2(100,100);

		//load the objects
        //std::list< ColorData > colordata;
        //AreaLoader::LoadColorData("./Graphics/AreaPlanetA.bmp", colordata);

        //ColorKey* key = new ColorKey();
        //AddColorType((*key), 255,0,0, Human);

        //AddColorType((*key), 0,255,0, Human);
        //AddColorType((*key), 0,254,0, Human);
        //AddColorType((*key), 0,253,0, Human);
        //AddColorType((*key), 0,252,0, Human);

        //AddColorType((*key), 100,100,100, GroundTileA);
        //AddColorType((*key), 0,0,255, EnemyShip);
        //AddColorType((*key), 0,255,0, StaticRock);
        //AddColorType((*key), 254,254,0, AreaPlanetA);
        //key.data[ Color::MakeRGB(255,0,0) ] = new ObjectCreatorType<Rock>(this);

        //AreaLoader::ImportDataToArea(this,colordata,key);

        //External::gi->AddCommand( new ImportAreaData( this, colordata, key ) );
        External::gi->AddCommand( new SetLoadingScreen( 0 ) );


        //Human* r = CreateAreaObject( Human );
        //r->SetPosition( vec2(5, 5) );
        //r->SetSleepState(0);

        //External::player->SetControlTarget(r);
        //External::gi->GetCamera()->SetWindowPos(5,5);
}

void AreaPlanetA::UpdateArea()
{
}

void AreaPlanetA::Cleanup()
{
}

void AreaPlanetA::CustomDraw()
{

}
