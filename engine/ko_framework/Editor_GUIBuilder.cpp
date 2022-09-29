#include <Editor_GUIBuilder.h>
#include <Editor.h>

#include <SystemInterface.h>

#include <GUIManager.h>
#include <GUIBuilder.h>
#include <GUIField.h>

#include <RenderManager.h>
#include <GridInfo.h>

//Commands
#include <CreateArea.h>
//


#include <fstream>

static Editor*	   _editor  = 0;
static GUIManager* _gui		= 0;

namespace GUI
{

namespace EditorGUIBuilder
{


namespace BuilderGlobals
{
    namespace NewAreaVars
    {
        static std::string name = "";
        static unsigned grid_size_x = 0;
        static unsigned grid_size_y = 0;
    }
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// Callbacks used by buttons and things
//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//Button callbacks
namespace Callbacks
{

void TogglePaintingMode(void* null)
{
    //if( External::editor->GetMode() == EditorMode::PAINT )
	//{
	//}
    //External::editor->SetMode( EditorMode::PAINT );
	CreatePaintingEditorWindow();
}


void TogglePhysicsMode(void* null)
{
    if( External::editor->GetMode() == EditorMode::PHYSICS )
        External::editor->SetMode( EditorMode::SELECTION );
    else
        External::editor->SetMode( EditorMode::PHYSICS );
}

void ToggleLightsMode(void* null)
{
    if( External::editor->GetMode() == EditorMode::LIGHTS )
        External::editor->SetMode( EditorMode::SELECTION );
    else
        External::editor->SetMode( EditorMode::LIGHTS );
}

void ToggleObjectsMode(void* null)
{
    if( External::editor->GetMode() == EditorMode::OBJECTS )
        External::editor->SetMode( EditorMode::SELECTION );
    else
        External::editor->SetMode( EditorMode::OBJECTS );
}

void ToggleFillMode(void* null)
{
    External::editor->ToggleFillMode();
}

void ToggleDisabled(void* null)
{
    External::editor->ToggleDisabled();
}

void OpenAtlasFileIntoEditorPainter(void* null)
{
	CreateOpenAtlasMapWindow();
}

void CreateNewArea(void* null)
{
    if( BuilderGlobals::NewAreaVars::name.empty()
    ||  BuilderGlobals::NewAreaVars::grid_size_x <= 0
    ||  BuilderGlobals::NewAreaVars::grid_size_y <= 0 )
        return;

    NewAreaData& data = (*(NewAreaData*)new_area_data);

    if( data.name.empty()
    ||  data.grid_size_x <= 0
    ||  data.grid_size_y <= 0 )
        return;

    //use the stored data to create a new empty custom area
	External::system->AddCommand( new CreateArea( BuilderGlobals::NewAreaVars::name
                                            , BuilderGlobals::NewAreaVars::grid_size_x
                                            , BuilderGlobals::NewAreaVars::grid_size_y ) );
}

void OpenCreateNewAreaWindow(void* null)
{
	CreateNewAreaWindow();
}

//End button callbacks
///////////////////////


//////////////////////////////////////////////////////////////////////
//File callbacks

//This callback will take the file path and name and create
//a window with the text content of the file.
void OpenTextFile(const std::string& path, const std::string& filename)
{
	const std::string EDITOR_OPENED_TEXT_FILE_NAME = "Editor_Opened_Text_File";
	if( !External::gui->FindRootNode(EDITOR_OPENED_TEXT_FILE_NAME) )
	{
		std::string file_text = "";
		std::string temp;
		std::fstream t_file;
		t_file.open( (path + "\\" + filename).c_str(), std::ios::in | std::ios::binary);
		while( t_file.good() )
		{
			 char c;
			t_file.read( &c, sizeof( char) );
				file_text.push_back(c);
		}

		t_file.close();
		if( file_text.size() > 0 )
		{
			GUINode* root = GUI::Widget::MakeTextBoxWindow(vec2(10, 10), vec2(500,500), EDITOR_OPENED_TEXT_FILE_NAME, filename, file_text);
			GUI::Window::SetWindowColor( root, vec3(0.25f,0.4f,0.2f) );
			_gui->AddNode( root );
			_gui->SetInputFocus( root );

		}
		else
		{
			std::cerr<<"Error: File "<<filename <<" not found.\n";
		}
	}

}

//This callback will take the file path and name, load the atlas into
//the renderer, and then set the current area to use the atlas.
//Note that atlas files are specific atlas .map files, not graphical textures
void OpenAtlasMap(const std::string& path, const std::string& filename)
{
	std::string extension = Global::GetExtension( filename );
	std::string file_to_open = path + Global::FILESLASH + filename;
	std::cerr<<"extension: "<<extension <<"\n";
	if( extension != "map" )
	{
		std::cerr<<"Cannot open " <<filename <<" \nNot a valid atlas .map file.\n";
		return;
	}

	//will return null if we're loaded into an area with no grid
	GridInfo* grid = External::editor->GetCurrentAreaGrid();
	if( grid )
		grid->SetAtlas( External::renderer->LoadAtlas( file_to_open ) );

	_editor->ForceSelectPaintingType(0);
}

//End file callbacks
///////////////////////

}//End Callbaks (namespace)

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// Call each frame to check for builder hotkeys
//////////////////////////////////////////////////////////////////

void Hotkey_Check()
{
	if( Global::Keyboard::KeyPressed('`') ) CreateMainWindow();
	if( Global::Keyboard::KeyPressed('!') ) CreateOpenTextFileWindow();
	if( Global::Keyboard::KeyPressed('@') ) CreatePaintingEditorWindow();
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// Init function, should be called before anything else in here
//////////////////////////////////////////////////////////////////
void InitBuilder(Editor* editor)
{
	_editor = External::editor;
	_gui = External::gui;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// Functions to create the GUI elements
//////////////////////////////////////////////////////////////////

//Create the main launcher window
void CreateMainWindow()
{
    GUINode* GeneralSettings = _gui->FindRootNode("Editor_General") ;
	GUINode* GS_Window = 0;

	vec3 editor_window_color = vec3(0,0,0.5f);
	vec2 editor_window_size  = vec2(250,180);

	//don't create the window if it already exists
	if( GeneralSettings )
		return;

	//create the editor window
	GeneralSettings = GUI::Window::MakeWindow(0, vec2(450,10), editor_window_size, "Editor_General", "Editor General Settings");
    GS_Window = GUI::Window::GetInnerWindow( GeneralSettings ); //GeneralSettings->GetNode("Editor_General/window");

	//set the color
	GUI::Window::SetWindowColor( GeneralSettings, editor_window_color );

    //MakeLabel( GS_Window, vec2(10,20), "disabled_label", "Disabled: " ); //TODO: make these buttons using the new editor GUI
    GUI::Button::MakeButton( GS_Window, Callbacks::ToggleDisabled, 0, vec2(10,10), vec2(80,17), "disabled_button", "Disabled: " );
	GUI::Field::MakeField<truth>( GS_Window, &_editor->disabled, vec2(105,10), 50, "disabled");

    //MakeLabel( GS_Window, vec2(10,40), "fill_mode_label", "Fill Mode: " );
    //GUI::Button::MakeButton( GS_Window, Callbacks::ToggleFillMode, 0, vec2(10,30), vec2(80,17), "fill_mode_button", "Fill Mode: " );
    //GUI::Field::MakeField<truth>( GS_Window, &_editor->fill_mode, vec2(105,30), 50, "fill_mode");
    _gui->AddNode( GeneralSettings );

	//create buttons to activate each each mode
    vec2 mode_button_pos = vec2(10,60);
    GUI::Button::MakeButton( GS_Window, Callbacks::TogglePaintingMode, 0, mode_button_pos + vec2(0,0), vec2(100,20), "mode_button_paint", "PaintMode");
    GUI::Button::MakeButton( GS_Window, Callbacks::ToggleLightsMode, 0, mode_button_pos + vec2(0,20), vec2(100,20), "mode_button_lights", "LightsMode");
    GUI::Button::MakeButton( GS_Window, Callbacks::ToggleObjectsMode, 0, mode_button_pos + vec2(120,0), vec2(100,20), "mode_button_objects", "ObjectsMode");
    GUI::Button::MakeButton( GS_Window, Callbacks::TogglePhysicsMode, 0, mode_button_pos + vec2(120,20), vec2(100,20), "mode_button_physics", "PhysicsMode");

    GUI::Button::MakeButton( GS_Window, Callbacks::OpenCreateNewAreaWindow, 0, vec2(10,130), vec2(100,20), "create_new_area", "New Area");

	_gui->SetInputFocus( GeneralSettings );
}

void CreatePaintingEditorWindow()
{
    GUINode* PaintingRoot = _gui->FindRootNode("Editor_Painting") ;
	GUINode* PR_Window = 0;

	vec3 window_color = vec3(0,0,0.5f);
	vec2 window_size  = vec2(250,150);

	//don't create the window if it already exists
	if( PaintingRoot )
		return;

	//create the editor window
	PaintingRoot = GUI::Window::MakeWindow(0,
							    vec2(450,10),
								window_size,
								"Editor_Painting",
								"Editor: Tile Painter");
    PR_Window = GUI::Window::GetInnerWindow( PaintingRoot );

	//set the color
	GUI::Window::SetWindowColor( PaintingRoot, window_color );

	//create button to load the atlas
    GUI::Button::MakeButton( PR_Window,
							 Callbacks::OpenAtlasFileIntoEditorPainter,
							 0,
							 vec2(10,10),
							 vec2(180,17),
							 "load_atlas_into_editor_painter",
							 "Set TilePainter Atlas" );

    _gui->AddNode( PaintingRoot );

	if( _editor->IsDisabled() )
		_editor->ToggleDisabled();

	//activate tile painting
	_gui->SetInputFocus( PaintingRoot );
}

void CreatePhysicsEditorWindow()
{
}

void CreateLightsEditorWindow()
{
}

void CreateObjectsEditorWindow()
{
}


void CreateOpenTextFileWindow()
{
    GUINode* FileExplorer = _gui->FindRootNode("Editor_FileExplorer") ;

	vec3 window_color = vec3(0,0,0.5f);
	vec2 window_size  = vec2(250,350);

	//don't create the window if it already exists
	if( FileExplorer )
		return;

	FileExplorer = GUI::Widget::MakeExplorerWindow(vec2(10, 10), window_size, "Editor_FileExplorer", Callbacks::OpenTextFile);

	GUI::Window::SetWindowColor( FileExplorer, window_color );

    _gui->AddNode( FileExplorer );
}

void CreateOpenAtlasMapWindow()
{
    GUINode* AtlasFileExplorer = _gui->FindRootNode("Editor_AtlasFileExplorer") ;

	vec3 window_color = vec3(0,0,0.5f);
	vec2 window_size  = vec2(250,350);

	//don't create the window if it already exists
	if( AtlasFileExplorer )
		return;

	AtlasFileExplorer = GUI::Widget::MakeExplorerWindow(vec2(10, 10), window_size, "Editor_AtlasFileExplorer", Callbacks::OpenAtlasMap);

	GUI::Window::SetWindowColor( AtlasFileExplorer, window_color );

    _gui->AddNode( AtlasFileExplorer );
}

//Create a popup window that creates a new grid when the button is pressed
void CreateNewAreaWindow()
{
    GUINode* CreateNewAreaWindow = _gui->FindRootNode("Editor_CreateNewArea") ;

	vec3 window_color = vec3(0,0,0.5f);
	vec2 window_size  = vec2(250,150);

	//don't create the window if it already exists
	if( CreateNewAreaWindow )
		return;

    //only this window uses this data
    //static NewAreaData new_area_data;
    //re-init the data
    //new_area_data = NewAreaData();

	CreateNewAreaWindow = GUI::Window::MakeWindow(0,
							    vec2(10,10),
								window_size,
								"Editor_CreateNewArea",
								"Create New Area");
	GUI::Window::SetWindowColor( CreateNewAreaWindow, window_color );

	//Use this pointer as the parent for items added to the window
    GUINode* CNAW_Window = GUI::Window::GetInnerWindow( CreateNewAreaWindow );

    vec2 NamePosAnchor = vec2(10,20);
    vec2 GridPosAnchor = vec2(10,40);

    GUINode* NameLabel = MakeLabel( CNAW_Window, NamePosAnchor, "name_label", "Name: " );
    //field to enter the name
    GUINode* Name_Field = GUI::Field::MakeField<std::string>( CNAW_Window,
                                        &BuilderGlobals::NewAreaVars::name,
                                        vec2(0,0),//set the position w/ a function below
                                        100,
                                        "new_area_name");

    GUINode* GridLabel = MakeLabel( CNAW_Window, GridPosAnchor, "grid_size_label", "GridSize: " );

    //field for the new grid size (x)
    GUINode* X_Field = GUI::Field::MakeField<unsigned>(    CNAW_Window,
                                        &BuilderGlobals::NewAreaVars::grid_size_x,
                                        vec2(0,0),//set the position w/ a function below
                                        42,
                                        "new_grid_size_x");


    //field for the new grid size (y)
    GUINode* Y_Field = GUI::Field::MakeField<unsigned>(    CNAW_Window,
                                        &BuilderGlobals::NewAreaVars::grid_size_y,
                                        vec2(0,0),//set the position w/ a function below
                                        42,
                                        "new_grid_size_y");

    Name_Field->SetPos( vec2(NamePosAnchor.x+NameLabel->GetSize().x,NamePosAnchor.y) );
    X_Field->SetPos( vec2(GridPosAnchor.x+GridLabel->GetSize().x,GridPosAnchor.y) );
    Y_Field->SetPos( vec2(GridPosAnchor.x+GridLabel->GetSize().x+54,GridPosAnchor.y) );


    MakeLabel( CNAW_Window, vec2(10,70), "create_warning_label1", "Warning: This will DELETE" );
    MakeLabel( CNAW_Window, vec2(10,83), "create_warning_label2", "         the current area!" );
    GUINode* CreateAreaButton = GUI::Button::MakeButton( CNAW_Window,
                                                         Callbacks::CreateNewArea,
                                                         0,
                                                         vec2(10,106),
                                                         vec2(150,20),
                                                         "create_new_area", "Create New Area");

    GUI::Button::SetClickAction( CreateAreaButton, GUI::Action::CloseTree );


    _gui->AddNode( CreateNewAreaWindow );
	_gui->SetInputFocus( CreateNewAreaWindow );
}


/////////////////////////////////////////////////////
//This function is used to see which of our editor windows is active
//It looks for a window with the specific name and sees if it has focus.
//This current design assumes we will have one main window per editor mode.
int GetActiveWindowModeType()
{
    GUINode* PaintingRoot = _gui->FindRootNode("Editor_Painting") ;
    GUINode* PhysicsRoot  = _gui->FindRootNode("Editor_Physics") ;
    GUINode* LightsRoot   = _gui->FindRootNode("Editor_Lights") ;
    GUINode* ObjectsRoot  = _gui->FindRootNode("Editor_Objects") ;

	if( PaintingRoot && _gui->HasFocus( PaintingRoot ) )
	{
		return EditorMode::PAINT;
	}
	if( PhysicsRoot && _gui->HasFocus( PhysicsRoot ) )
	{
		return EditorMode::PHYSICS;
	}
	if( LightsRoot && _gui->HasFocus( LightsRoot ) )
	{
		return EditorMode::LIGHTS;
	}
	if( ObjectsRoot && _gui->HasFocus( ObjectsRoot ) )
	{
		return EditorMode::OBJECTS;
	}

	//don't change state!
	return -1;
}

}//End EditorGUIBuilder
}//End GUI
