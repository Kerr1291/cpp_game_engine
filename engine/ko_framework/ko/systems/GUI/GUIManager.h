#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <globals.h>

class GUINode;

class GUIManager
{
    public:

        GUIManager();
        ~GUIManager();

        void AddNode(GUINode* root_node);
        GUINode* FindRootNode(const std::string& name);

        void DeleteNode(GUINode* node);
        void DeleteNode(const std::string& name);
        void RemoveNode(GUINode* node);

        truth PendingGameLogic( GUINode* node );
        void Cleanup();
        void Update();
        void Render();

        truth HasFocus(GUINode* root_node);
        GUINode* GetFocus();
        void SetInputFocus(GUINode* node);
        GUINode* GetInputFocus();
        truth HasInputFocus(GUINode* node);

        void SetSignalFlag();
        truth GetSignalFlag();

		truth MouseOverGUI();

		void SetGUIHidden(bool hidden);

    private:

        truth NodeExists(GUINode* node);

        void MoveFocusToFront();
        void PackBack();
        void ShiftNodes(int direction);

    private:

		bool _hidden;
        GUINode* focus;
        GUINode* input_focus;
        truth signal_flag;
        std::map<int, GUINode*> nodes;
        std::list< GUINode* > delete_list;
};


#endif
