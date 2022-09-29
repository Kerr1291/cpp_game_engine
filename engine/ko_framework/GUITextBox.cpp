#include <GUITextBox.h>
#include <GUIManager.h>

#include <GUIWindow.h>

#if 0
//TODO: finish this GetLinRange function? Might not be necessary anymore, look over the code when I get back to it...
void GUITextBox::GetLineRange()
{
	const unsigned NUM_VISIBLE = NumLinesVisible();
	//TODO: implement the GetLineRange() function? look over the code later...
}
#endif

unsigned GUITextBox::NumLinesVisible()
{
	const int FONT_H = 13; //TODO: get this from the font being used after I move away from the glut font

	int MAX_LINES = ( static_cast<int>( GetSize().y ) / FONT_H );
	MAX_LINES = ( MAX_LINES < 0 ) ? -MAX_LINES : MAX_LINES;
	return static_cast<unsigned>( MAX_LINES );
}

void GUITextBox::RenderText(const std::string& text_to_render, vec2 location)
{
	glColor4f( text_color.r, text_color.g, text_color.b, 1.0f );
	glRasterPos2i( location.x, location.y+13 );
	glutBitmapString( GLUT_BITMAP_8_BY_13, (unsigned char*)text_to_render.c_str() );
}

void GUITextBox::BuildLines()
{
	const int FONT_W = 8; //TODO: get this from the font being used after I move away from the glut font

	lines.clear();

	int MAX_LINE_LEN = ( static_cast<int>( GetSize().x ) / FONT_W );
	MAX_LINE_LEN = ( MAX_LINE_LEN < 0 ) ? -MAX_LINE_LEN : MAX_LINE_LEN;


	std::string carry_over_word;
	std::string::iterator s_iter = text.begin();
	for(; s_iter != text.end();)
	{
		std::string new_line;
		std::string new_word;

		if( !carry_over_word.empty() )
		{
			new_word = carry_over_word;
			carry_over_word.clear();
		}

		for(; s_iter != text.end(); ++s_iter)
		{
			const char c = *s_iter;

			if( c == ' ' )
			{
				if( !new_word.empty() )
				{
					new_line += new_word;
					new_word.clear();
				}

				new_line.push_back( c );
			}
			else if( c == '\n' )
			{
				if( !new_word.empty() )
					new_line += new_word;
				new_word.clear();
				break;
			}
			else if( c == '\t' )
			{
				if( !new_word.empty() )
				{
					new_line += new_word;
					new_word.clear();
				}

				new_line.push_back( ' ' );
				new_line.push_back( ' ' );
				new_line.push_back( ' ' );
				new_line.push_back( ' ' );
			}
			else
			{
				new_word.push_back( c );
			}

			if( new_line.size() + new_word.size() + 1 > static_cast<unsigned>( MAX_LINE_LEN ) )
			{
				if( new_word.size() > 0 && new_line.size() > 0 )
					carry_over_word = new_word;
				else if( new_word.size() > 0 && new_line.empty() )
					new_line = new_word;
				new_word.clear();
				break;
			}
		}

		if( !new_word.empty() )
			new_line += new_word;

		if( s_iter != text.end() )
			++s_iter;

		lines.push_back( new_line );
	}
}

truth GUITextBox::LineIsVisible(unsigned line_number)
{
	return 1;
}

void GUITextBox::UpdateScrollbarParams()
{
    if(!this->GetParent())
        return;
    GUINode* p = this->GetParent()->GetParent();
    if(!p)
        return;

    float ratio =  p->GetSize().y / GetSize().y;
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

truth GUITextBox::MouseOverScrollbar()
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

void GUITextBox::RenderScrollbar()
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

truth GUITextBox::HasScrollbar()
{
    if(!this->GetParent())
        return 0;
    GUINode* p = this->GetParent()->GetParent();
    if(!p)
        return 0;

    float ratio =  p->GetSize().y / GetSize().y;
    if( ratio >= 1.0f )
        return 0;

    return 1;
}

GUITextBox::GUITextBox(const std::string& name)
:GUINode(0, name)
,spacing(2.0f)
,scrollbar_location(0.0f)
,text("")
,scrollbar_step(1.0f)
,scrollbar_color( vec3( 0.7f, 0.7f, 0.7f ) )
,text_color( vec3(1.0f,1.0f,1.0f) )
{
    UpdateScrollbarParams();
}

void GUITextBox::Render()
{
    if( hidden )
        return;

	for(unsigned i = 0; i < lines.size(); ++i)
	{
		RenderText( lines[i], vec2( GetScreenPos().x, GetScreenPos().y + (13+spacing) * i ) );
	}

    if( HasScrollbar() )
        RenderScrollbar();
}


truth GUITextBox::MouseOver()
{
    if( IsMouseOverNodes( children ) )
        return 1;

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

void GUITextBox::Update()
{
    if( MouseOver() && RootHasFocus() && glm::abs( Global::Mouse::WheelDelta ) > 0 )
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
}


void GUITextBox::SetSpacing(float new_spacing)
{
    spacing = new_spacing;
	BuildLines();
}

void GUITextBox::SetText(const std::string& input)
{
	text = input;
	BuildLines();
}

const std::string& GUITextBox::GetText()
{
	return text;
}
