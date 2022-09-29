//#include <GUIField.h>
#include <GUIManager.h>

template <typename T>
void GUIField<T>::UpdateInput()
{
    const unsigned char ENTER = 13;
    const unsigned char BACKSPACE = 8;
    if( Global::Keyboard::KeyPressed( ENTER ) )
    {
        if( input_text.size() > 0 )
        {
            StringToVar();
        }
        input_in_process = 0;
        SetInputFocus( 0 );
    }
    else
    {
        //read from last pressed key, update text input
        if( Global::Keyboard::AnyKeyWasPressed() )
        {
            unsigned char key_pressed = Global::Keyboard::GetLastPressedKey();
            if( key_pressed == BACKSPACE )
            {
                if( input_text.size() > 0 )
                    input_text.erase( input_text.size()-1, 1 );
            }
            else
                input_text.push_back( key_pressed );
        }
    }
}

template <>
inline void GUIField<std::string>::StringToVar()
{
	var = input_text;
}

template <>
inline void GUIField<truth>::VarToString()
{
    std::stringstream s;
    s << (int)var;
    text = s.str();
    input_text = text;
}

template <typename T>
void GUIField<T>::StringToVar()
{
    std::stringstream s;
    s << input_text;
    s >> var;
}

template <typename T>
void GUIField<T>::VarToString()
{
    std::stringstream s;
    s << var;
    text = s.str();
    input_text = text;
}


template <typename T>
GUIField<T>::GUIField(T& _var, truth _read_only, const std::string& name)
:GUINode(1, name)
,var(_var)
,read_only(_read_only)
,text("")
,input_in_process(0)
,input_text("")
,cursor_char('_')
,cursor_blink_timer(0)
{
    VarToString();
}

template <typename T>
void GUIField<T>::Render()
{
    if( hidden )
        return;

    if( RootHasFocus() == 0 || HasInputFocus() == 0 )
    {
        if( HasInputFocus() )
            SetInputFocus( 0 );
        input_in_process = 0;
    }

	glRasterPos2i( (int)screen_pos.x, (int)screen_pos.y + 13 );
	std::string* render_string = 0;

    if( input_in_process )
    {
        UpdateInput();
        render_string = &input_text;
    }
    else
    {
        VarToString();
        render_string = &text;
    }

    if( MouseOver() )
        glColor3f(1.0f, 0.0f, 0.0f);
    else
        glColor3f(1.0f, 1.0f, 1.0f);

	std::string endchar = "";
    if( input_in_process )
		endchar.push_back( cursor_char );

	glutBitmapString( GLUT_BITMAP_8_BY_13, (unsigned char*)((*render_string + endchar).c_str()) );
}

template <typename T>
truth GUIField<T>::MouseOver()
{
    vec2 mouse = Global::Mouse::ScreenPos;
	if( parent && dynamic_cast<GUIWindow*>( parent ) )
	{
		if( mouse.x < parent->GetScreenPos().x )
			return 0;
		if( mouse.y < parent->GetScreenPos().y )
			return 0;
		if( mouse.x > parent->GetScreenPos().x + parent->GetSize().x )
			return 0;
		if( mouse.y > parent->GetScreenPos().y + parent->GetSize().y )
			return 0;

		return 1;
	}

    if( mouse.x < screen_pos.x - 2 )
        return 0;
    if( mouse.y < screen_pos.y )
        return 0;
    if( mouse.x > screen_pos.x + text.size() * 8 + 20 )
        return 0;
    if( mouse.y > screen_pos.y + 15 )
        return 0;

    return 1;
}

template <typename T>
void GUIField<T>::Update()
{
    if( Global::Mouse::GetState( Button::LEFT_DOWN ) && MouseOver() && RootHasFocus() && External::gui->GetSignalFlag() == 0 )
    {
        External::gui->SetSignalFlag();
        SetInputFocus( this );
        input_in_process = 1;
    }
}
