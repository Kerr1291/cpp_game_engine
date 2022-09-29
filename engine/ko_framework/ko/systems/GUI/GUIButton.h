#ifndef GUIBUTTON_H
#define GUIBUTTON_H

#include <GUINode.h>

class Command;

class GUIButton : public GUINode
{
    friend class GUIManager;
    private:
        Command* command;

        void (*click_action)(GUINode*);

        void (*click_function)(void*);
        void* click_function_data;

    public:

        GUIButton(const std::string& name = "");
        virtual ~GUIButton();

        void Render();
        truth MouseOver();
        void Update();

        void SetCommand(Command* button_command);

        //when click actions are called they are passed the a pointer to the button
        void SetClickAction(void (*new_click_action)(GUINode*));

        void SetClickFunction(void (*new_click_function)(void*), void* data);

        std::string text;
        vec2 text_pos;

        unsigned edge_thickness;
        vec3 text_color;
        vec3 bg_color;
        vec3 edge_color;
};

#endif
