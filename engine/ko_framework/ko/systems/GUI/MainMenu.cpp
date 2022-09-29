#include <MainMenu.h>

#include <LoadArea.h>
#include <DoNothing.h>


#include <GUIBuilder.h>

#include <GUIWindow.h>
#include <GUIManager.h>
#include <GUIList.h>
#include <GUIButton.h>
#include <GUIGraphic.h>
#include <GUIField.h>
#include <GUIDirectoryBrowser.h>

#include <Editor_GUIBuilder.h>

#include <fstream>

MainMenu::MainMenu()
//:activeScreen(0)
{
}

MainMenu::~MainMenu()
{
}

void TestCallback(const std::string& path, const std::string& filename)
{
	std::cerr<<"I was passed "<<filename <<"\n";
	if( !External::gui->FindRootNode("opened_file") )//&& filename.find(".ini") != std::string::npos )
	{
		GUINode* root = GUI::Window::MakeWindow(0, vec2(10, 10), vec2(500,500), "opened_file", filename);
		GUI::Window::SetWindowColor( root, vec3(0.25f,0.4f,0.2f) );

        External::gui->AddNode( root );
		External::gui->SetInputFocus( root );

        GUINode* window = GUI::Window::GetInnerWindow( root );

		std::string test_text = "";
		std::string temp;
		std::fstream t_file;
		t_file.open( (path + "\\" + filename).c_str(), std::ios::in | std::ios::binary);
		while( t_file.good() )
		{
			 char c;
			t_file.read( &c, sizeof( char) );
				test_text.push_back(c);
		}

		t_file.close();
		if( test_text.size() > 0 )
			GUINode* textbox = GUI::TextBox::MakeTextBox( window, vec2(10, 10), vec2(500,400), "textbox", test_text );
	}
}

void MainMenu::Load()
{
        GUIManager* gui = External::gui;

        /*
		GUINode* root = GUI::Window::MakeWindow(0, vec2(0, 0), vec2(800,600), "MainMenu", "Main Menu");
		GUI::Window::SetWindowColor( dynamic_cast<GUIWindow*>(root), vec3(0.1f,0.1f,0.1f) );
        gui->AddNode( root );

        GUINode* custom_area = GUI::Button::MakeButton(root, new LoadArea<CustomArea>(), vec2(300,250), vec2(150,40), "CustomAreaButton", "Custom Area");
        GUINode* empty_area = GUI::Button::MakeButton(root, new LoadArea<EmptyArea>(), vec2(300,300), vec2(150,40), "EmptyAreaButton", "Empty Area");
        //GUINode* test_area = GUI::Button::MakeButton(root, new LoadArea<Beta>(), vec2(300,350), vec2(150,40), "TestArea", "Test Area");

        GUI::Button::SetClickAction( custom_area, GUI::Action::CloseTree );
        GUI::Button::SetClickAction( empty_area, GUI::Action::CloseTree );
        //GUI::Button::SetClickAction( test_area, GUI::Action::CloseTree );


        gui->AddNode( custom_area );
        gui->AddNode( empty_area );
        //gui->AddNode( test_area );
        */

		//GUI::EditorGUIBuilder::CreateOpenTextFileWindow();

		/*//the code below was moved into CreateOpenTextFileWindow()
		GUINode* root = GUI::Window::MakeWindow(0, vec2(10, 10), vec2(500,500), "test", "This is a test!");
		GUI::Window::SetWindowColor( root, vec3(0.5f,0,0) );

		GUINode* root = MakeExplorerWindow(vec2(10, 10), vec2(500,500), "test", TestCallback);
		
        gui->AddNode( root );

        GUINode* window = GUI::Window::GetInnerWindow( root ); //root->GetNode("test/window");


		GUINode* dbtest = GUI::DirectoryBrowser::MakeDirectoryBrowser(window,vec2(0,0),vec2(150,150),"dbtest");
		GUI::DirectoryBrowser::SetSelectionCallback( dbtest, TestCallback );
		*/
		
        //GUIDirectoryBrowser* db = new GUIDirectoryBrowser("dbtest");
        //db->SetLocalTransform( vec2(0,0), vec2(150,150) );
        //db->BeginBrowse();
        //db->SetSelectionCallback( TestCallback );
        //window->AttachChild( 0, db );

        /*
		std::string test_text = "";
		std::string temp;
		std::fstream t_file;
		t_file.open("./config.ini", std::ios::in | std::ios::binary);
		while( t_file.good() )
		{
			 char c;
			t_file.read( &c, sizeof( char) );
				test_text.push_back(c);
		}

		GUINode* textbox = GUI::TextBox::MakeTextBox( window, vec2(10, 10), vec2(500,400), "textbox", test_text );
        */

		/*

        static int tf1 = 0;
        static std::string foo = "test";

		GUINode* w1 = GUI::Window::MakeWindow(window, vec2(10, 10), vec2(150,180), "w1", "List 1");
		GUI::Window::SetWindowColor( w1, vec3(0,0.5f,0) );
        GUINode* w1_window = GUI::Window::GetInnerWindow( w1 );// w1->FindNode("w1/window");

        GUINode* t = GUI::List::MakeList( w1_window, vec2(10,10), "test_list" );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "a001") );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<std::string>( 0, &foo, vec2(0,0), 80, "b001") );
        GUI::List::AddItemToList( t, GUI::Button::MakeButton(0, new DoNothing(), vec2(0,0), vec2(50,50), "c001", "test") );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "d001") );
        GUI::List::AddItemToList( t, GUI::Button::MakeButton(0, new DoNothing(), vec2(0,0), vec2(50,50), "e001", "test") );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "f001") );
        GUI::List::SetSpacing( t, 25.0f );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "f002") );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "f003") );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "f004") );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "f005") );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "f006") );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "f007") );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "f008") );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "f009") );
        GUI::List::AddItemToList( t, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "f0010") );
        GUI::List::AddItemToList( t, GUI::Button::MakeButton(0, new DoNothing(), vec2(0,0), vec2(50,50), "end", "end") );

        GUI::List::AddItemToList( t, GUI::MakeLabel(0,vec2(0,0),"label_0", "This") );
        GUI::List::AddItemToList( t, GUI::MakeLabel(0,vec2(0,0),"label_1", "is") );
        GUI::List::AddItemToList( t, GUI::MakeLabel(0,vec2(0,0),"label_2", "a") );
        GUI::List::AddItemToList( t, GUI::MakeLabel(0,vec2(0,0),"label_3", "test") );
        GUI::List::AddItemToList( t, GUI::MakeLabel(0,vec2(0,0),"label_4", "of") );
        GUI::List::AddItemToList( t, GUI::MakeLabel(0,vec2(0,0),"label_5", "the") );
        GUI::List::AddItemToList( t, GUI::MakeLabel(0,vec2(0,0),"label_6", "list") );
        GUI::List::AddItemToList( t, GUI::MakeLabel(0,vec2(0,0),"label_7", "system.") );
        GUI::List::SetSpacing( t, 2.0f );

		//GUI::Window::SetBackgroundTexture(w1, "trees.png");

		GUINode* w2 = GUI::Window::MakeWindow(window, vec2(210, 10), vec2(150,350), "w2", "List 2");
		GUI::Window::SetWindowColor( w2, vec3(0,0,0.5f) );
        GUINode* w2_window = GUI::Window::GetInnerWindow( w2 );//w2->FindNode("w2/window");

        GUINode* t2 = GUI::List::MakeList( w2_window, vec2(10,10), "test_list" );
        GUI::List::AddItemToList( t2, GUI::Field::MakeField<std::string>( 0, &foo, vec2(0,0), 80, "a001") );
        GUI::List::AddItemToList( t2, GUI::Field::MakeField<std::string>( 0, &foo, vec2(0,0), 80, "b001") );
        GUI::List::AddItemToList( t2, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "d001") );
        GUI::List::AddItemToList( t2, GUI::Field::MakeField<int>( 0, &tf1, vec2(0,0), 80, "f001") );
        GUI::List::SetSpacing( t2, 25.0f );

		*/

		//GUI::List::AddItemToList( t2, GUI::Graphic::MakeGraphic(0,vec2(0,0),vec2(80,55),"graphic","testTexture.png") );
		//GUI::List::AddItemToList( t2, GUI::Graphic::MakeGraphic(0,vec2(0,0),vec2(80,55),"graphic1","testTexture.png") );
		//GUI::List::AddItemToList( t2, GUI::Graphic::MakeGraphic(0,vec2(0,0),vec2(80,55),"graphic2","testTexture.png") );

}

void MainMenu::Update()
{
    /*
        std::list<MenuButton>::iterator iter = screens[activeScreen].begin();
        for( ; iter != screens[activeScreen].end(); ++iter )
        {
                iter->Update();
        }
        */
}

void MainMenu::Draw()
{
    /*
        std::list<MenuButton>::iterator iter = screens[activeScreen].begin();
        for( ; iter != screens[activeScreen].end(); ++iter )
        {
                iter->DrawObject();
        }
        */
}

