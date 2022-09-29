#ifndef EDITORGUIBUILDER_H
#define EDITORGUIBUILDER_H

#include <globals.h>
class Editor;
class GUIManager;

namespace GUI
{
	//a "namespace" for functions to create various editor GUI parts
	namespace EditorGUIBuilder
	{

		namespace Callbacks
		{
			void TogglePaintingMode(void* null);
			void TogglePhysicsMode(void* null);
			void ToggleLightsMode(void* null);
			void ToggleObjectsMode(void* null);
			void ToggleFillMode(void* null);
			void ToggleDisabled(void* null);
			void OpenAtlasFileIntoEditorPainter(void* null);
			void CreateNewArea(void* null);
			void OpenCreateNewAreaWindow(void* null);

			void OpenTextFile(const std::string& path, const std::string& filename);
			void OpenAtlasMap(const std::string& path, const std::string& filename);
		};


		void Hotkey_Check();
		void InitBuilder(Editor* editor);

		//creation functions
		void CreateMainWindow();
		void CreatePaintingEditorWindow();
		void CreatePhysicsEditorWindow();
		void CreateLightsEditorWindow();
		void CreateObjectsEditorWindow();

		void CreateOpenTextFileWindow();
		void CreateOpenAtlasMapWindow();
		void CreateNewAreaWindow();

		//state checking functions (to see if windows exists and is active)
		int GetActiveWindowModeType();
	};
}

#endif
