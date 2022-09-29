#include <GUIWindow.h>
#include <GUIManager.h>

GUIWindow::GUIWindow(const std::string& name)
:GUINode(0, name)
,mouse_previous_pos( vec2(0,0) )
,dragging( 0 )
,edge_thickness(1)
,bg_color( vec3(1.0f,1.0f,1.0f) )
,edge_color( vec3(1.0f,1.0f,1.0f) )
{
}

GUIWindow::~GUIWindow()
{}

/*
std::string PrintVec2( const vec2& v )
{
	std::stringstream s;
	s << "(" <<v.x <<"," <<v.y <<")";
	return s.str();
}
		Debug::printf(20,700, ("delta " + PrintVec2(delta)).c_str());
*/


void GUIWindow::Render()
{
    if( hidden )
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    int w = Global::GameWindow->GetW();
    int h = Global::GameWindow->GetH();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

	float scale = 1.0f;

	if( MouseOver() && RootHasFocus() )
		scale = 1.1f;
	
	if( GetName() == "titlebar" && External::gui->GetFocus() != GetRoot() )
	{
		scale = 0.5f;
	}

	glColor4f( bg_color.r * scale, bg_color.g * scale, bg_color.b * scale, alpha );

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f( screen_pos.x, screen_pos.y );
        glVertex2f( screen_pos.x, screen_pos.y + GetSize().y );
        glVertex2f( screen_pos.x + GetSize().x, screen_pos.y + GetSize().y );
        glVertex2f( screen_pos.x + GetSize().x, screen_pos.y );
    glEnd();

	glColor4f( edge_color.r, edge_color.g, edge_color.b, alpha );

    glBegin(GL_LINE_LOOP);
        glVertex2f( screen_pos.x, screen_pos.y );
        glVertex2f( screen_pos.x, screen_pos.y + GetSize().y );
        glVertex2f( screen_pos.x + GetSize().x, screen_pos.y + GetSize().y );
        glVertex2f( screen_pos.x + GetSize().x, screen_pos.y );
    glEnd();
    glDisable(GL_BLEND);

    RenderNodes( members );
    RenderNodes( children );

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

truth GUIWindow::MouseOver()
{
    if( GetName() == "window" && IsMouseOverNodes( children ) )
        return 0;

    if( IsMouseOverNodes( members ) )
        return 1;

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

void GUIWindow::Update()
{
	if( Global::Mouse::GetState( Button::LEFT_DOWN ) && MouseOver() && GetName() == "titlebar" )
	{
		dragging = 1;
		mouse_previous_pos = Global::Mouse::ScreenPos;
		return;
	}

	if( dragging && Global::Mouse::GetState( Button::LEFT_HELD ) )
	{
		vec2 current_pos = Global::Mouse::ScreenPos;
		vec2 delta = current_pos - mouse_previous_pos;
		mouse_previous_pos = current_pos;

		this->parent->MovePos( delta );

		GUINode* container = ( GetRoot() ? GetRoot() : GetParent() );

        if( parent->GetScreenPos().x < container->GetScreenPos().x )
            GetParent()->MovePos( -delta );
        if( parent->GetScreenPos().y < container->GetScreenPos().y )
            GetParent()->MovePos( -delta );
        if( parent->GetScreenPos().x + parent->GetSize().x > container->GetScreenPos().x + container->GetSize().x )
            GetParent()->MovePos( -delta );
        if( parent->GetScreenPos().y + parent->GetSize().y > container->GetScreenPos().y + container->GetSize().y )
            GetParent()->MovePos( -delta );

		return;
	}

	if( dragging && Global::Mouse::GetState( Button::LEFT_UP ) )
	{
		dragging = 0;
		return;
	}

	UpdateNodes( children );
	UpdateNodes( members );
}
