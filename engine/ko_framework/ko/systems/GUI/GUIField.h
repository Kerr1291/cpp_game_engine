#ifndef GUIFIELD_H
#define GUIFIELD_H

#include <GUINode.h>

template <typename T>
class GUIField : public GUINode
{
    private:

        T& var;
        const truth read_only;
        std::string text;

        truth input_in_process;
        std::string input_text;
		unsigned char cursor_char;
		unsigned cursor_blink_timer;

        void UpdateInput();
        void StringToVar();
        void VarToString();

    public:

        GUIField(T& _var, truth _read_only, const std::string& name = "");

        void Render();
        truth MouseOver();
        void Update();
};

#include <GUIWindow.h>

#include <GUIField.cpp>

namespace GUI
{

namespace Field
{

//Field functions, requires parent
template <typename T>
GUINode* MakeField(GUINode* parent, T* var, const vec2& position, const float size, const std::string& name)
{
	//if( !parent )
	//	return 0;

	const std::string field_name = "field";
	const std::string window_name = "field_window";

	GUIWindow* base = new GUIWindow(name);

	const float FONT_H_SPACE = 17;
	vec2 field_size = vec2(size, FONT_H_SPACE);

	base->SetLocalTransform( position, field_size );
	base->bg_color = vec3(0,0,0);
	base->edge_color = vec3(0.7f, 0.7f, 0.7f);
	base->SetAlpha(0);

    GUIField<T>* field = new GUIField<T>(*var, 0, field_name);
    field->SetPos(vec2(2,2));
	base->AttachMember(1,field);

	GUIWindow* field_window = new GUIWindow(window_name);
	field_window->SetLocalTransform( vec2(0,0), field_size );
	field_window->bg_color = vec3(0.15f, 0.15f, 0.15f);
	field_window->edge_color = vec3(0.5f, 0.3f, 0.5f);
	base->AttachMember(0,field_window);

	if( parent )
		parent->AttachMember( 0, base );

	return base;
}

}//End Field


}

#endif
