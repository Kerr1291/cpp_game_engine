#include <GUIDirectoryBrowser.h>
#include <GUIBuilder.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else //#endif //TODO: change back to #endif after linux build option is fixed
//#ifdef LINUX_BUILD //TODO: uncomment this after LINUX_BUILD is fixed
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#endif //end of #if #else for testing _WIN32
#ifdef MAC_BUILD
#endif



/////////////////////
// Helper functions

//TODO: move the local functions here (GetDirectoryContent and GetCWD) to a global helper function space
void GetDirectoryContent(std::set<std::string>& out_directories, std::set<std::string>& out_files, const std::string& in_dir)
{
    out_directories.clear();
    out_files.clear();

#ifdef _WIN32
    //Note: this code taken from a stackoverflow answer, slightly modified for use here
    HANDLE dir;
    WIN32_FIND_DATA file_data;

    if ((dir = FindFirstFile((in_dir + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return; /* No files found */

    do {
		const std::string file_name = file_data.cFileName;
        const std::string full_file_name = file_name;
        const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        if (is_directory)
        {
            out_directories.insert(full_file_name);
        }
        else
        {
            out_files.insert(full_file_name);
        }
    } while (FindNextFile(dir, &file_data));

    FindClose(dir);
#else //#endif //TODO: change back to #endif after linux build option is fixed
//#ifdef LINUX_BUILD //TODO: uncomment this after LINUX_BUILD is fixed
    DIR* dirstream = opendir( in_dir.c_str() );
    if( dirstream == 0 )
    {
        std::cerr<<"Failed to open directory " <<in_dir <<"\n";
        return;
    }

    while( dirstream )
    {
        dirent* current_entry = 0;
        current_entry = readdir( dirstream );
        if( current_entry )
        {
            std::string dir_name( current_entry->d_name );

            //check if this ia directory or file
            struct stat st;
            lstat(dir_name.c_str(), &st);
            if(S_ISDIR(st.st_mode))
                out_directories.insert( dir_name );
            else
                out_files.insert( dir_name );
        }
        else
        {
            break;
        }
    }

    closedir( dirstream );
    //#endif
#endif//end of #if #else for testing _WIN32
#ifdef MAC_BUILD
    //TODO: write mac file loading code
#endif
}

void GetCWD(std::string& out)
{
    char cwd[1024];
#ifdef _WIN32
    GetCurrentDirectory( sizeof(cwd), cwd );
#else//#endif //TODO: change back to #endif after linux build option is fixed
//#ifdef LINUX_BUILD //TODO: uncomment this after LINUX_BUILD is fixed
    getcwd(cwd,sizeof(cwd));
#endif //end of #if #else for testing _WIN32
#ifdef MAC_BUILD
    //TODO: write mac code to get the current working directory
#endif
    out = std::string(cwd);
    std::cerr<<"cwd is "<<out <<"\n";
}


/////////////////////////////////////////////////////////////
// Private methods
//

void GUIDirectoryBrowser::SelectUnderCursor(std::string& out)
{
    vec2 mouse = Global::Mouse::ScreenPos;
    if( mouse.y < screen_pos.y )
        return;
    if( mouse.y > screen_pos.y + GetSize().y )
        return;

    float location = mouse.y - screen_pos.y + (scrollbar_location * GetListSize());
    int index = location / 13;

    std::set<std::string>::iterator iter;
    if( static_cast<unsigned>(index) < directories.size() )
    {
        iter = directories.begin();
        std::advance( iter, index );
		out = *iter;
    }
	else
    {
        index -= directories.size();
		if( static_cast<unsigned>(index) < files.size() )
		{
			iter = files.begin();
			std::advance( iter, index );
			out = *iter;
		}
    }
}

truth GUIDirectoryBrowser::IsDirectory(const std::string& name)
{
    if( selected_item.empty() )
        return 0;

    if( directories.find(name) != directories.end() )
        return 1;
    return 0;
}

void GUIDirectoryBrowser::BrowseToDirectory(const std::string& dirname )
{
#ifdef _WIN32
    const std::string SLASH = "\\";
#else //#endif //TODO: change back to #endif after linux build option is fixed
//#ifdef LINUX_BUILD //TODO: uncomment this after LINUX_BUILD is fixed
    const std::string SLASH = "/";
#endif //end of #if #else for testing _WIN32
#ifdef MAC_BUILD
    const std::string SLASH = "/";
#endif
    //set the next directory
    std::string nextdir = working_path + SLASH + dirname;

    //change to it
    chdir(nextdir.c_str());

    //get the path of this location
    GetCWD(working_path);

    //get the content at this location
    GetDirectoryContent(directories, files, ".");

    //change the program's working directory back to its root
    chdir(start_path.c_str());

    //clear the selected item
    selected_item = "";
	scrollbar_location = 0;
}

void GUIDirectoryBrowser::DoSelectAction(const std::string& file)
{
    //for now, as a test, just print this
    std::cerr<<"file selected: " <<file <<"\n";
    std::cerr<<"path of selected file: "<<working_path <<"\n";
    if( select_action )
        select_action(working_path, file);
    selected_item = "";
	if( should_close )
		GUI::Action::CloseTree( this );
}


////////////////////////////////////////
// Scrollbar methods



void GUIDirectoryBrowser::UpdateScrollbarParams()
{
    if(!this->GetParent())
        return;

    GUINode* p = this->GetParent();

    float ratio =  p->GetSize().y / GetListSize();
    scrollbar_step = ratio * 0.1f;

    const float scrollbar_width = 10.0f;
    const float scrollbar_height = ratio * p->GetSize().y;
    vec2 scrollbar_anchor =
        p->GetScreenPos() +
        vec2( p->GetSize().x, scrollbar_location * p->GetSize().y - scrollbar_location * ratio * p->GetSize().y) - vec2(scrollbar_width,0);

    scrollbar_points[0] = vec2(0,0)                              + scrollbar_anchor;
    scrollbar_points[1] = vec2(0,              scrollbar_height) + scrollbar_anchor;
    scrollbar_points[2] = vec2(scrollbar_width,scrollbar_height) + scrollbar_anchor;
    scrollbar_points[3] = vec2(scrollbar_width,0)                + scrollbar_anchor;
}

truth GUIDirectoryBrowser::MouseOverScrollbar()
{
    vec2 mouse = Global::Mouse::ScreenPos;
    if( mouse.x < scrollbar_points[0].x )
        return 0;
    if( mouse.y < scrollbar_points[0].y )
        return 0;
    if( mouse.x > scrollbar_points[2].x )
        return 0;
    if( mouse.y > scrollbar_points[2].y )
        return 0;

    return 1;
}

void GUIDirectoryBrowser::RenderScrollbar()
{
    float color_scale = 1.0f;
    if( MouseOverScrollbar() )
    {
        color_scale = 1.5f;
    }

    glColor4f( color_scale * scrollbar_color.r, color_scale * scrollbar_color.g, color_scale * scrollbar_color.b, alpha );

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f( scrollbar_points[0].x, scrollbar_points[0].y );
        glVertex2f( scrollbar_points[1].x, scrollbar_points[1].y );
        glVertex2f( scrollbar_points[2].x, scrollbar_points[2].y );
        glVertex2f( scrollbar_points[3].x, scrollbar_points[3].y );
    glEnd();

    glColor4f( 1.0f, 1.0f, 1.0f, alpha );

    glBegin(GL_LINE_LOOP);
        glVertex2f( scrollbar_points[0].x, scrollbar_points[0].y );
        glVertex2f( scrollbar_points[1].x, scrollbar_points[1].y );
        glVertex2f( scrollbar_points[2].x, scrollbar_points[2].y );
        glVertex2f( scrollbar_points[3].x, scrollbar_points[3].y );
    glEnd();
    glDisable(GL_BLEND);
}

truth GUIDirectoryBrowser::HasScrollbar()
{
    if(!this->GetParent())
        return 0;

    GUINode* p = this->GetParent();

    float ratio = p->GetSize().y / GetListSize();
    if( ratio >= 1.0f )
        return 0;

    return 1;
}




/////////////////////////////////////////////////////////////
// Public methods
//

GUIDirectoryBrowser::GUIDirectoryBrowser(const std::string& name)
:GUINode(1, name)
,start_path("")
,working_path("")
,selected_item("")
,scrollbar_location(0)
,browsing(0)
,select_action(0)
,should_close(0)
,scrolling_with_mouse(0)
,scrollbar_step(1.0f)
,scrollbar_color( vec3( 0.7f, 0.7f, 0.7f ) )
,files_color( vec3(1,1,1) )
,directories_color( vec3(1,0.5,0.5) )
{
    UpdateScrollbarParams();
}

GUIDirectoryBrowser::~GUIDirectoryBrowser()
{
}

void GUIDirectoryBrowser::Render()
{
    if( hidden )
        return;

	UpdateScrollbarParams();

    //TODO: change this to use FTGL after it's more completely implemented

    //TODO: allow the directory font color to be set/changed (don't hard code here)
    float alpha = 1.0f;
	//glColor4f( color.r, color.g, color.b, alpha );
	//glRasterPos2i( screen_pos.x, screen_pos.y+13 );

	const float list_size = GetListSize();
    float render_pos = 0;
    unsigned i = 0;
	{//print list of directories
        std::set<std::string>::iterator iter = directories.begin();
        while( iter != directories.end() )
        {
            render_pos = (i*13)+(screen_pos.y+13) - (scrollbar_location * list_size);
            if( render_pos > screen_pos.y+GetSize().y )
            {
                break;
            }

			if( render_pos >= screen_pos.y )
			{
				if(*iter == selected_item)
					glColor4f( files_color.r*0.5f, files_color.g*0.5f, files_color.b*1.5f, alpha );
				else
					glColor4f( files_color.r, files_color.g, files_color.b, alpha );

				glRasterPos2i( screen_pos.x, render_pos );

				glutBitmapString( GLUT_BITMAP_8_BY_13, (unsigned char*)(*iter).c_str() );
			}
            ++iter;
            ++i;
        }
    }

	{//print list of files
        std::set<std::string>::iterator iter = files.begin();
        while( iter != files.end() )
        {
            render_pos = (i*13)+(screen_pos.y+13) - (scrollbar_location * list_size);
            if( render_pos > screen_pos.y+GetSize().y )
            {
                break;
            }

			if( render_pos >= screen_pos.y )
			{
				if(*iter == selected_item)
					glColor4f( directories_color.r*0.5f, directories_color.g*0.5f, directories_color.b*1.5f, alpha );
				else
					glColor4f( directories_color.r, directories_color.g, directories_color.b, alpha );

				glRasterPos2i( screen_pos.x, render_pos );

				glutBitmapString( GLUT_BITMAP_8_BY_13, (unsigned char*)(*iter).c_str() );
			}
            ++iter;
            ++i;
        }
    }

    if( HasScrollbar() )
        RenderScrollbar();
}

truth GUIDirectoryBrowser::MouseOver()
{
    vec2 mouse = Global::Mouse::ScreenPos;
    if( mouse.x < screen_pos.x )
        return 0;
    if( mouse.y < screen_pos.y )
        return 0;
    if( mouse.x > screen_pos.x + GetSize().x )
        return 0;
    if( mouse.y > screen_pos.y + GetSize().y )
        return 0;

    return 1;
}

void GUIDirectoryBrowser::Update()
{
	//updates for the scroll bar
	if( ( MouseOver() || this->GetParent() && this->GetParent()->MouseOver() ) && RootHasFocus() && glm::abs( Global::Mouse::WheelDelta ) > 0 )
    {
        scrollbar_location += Global::Mouse::WheelDelta * scrollbar_step;
        scrollbar_location = glm::clamp( scrollbar_location, 0.0f, 1.0f );
    }

    if( HasScrollbar() )
    {
        if( MouseOverScrollbar() && Global::Mouse::GetState( Button::LEFT_DOWN ) )
        {
            scrolling_with_mouse = 1;
            mouse_previous_pos = Global::Mouse::ScreenPos;
        }
        if( scrolling_with_mouse && Global::Mouse::GetState( Button::LEFT_HELD ) )
        {
            vec2 current_pos = Global::Mouse::ScreenPos;
            vec2 delta = current_pos - mouse_previous_pos;
            mouse_previous_pos = current_pos;

            float direction = 0;
            if( glm::abs(delta.y) > Global::TINY )
                direction = (delta.y < 0 ? -1 : 1);

            scrollbar_location += direction * scrollbar_step;
            scrollbar_location = glm::clamp( scrollbar_location, 0.0f, 1.0f );
        }
        if( scrolling_with_mouse && Global::Mouse::GetState( Button::LEFT_UP ) )
        {
            scrolling_with_mouse = 0;
            return;
        }
    }


    //on-click behaviors
	if( Global::Mouse::GetState( Button::LEFT_DOWN ) && RootHasFocus() && MouseOver() )
	{
        //update item under cursor (if any)
        std::string previous_item = selected_item;
        SelectUnderCursor( selected_item );

        //if we clicked the same thing twice
        if( !selected_item.empty() && previous_item == selected_item )
        {
            //if we double click a directory, move us there
            //else we clicked a file and something needs to be done
            if( IsDirectory( selected_item ) )
                BrowseToDirectory( selected_item );
            else
                DoSelectAction( selected_item );
        }
        //else do nothing, we just changed the selected item is all
	}

    //keep the node size up-to-date
	vec2 new_size = GetSize();
	new_size.y = std::min<float>( GetListSize(), GetParent()->GetSize().y );
	SetSize( new_size );
}


void GUIDirectoryBrowser::BeginBrowse()
{
    if(browsing)
        return;

    //at the beginning of a browse we want to initialize the path at the working directory
    //from here we will only change "working path"
    selected_item = "";
    GetCWD(start_path);
    working_path = start_path;
    GetDirectoryContent(directories, files, ".");
    browsing = 1;
}

void GUIDirectoryBrowser::GetCurrentDirectory(std::string& out)
{
    if( browsing )
        out = working_path;
    else
        GetCWD( out );
}

void GUIDirectoryBrowser::SetSelectionCallback(void (*new_select_action)(const std::string& path, const std::string& filename))
{
    select_action = new_select_action;
}

void GUIDirectoryBrowser::SetCloseAfterSelection(truth should_close_state)
{
	should_close = should_close_state;
}

float GUIDirectoryBrowser::GetListSize()
{
	return ((directories.size()+files.size())*13);
}

		