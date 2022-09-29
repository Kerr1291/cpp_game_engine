#ifndef GUIDIRECTORYBROWSER_H
#define GUIDIRECTORYBROWSER_H

#include <GUINode.h>

class GUIDirectoryBrowser: public GUINode
{
    private:

        std::string start_path;
        std::string working_path;
        std::string selected_item;

        float scrollbar_location; //TODO: implement scroll bar for this class (see if the behavior can be generalized for all nodes?)

        truth browsing;
        std::set<std::string> directories;
        std::set<std::string> files;

        void SelectUnderCursor(std::string& out);
        truth IsDirectory(const std::string& name);
        void BrowseToDirectory(const std::string& dirname );
        void DoSelectAction(const std::string& file);

        void (*select_action)(const std::string& path, const std::string& filename);
		truth should_close;
		
			
		//scrollbar functions
		//TODO: think about moving scrollbar functionality out of the textbox GUI object and into its own class, or just make part of GUINode....
		void UpdateScrollbarParams();
		truth MouseOverScrollbar();
		void RenderScrollbar();
		truth HasScrollbar();

		//scrollbar members
		vec2 scrollbar_points[4];
		truth scrolling_with_mouse;
		vec2 mouse_previous_pos;
		float scrollbar_step;

    public:

        GUIDirectoryBrowser(const std::string& name = "");
        virtual ~GUIDirectoryBrowser();

        void Render();
        truth MouseOver();
        void Update();

        void BeginBrowse();
        void GetCurrentDirectory(std::string& out);
        void SetSelectionCallback(void (*new_select_action)(const std::string& path, const std::string& filename));
		void SetCloseAfterSelection(truth should_close_state);
		
		float GetListSize();
        vec3 scrollbar_color;
		vec3 files_color;
		vec3 directories_color;
};

#endif
