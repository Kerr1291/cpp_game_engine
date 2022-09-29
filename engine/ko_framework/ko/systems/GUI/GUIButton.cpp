#include <GUIButton.h>
#include <GUIManager.h>

#include <SystemInterface.h>
#include <Command.h>

GUIButton::GUIButton(const std::string& name)
:GUINode(1, name)
,command(0)
,click_action(0)
,click_function(0)
,click_function_data(0)
,text("")
,text_pos( vec2(0.1f, 0.75f ) )
,edge_thickness(1)
,text_color( vec3(1.0f,1.0f,1.0f) )
,bg_color( vec3(1.0f,1.0f,1.0f) )
,edge_color( vec3(1.0f,1.0f,1.0f) )
{
}

GUIButton::~GUIButton()
{
    //only delete commands if this button is designated to manage them
    if( command && command->managed == CommandType::NON_MANAGED )
        delete command;
}

void GUIButton::Render()
{
    if( hidden )
        return;

	float scale = 1.0f;

	if( MouseOver() && RootHasFocus() )
    {
        if( Global::Mouse::GetState( Button::LEFT_HELD ) || Global::Mouse::GetState( Button::LEFT_DOWN ) )
            scale = 0.7f;
        else
            scale = 1.1f;
    }

    if( !parent )
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        int w = Global::GameWindow->GetW();
        int h = Global::GameWindow->GetH();
        gluOrtho2D(0, w, h, 0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
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


    if( text.size() > 0 )
    {
        glColor4f( text_color.r, text_color.g, text_color.b, alpha );
        text_pos = glm::clamp( text_pos, vec2(0,0), vec2(1.0f,1.0f) );
        glRasterPos2i( screen_pos.x + text_pos.x * GetSize().x, screen_pos.y + text_pos.y * GetSize().y );
        glutBitmapString( GLUT_BITMAP_8_BY_13, (unsigned char*)text.c_str() );
    }


    if( !parent )
    {
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }
}

truth GUIButton::MouseOver()
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

void GUIButton::Update()
{
	if( Global::Mouse::GetState( Button::LEFT_DOWN ) && RootHasFocus() && MouseOver() )
	{
        if( click_action )
        {
            click_action(this);
        }

	    if( command )
        {
            External::system->AddCommand( command );
        }

        if( click_function )
        {
            click_function(click_function_data);
        }
	}
}

void GUIButton::SetCommand(Command* button_command)
{
    command = button_command;
    if( command )
        command->managed = CommandType::NON_MANAGED;
}

void GUIButton::SetClickAction(void (*new_click_action)(GUINode*))
{
    click_action = new_click_action;
}

void GUIButton::SetClickFunction(void (*new_click_function)(void*), void* data)
{
    if( new_click_function )
    {
        click_function = new_click_function;
        click_function_data = data;
    }
    else
    {
        click_function = 0;
        click_function_data = 0;
    }
}
