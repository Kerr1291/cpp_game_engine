#include <GUIBuilder.h>

#include <GUIText.h>
#include <GUIWindow.h>
#include <GUIButton.h>
#include <GUIList.h>
#include <GUIGraphic.h>
#include <GUITextBox.h>
#include <GUIDirectoryBrowser.h>

#include <GUIManager.h>

#include <Command.h>
#include <DoNothing.h>

//macro used in GUIBuilder.cpp to make code a bit more readable
#define DCAST( T, var ) \
  dynamic_cast<T>( var )

namespace GUI
{



GUINode* MakeLabel(GUINode* parent, const vec2& position, const std::string& name, const std::string& text)
{
    if( text.empty() || (parent && parent->IsComponent()) )
        return 0;

    const float min_size_x = text.size()*8 + 4;
    const float min_size_y = 17;
    vec2 final_size = vec2(min_size_x,min_size_y);

	GUIText* text_object = new GUIText(name);
	text_object->SetLocalTransform( position, final_size );
	text_object->text = text;

	if(parent)
        parent->AttachMember( 0, text_object );

	return text_object;
}



namespace Action
{
    void CloseTree(GUINode* action_node)
    {
        action_node->DeleteThisTree();
    }
    void Close(GUINode* action_node)
    {
        External::gui->DeleteNode( action_node->GetParent() );
    }
    void Hide(GUINode* action_node)
    {
        action_node->SetHidden(1);
    }
    void HideTree(GUINode* action_node)
    {
        GUINode* n = action_node->GetRoot();
        if( !n )
            n = action_node;

        n->SetHidden(1);
    }
}

namespace Window
{

GUINode* MakeWindow(GUINode* parent, const vec2& position, const vec2& size, const std::string& name, const std::string& title, const std::string& bg_image_file)
{
	const std::string titlebar_name = "titlebar";
	const std::string title_name = "title";
	const std::string window_name = "window";
	const std::string close_button_name = "close_button";
	const std::string background_graphic_name = "background";

	GUIWindow* base = new GUIWindow(name);

	base->SetLocalTransform( position, size );
	base->bg_color = vec3(0,0,0);
	base->edge_color = vec3(0.7f, 0.7f, 0.7f);
	base->SetAlpha(0);

	GUIText* t = 0;
	if( title.size() > 0 )
	{
		t = new GUIText(title_name);
		t->text = title;
		t->SetLocalTransform( vec2(2, 2), vec2( title.size() * 8, 13 ) );
		base->AttachMember(1,t);
	}

	GUIWindow* titlebar = new GUIWindow(titlebar_name);
	titlebar->SetLocalTransform( vec2(0,0), vec2( size.x, 17 ) );
	titlebar->bg_color = vec3(0.6f, 0.6f, 0.6f);
	titlebar->edge_color = vec3(0.7f, 0.7f, 0.7f);
	base->AttachMember(0,titlebar);


	GUIButton* close_button = new GUIButton(close_button_name);
	close_button->SetLocalTransform( vec2(size.x - 17,2), vec2( 15, 15 ) );
	close_button->bg_color = vec3(0.35f, 0.35f, 0.35f);
	close_button->edge_color = vec3(0.7f, 0.7f, 0.7f);
	close_button->text = "X";
	close_button->text_pos.x = 0.25f;
	close_button->SetClickAction( GUI::Action::Close );
    base->AttachMember(2,close_button);


	GUIWindow* inner_window = new GUIWindow(window_name);
	inner_window->SetLocalTransform( vec2(0,17), vec2( size.x, size.y-17 ) );
	inner_window->bg_color = vec3(0.5f, 0.5f, 0.5f);
	inner_window->edge_color = vec3(0.7f, 0.7f, 0.7f);
	base->AttachChild(0,inner_window);

	if( parent )
		parent->AttachChild( 0, base );

	GUINode* bg_graphic = Graphic::MakeGraphic(0,vec2(0,0),vec2( size.x, size.y-17 ),background_graphic_name,bg_image_file);
	inner_window->AttachMember(0,bg_graphic);

	return base;
}

GUINode* GetInnerWindow( GUINode* window_base )
{
    GUINode* window = window_base->GetNode(window_base->GetFullName() + "/window");
    return window;
}

void SetWindowColor(GUINode* window, vec3 rgb)
{
    GUIWindow* window_node = DCAST( GUIWindow*, window );
    if( !window_node )
        return;

	vec3 window_color = rgb;
	vec3 titlebar_color = rgb * 1.5f;

	GUIWindow* inner_window = DCAST( GUIWindow*, window->GetNode( window->GetFullName() + "/window" ) );
	if( inner_window )
	{
		inner_window->bg_color = window_color;
	}

	GUIWindow* titlebar = DCAST( GUIWindow*, window->GetNode( window->GetFullName() + "/titlebar" ) );
	if( titlebar )
	{
		titlebar->bg_color = titlebar_color;
	}
}

void SetBackgroundTexture(GUINode* window, const std::string& texture)
{
    GUIWindow* window_node = DCAST( GUIWindow*, window );
    if( !window_node )
        return;

	GUINode* inner_window = GetInnerWindow( window );
	if( !inner_window )
		return;

	GUIGraphic* background = DCAST( GUIGraphic*, inner_window->GetNode( inner_window->GetFullName() + "/background" ) );
	if( background )
	{
		background->LoadTexture(texture);
	}
}

}//End Window



namespace Button
{

GUINode* MakeButton(GUINode* parent, Command* click_action, const vec2& position, const vec2& size, const std::string& name, const std::string& text)
{
    if( !click_action )
        return 0;

    if( parent && parent->IsComponent() )
        return 0;

	GUIButton* button = new GUIButton(name);

    button->SetLocalTransform(position, size);
	button->bg_color = vec3(0.35f, 0.35f, 0.35f);
	button->edge_color = vec3(0.7f, 0.7f, 0.7f);
	button->text = text;
	button->SetCommand( click_action );

    if( parent )
        parent->AttachChild( 0, button );

	return button;
}

GUINode* MakeButton(GUINode* parent, void (*func)(void*), void* data, const vec2& position, const vec2& size, const std::string& name, const std::string& text)
{
    GUIButton* button = DCAST( GUIButton*, MakeButton(parent, new DoNothing(), position, size, name, text) );
    if( button )
    {
        button->SetClickFunction(func, data);
    }
    return button;
}


void SetCommand( GUINode* button, Command* command )
{
    GUIButton* button_node = DCAST( GUIButton*, button );
    if( button_node )
        button_node->SetCommand( command );
}

void SetClickAction( GUINode* button, void (*new_click_action)(GUINode*) )
{
    GUIButton* button_node = DCAST( GUIButton*, button );
    if( button_node )
        button_node->SetClickAction( new_click_action );
}

void SetClickFunction( GUINode* button, void (*new_click_function)(void*), void* data )
{
    GUIButton* button_node = DCAST( GUIButton*, button );
    if( button_node )
        button_node->SetClickFunction( new_click_function, data );
}

}//End Button


namespace List
{


GUINode* MakeList(GUINode* parent, const vec2& position, const std::string& name)
{
	GUIWindow* list_base = new GUIWindow(name);

	list_base->SetLocalTransform( position, vec2(0,0) );
	list_base->bg_color = vec3(0,0,0);
	list_base->edge_color = vec3(1,1,1);
	list_base->SetAlpha(0);

	GUIList* list_object = new GUIList("__list__");
    list_base->AttachMember( 0, list_object );

	if( parent )
		parent->AttachChild( 0, list_base );

	return list_base;
}

void AddItemToList(GUINode* list_root, GUINode* item)
{
    GUIList* list_node = DCAST( GUIList*, list_root->FindNode("__list__") );
    if( !list_node )
        return;

    list_node->AddItem(item);
}

void SetSpacing(GUINode* list_root, float spacing)
{
    GUIList* list_node = DCAST( GUIList*, list_root->FindNode("__list__") );
    if( !list_node )
        return;

    list_node->SetSpacing(spacing);
}



}//End List



//////////////////////////////////////
// Graphic functions
namespace Graphic
{

GUINode* MakeGraphic(GUINode* parent, const vec2& position, const vec2& size, const std::string& name, const std::string& filename)
{
	GUIGraphic* graphic = new GUIGraphic(name);
	graphic->SetLocalTransform( position, size );
	graphic->LoadTexture(filename);

	if( parent )
		parent->AttachChild( 0, graphic );

    return graphic;
}

void SetTexture(GUINode* graphic_node, const std::string& filename)
{
    GUIGraphic* graphic = DCAST( GUIGraphic*, graphic_node );
    if( !graphic )
        return;

	graphic->LoadTexture(filename);
}


}//End Graphic






//////////////////////////////////////
// TextBox functions
namespace TextBox
{
	

GUINode* MakeTextBox(GUINode* parent, const vec2& position, const vec2& size, const std::string& name, const std::string& text)
{
	GUITextBox* textbox = new GUITextBox(name);
	textbox->SetLocalTransform( position, size );
	textbox->SetText(text);

	if( parent )
		parent->AttachChild( 0, textbox );

    return textbox;
}

void SetText(GUINode* textbox_node, const std::string& text)
{
    GUITextBox* textbox = DCAST( GUITextBox*, textbox_node );
    if( !textbox )
        return;

	textbox->SetText(text);
}

	
}//End TextBox


//////////////////////////////////////
// DirectoryBrowser functions
namespace DirectoryBrowser
{



GUINode* MakeDirectoryBrowser(GUINode* parent, const vec2& position, const vec2& size, const std::string& name)
{
	GUIDirectoryBrowser* directory_browser = new GUIDirectoryBrowser(name);
	directory_browser->SetLocalTransform( position, size );

	if( parent )
		parent->AttachChild( 0, directory_browser );

    return directory_browser;
}

void SetSelectionCallback( GUINode* directory_browser_node, void (*new_select_action)(const std::string& path, const std::string& filename) )
{
    GUIDirectoryBrowser* directory_browser = DCAST( GUIDirectoryBrowser*, directory_browser_node );
    if( !directory_browser )
        return;

	directory_browser->SetSelectionCallback( new_select_action );
	if( new_select_action )
		directory_browser->BeginBrowse();
}



}//End DirectoryBrowser

////////////////////////////////////////
// Widget functions
// Widgets are created by compositing other GUI types
namespace Widget
{

GUINode* MakeTextBoxWindow(const vec2& position, const vec2& size, 
						   const std::string& name, const std::string& titlebar, 
						   const std::string& text)
{
	GUINode* root = GUI::Window::MakeWindow(0
		, position
		, size
		, name
		, titlebar);

	GUI::Window::SetWindowColor( root, vec3(0.2f,0.2f,0.2f) );
	GUINode* window = GUI::Window::GetInnerWindow( root );
	GUINode* textbox = GUI::TextBox::MakeTextBox( window, vec2(0, 0),size, "TextBox", text );
	return root;
}

GUINode* MakeExplorerWindow(const vec2& position, const vec2& size, 
							const std::string& name, 
							void (*select_action)(const std::string& path, const std::string& filename) )
{
	GUINode* root = GUI::Window::MakeWindow(0
		, position
		, size
		, name
		, "Browse or make a selection");

	GUI::Window::SetWindowColor( root, vec3(0.2f,0.2f,0.2f) );
    GUINode* window = GUI::Window::GetInnerWindow( root );
	GUINode* db = GUI::DirectoryBrowser::MakeDirectoryBrowser(window,vec2(0,0),size,"DirectoryBrowser");
	GUI::DirectoryBrowser::SetSelectionCallback( db, select_action );
	dynamic_cast< GUIDirectoryBrowser* >( db )->SetCloseAfterSelection(1);
	return root;
}


}//End Widget

}//End GUI
