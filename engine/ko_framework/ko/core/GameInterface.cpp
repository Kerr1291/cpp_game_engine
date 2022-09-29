#include <GameInterface.h>
#include <GUIManager.h>

#include <Parser.h>

#include <Camera.h>

#include <Area.h>
#include <Command.h>
#include <GridInfo.h>

//Areas
#include <Area_Beta.h>

//Commands
#include <LoadArea.h>
#include <UnloadArea.h>
#include <EnterMenu.h>

//HUDs
#include <BasicHUD.h>

GameInterface::GameInterface(INIFile* configFile)
:config(configFile)
,showHUD(1)
,inmenu(0)
,loading(0)
,loadingCounter(0)
,loadingDirection(0)
,loadingPoint( vec2(100,100) )
,gamefps(0.016)
,currentCam(0)
,currentArea(0)
{
    //External::gi = this;
}

GameInterface::~GameInterface()
{
        delete currentCam;
        delete currentArea;
}

void GameInterface::Initialize()
{
        currentCam = new Camera();

        physics.Init();
		audio.Init();

        player.Init();

        mainmenu.Load();
        inmenu = 1;

        hudDisplay.AddHUD( "BasicHUD", new BasicHUD() );
        hudDisplay.SetActiveHUD( "BasicHUD" );

        //TODO: load the menu or first level

        /*
        player = new Player();
        player->SetWorld( this );
        gameObjects.AddObject( player );
        */
}

void GameInterface::SetLoading(truth isLoading)
{
        loading = isLoading;
        if( !loading )
        {
                loadingCounter = 0;
                loadingDirection = 0;
                loadingPoint = vec2(100,100);
        }
}

void GameInterface::SetMainMenu(truth inMainMenu)
{
        inmenu = inMainMenu;
}

void GameInterface::UnloadCurrentArea()
{
        if(!currentArea)
            return;

		Global::SetDebugMode(0);
        player.SetControlTarget(0);
        currentArea->Cleanup();
        delete currentArea;
        currentArea = 0;
        External::physics->Cleanup();
}

void GameInterface::SetArea(Area* area)
{
        if(!area)
            return;

		Global::SetDebugMode(0);
        currentArea = area;
}

Area* GameInterface::GetCurrentArea()
{
        return currentArea;
}

void GameInterface::InitCurrentArea()
{
        if(currentArea)
            currentArea->Init();
}

void GameInterface::ToggleHUD()
{
	showHUD = (showHUD ? 0 : 1);
}

void GameInterface::Input(double fps)
{
        if( loading )
            return;

		//update the mouse's world position
		Global::Mouse::WorldPos = ScreenToWorld( Global::Mouse::ScreenPos );

        //special keys go here, these will activate before any others
        //if( Global::Keyboard::KeyPressed( '0' ) )
        //{
        //        Global::SetDebugMode(( Global::GetDebugMode() ? 0 : 1 ));
        //}

        //if( Global::Keyboard::KeyPressed('\\') )
        //{
        //        showHUD = (showHUD ? 0 : 1);
        //}

		if( Global::GetDebugMode() )
		{
			if( Global::Keyboard::KeyPressed('`') )
			{
					//AddCommand( new UnloadArea( ) );
					//AddCommand( new EnterMenu( ) );
                    mainmenu.Load();//TODO: move this? think about how the "initial" game screen should be loaded more
			}
		}

        #if 0 //old test code to swap areas, leave in for now as example
        static truth a = 0;
        static truth b = 0;
        if( keys[ 'f' ] )
        {
                if(!a++)
                {
                        AddCommand( new UnloadArea( this ) );
                        AddCommand( new LoadArea<Alpha>( this ) );
                        b = 0;
                        //AddCommand( new ChangeArea(this, new Alpha(*this)) );
                }
        }

        if( keys[ 'g' ] )
        {
                if(!b++)
                {
                        AddCommand( new UnloadArea( this ) );
                        AddCommand( new LoadArea<Beta>( this ) );
                        a = 0;
                }
                //AddCommand( new ChangeArea(this, new Beta(*this)) );
        }
        #endif

        if( External::gui->GetFocus() )
            return;

        //currentArea->Input( fps );
        player.Input( fps );
        currentCam->Input( fps );
}

void GameInterface::Update(double fps)
{
		audio.Update(fps);

        if(loading)
                return;

        if(inmenu)
        {
                mainmenu.Update();
                return;
        }

        player.Update(fps);

        if(currentArea)
            currentArea->UpdateArea();

        gameObjects.Update(fps);
        physics.Update(fps);
        currentCam->SyncWithTarget(fps);
        hudDisplay.Update();
}

void GameInterface::ProcessCommands()
{
        if(commands.empty())
                return;

        std::list<Command*>::iterator iter = commands.begin();
        while( iter != commands.end() )
        {
                if( (*iter)->IsComplete() )
                {
                        if( (*iter)->managed == CommandType::MANAGED )
                                delete *iter;
                        else
                                (*iter)->Reset();
                        iter = commands.erase( iter );
                }
                else
                {
                        (*iter)->Execute();
                        if((*iter)->blocking)
                                break;
                        ++iter;
                }
        }
}

void GameInterface::Draw()
{
        if(loading)
        {
                DrawLoadingScreen();
                return;
        }

        if(inmenu)
        {
			mainmenu.Draw();
			if(Global::GetDebugMode())
				Debug::Internal::PrintStrings();

            return;
        }

		renderer.PreRender();

        if(currentArea)
		{
            currentArea->CustomDraw();
//			if( currentArea->areaGrid )
//				renderer.RenderGrid( *currentArea->areaGrid, 0 );
		}

        gameObjects.Draw();

//        if(currentArea && currentArea->areaGrid )
//			renderer.RenderGrid( *currentArea->areaGrid, 1 );

//		renderer.RenderPostProcessing();

        if(showHUD)
            hudDisplay.Draw();

        if(Global::GetDebugMode())
            Debug::Internal::PrintStrings();

		if(physics.debugDrawEnabled)
            physics.DoDebugDraw();
}

void GameInterface::AddCommand( Command* c )
{
        commands.push_back( c );
}

void GameInterface::AddObject( BaseObject* c )
{
        gameObjects.AddObject( c );
}

void GameInterface::RemoveObject( BaseObject* c )
{
	BaseObject* player = 0;
	if( External::player->GetControlTarget() )
		player = External::player->GetControlTarget();

	if( player && player == c )
		External::player->SetControlTarget( 0 );

    gameObjects.RemoveObject( c );
}

void GameInterface::Cleanup()
{
    gameObjects.Cleanup();
}


truth GameInterface::HasPendingCommand( Command* c )
{
    if(commands.empty())
        return 0;

    std::list<Command*>::iterator iter = commands.begin();
    for(; iter != commands.end(); ++iter )
    {
        if( (*iter) == c )
            return 1;
    }

    return 0;
}

vec2 GameInterface::ScreenToWorld(vec2 screen_pos)
{
	if( GetCamera() )
	{
        vec3 v_in( screen_pos.x, Global::GameWindow->GetH() - screen_pos.y, 0 );
		vec3 v_out( 0, 0, 0 );
		mat4 model(1.0f);
		vec4 viewport( 0, 0, Global::GameWindow->GetW(), Global::GameWindow->GetH() );
		v_out = glm::unProject( v_in, model, GetCamera()->GetProjectionMatrixGLM(), viewport );

		return vec2(v_out.x,v_out.y);
	}

	return screen_pos;
}


/////////////////////////////////////////////////////////
//Private Methods


/////////////////////////////////////////////////////////////////////////////
// This function draws a coordinate system cross in the center of the world
// If you want it bigger/smaller just change the 'num' value.
void GameInterface::DrawWorldCenteredDebugCross()
{
        const unsigned num = 20;
        const float offset = num;

        glBegin(GL_LINES);

        glColor3f(0,0,1);
        glVertex3f( -1, 0, 0 );
        glVertex3f( 1, 0, 0 );

        for(unsigned i = 0; i < num; ++i)
        {
                glVertex3f( (i*2.0f) - offset, 0, 0 );
                glVertex3f( (i*2.0f+1.0f) - offset, 0, 0 );
        }

        glColor3f(0,1,0);
        glVertex3f( 0, -1, 0 );
        glVertex3f( 0, 1, 0 );

        for(unsigned i = 0; i < num; ++i)
        {
                glVertex3f( 0, (i*2.0f) - offset, 0 );
                glVertex3f( 0, (i*2.0f+1.0f) - offset, 0 );
        }

        glColor3f(1,0,0);
        glVertex3f( 0, 0, -1 );
        glVertex3f( 0, 0, 1 );

        for(unsigned i = 0; i < num; ++i)
        {
                glVertex3f( 0, 0, (i*2.0f) - offset );
                glVertex3f( 0, 0, (i*2.0f+1.0f) - offset );
        }


        glEnd();
}

void GameInterface::DrawLoadingScreen()
{
        loadingCounter++;
        if( loadingCounter > 100 )
        {
                loadingDirection++;
                loadingDirection = loadingDirection%4;
                loadingCounter = 0;
        }

        const float distance = 1.0f;
        if( loadingDirection == 0 )
                loadingPoint.x += distance;
        else if( loadingDirection == 1 )
                loadingPoint.y += distance;
        else if( loadingDirection == 2 )
                loadingPoint.x -= distance;
        else if( loadingDirection == 3 )
                loadingPoint.y -= distance;

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        int w = Global::GameWindow->GetW();
        int h = Global::GameWindow->GetH();
        gluOrtho2D(0, w, h, 0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor3f(0.5,0.5,0.5);
        glPointSize(3);
        glBegin(GL_POINTS);
        glVertex2f( loadingPoint.x, loadingPoint.y );
        glEnd();
        glPointSize(1);

        DebugDraw loader;
        if( loadingDirection == 0 )
                loader.DrawString(50, 50, "Loading.");
        else if( loadingDirection == 1 )
                loader.DrawString(50, 50, "Loading..");
        else if( loadingDirection == 2 )
                loader.DrawString(50, 50, "Loading...");
        else if( loadingDirection == 3 )
                loader.DrawString(50, 50, "Loading....");


        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
}

