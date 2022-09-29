#ifndef GUITEXTBOX_H
#define GUITEXTBOX_H

#include <GUINode.h>

class GUITextBox : public GUINode
{
    float spacing;
    float scrollbar_location;

	std::string text;
	std::vector<std::string> lines;

	//TODO: put font object + details here later once the ftgl stuff has been implemented

	unsigned NumLinesVisible();
	void RenderText(const std::string& text_to_render, vec2 location);
	void BuildLines();
    truth LineIsVisible(unsigned line_number);

	//scrollbar functions
	//TODO: think about moving scrollbar functionality out of the textbox GUI object and into its own class
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

        GUITextBox(const std::string& name = "");

        void Render();
        truth MouseOver();
        void Update();

        void SetSpacing(float new_spacing);
        vec3 scrollbar_color;
		vec3 text_color;

		void SetText(const std::string& input);
		const std::string& GetText();

};

#endif
