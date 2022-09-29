#ifndef GUIBUILDER_H
#define GUIBUILDER_H

#include <globals.h>


class GUINode;

class Command;

namespace GUI
{



    //////////////////////////////////////
    // Generic GUI functions


    //simple text label functions
    GUINode* MakeLabel(GUINode* parent, const vec2& position, const std::string& name, const std::string& text);



    //////////////////////////////////////
    // Actions (functions meant for binding to buttons
    namespace Action
    {
        void CloseTree(GUINode* action_node);
        void Close(GUINode* action_node);
        void Hide(GUINode* action_node);
        void HideTree(GUINode* action_node);
    }

    //////////////////////////////////////
    // Window functions
    namespace Window
    {

    //This is the "standard" window, it creates a window and places a window and a title bar inside the window.
    //Since you typically want to place things inside the inner window (not overlapping the title bar)
    //if you use this function you will want to call GetInnerWindow to retrieve the node from inside this window
    //then parent any further GUI objects to the inner window.
	GUINode* MakeWindow(GUINode* parent, const vec2& position, const vec2& size, const std::string& name, const std::string& title = "", const std::string& bg_image_file = "");
    GUINode* GetInnerWindow( GUINode* window_base );

    void SetWindowColor(GUINode* window, vec3 rgb);
	void SetBackgroundTexture(GUINode* window, const std::string& texture);

    }

    //////////////////////////////////////
    // Button functions
    namespace Button
    {

    //Simple button functions
    GUINode* MakeButton(GUINode* parent, Command* click_action, const vec2& position, const vec2& size, const std::string& name, const std::string& text = "");
    GUINode* MakeButton(GUINode* parent, void (*func)(void*), void* data, const vec2& position, const vec2& size, const std::string& name, const std::string& text = "");

    void SetCommand( GUINode* button, Command* command );
    void SetClickAction( GUINode* button, void (*new_click_action)(GUINode*) );
    void SetClickFunction( GUINode* button, void (*new_click_function)(void*), void* data );

    }

    //////////////////////////////////////
    // List functions
    namespace List
    {

    GUINode* MakeList(GUINode* parent, const vec2& position, const std::string& name);

    void AddItemToList(GUINode* list_root, GUINode* item);
    void SetSpacing(GUINode* list_root, float spacing);

    }


    //////////////////////////////////////
    // Graphic functions
    namespace Graphic
    {

    GUINode* MakeGraphic(GUINode* parent, const vec2& position, const vec2& size, const std::string& name, const std::string& filename);

    void SetTexture(GUINode* graphic_node, const std::string& filename);

    }

    //////////////////////////////////////
    // TextBox functions
    namespace TextBox
    {

    GUINode* MakeTextBox(GUINode* parent, const vec2& position, const vec2& size, const std::string& name, const std::string& text);

	void SetText(GUINode* textbox_node, const std::string& text);

	}

	
    //////////////////////////////////////
    // DirectoryBrowser functions functions
    namespace DirectoryBrowser
    {

    GUINode* MakeDirectoryBrowser(GUINode* parent, const vec2& position, const vec2& size, const std::string& name);

    void SetSelectionCallback( GUINode* directory_browser, void (*new_select_action)(const std::string& path, const std::string& filename) );
	
	}

	////////////////////////////////////////
	// Widget functions
	// Widgets are created by compositing other GUI types
	namespace Widget
	{

	//Create a window that displays some text with a close X in the corner
	GUINode* MakeTextBoxWindow(const vec2& position, const vec2& size, const std::string& name, const std::string& titlebar, const std::string& text);

	//Create a window that allows you to browse to a file and run the select action on it. This window then closes.
	GUINode* MakeExplorerWindow(const vec2& position, const vec2& size, const std::string& name, void (*select_action)(const std::string& path, const std::string& filename) );

	}
}

#endif
