#ifndef EDITOR_H
#define EDITOR_H

#include <globals.h>
#include <tweakbar.h>

class BaseObject;
class b2Body;
struct Light;
class GridInfo;
class GUINode;

const unsigned short CLEAR_TYPE = (unsigned short)(-1);

namespace EditorMode
{
	const unsigned SELECTION = 0;
	const unsigned PAINT = 1;
	const unsigned PHYSICS = 2;
	const unsigned LIGHTS = 3;
	const unsigned OBJECTS = 4;

	const unsigned NUM_MODES = 5;
}

//////////////////////////////////////////////
// Helper functions (implemented in Editor_Helpers.cpp)
truth IsInside(const vec2& p, const vec2& pos, const vec2& size);
void CreateSelectionBox(vec2& bottom_left, vec2& top_right);
///////////////////////////////////////////////


class ObjectEditor
{
	friend class Editor;
	public:
		~ObjectEditor();

	private:

		ObjectEditor();
		virtual void RegisterTBVariables();
		//TwBar* GetBar() { return _t; }

		void InitEditMode();
		void ExitEditMode();
	private:
		truth object_build_mode;
};

//EditorGUIBuilder forward declarations
namespace GUI { namespace EditorGUIBuilder {
void CreateMainWindow();
}}


class Editor
{
	friend void GUI::EditorGUIBuilder::CreateMainWindow();
	public://functions

		Editor();
		~Editor();

		virtual void RegisterTBVariables();

		void Init();
		void Update(double fps);
		void Draw();

		void ClearSelections();
		void SetMode(unsigned new_mode);
		inline unsigned GetMode() { return mode; }
		void UpdateEditorMode();
		GridInfo* GetCurrentAreaGrid();
		void GetCurrentAreaGridSize( vec2& out_size );
		void SetCurrentAreaGrid( GridInfo* in_grid );
		void SetCurrentAreaName( const std::string& name );

		truth IsDisabled() { return disabled; }
		void ToggleDisabled();
		void ToggleFillMode();
		void ForceSelectPaintingType(unsigned type);

	public://variables

		std::vector<BaseObject*> selected_objects;
		std::vector<b2Body*>* selected_bodies;

	private://functions

		void SelectInsideBox();
		void MoveObjectsWithMouse();
		void MoveCameraWithMouse();
		truth IsMouseOverPaintingGUI() const;
		unsigned short GetPaintingTypeFromGUI(const vec2& screen_pos) const;
		void SelectPaintingTypeFromGUI();
		void DrawSelectedAtlasTextures();
		void FillInsideBox();
		void FillPhysicsInsideBox();
		void CreatePhysicsEdge();
		void SnapToGrid(GridInfo* grid, vec2& point_to_snap);
		void SelectLightsInsideBox();
		void DeleteSelectedLights();
		void MoveLightsWithMouse();
		void ExposeObjects();
		void ClearExposedObjects();

	public://public for the ObjectEditor
		void SetObjectEditorMode(truth mode);
		void RefreshObjectEditor();

	private:
		void ProcessKeyboardInput();
		void ProcessMouseInput();

		void SelectionKeyboardInput();
		void SelectionMouseInput();

		void PaintingKeyboardInput();
		void PaintingMouseInput();

		void PhysicsKeyboardInput();
		void PhysicsMouseInput();

		void LightsKeyboardInput();
		void LightsMouseInput();

		void ObjectsKeyboardInput();
		void ObjectsMouseInput();

	private://variables

		std::string previous_gui_focus;
		unsigned mode;

		truth disabled;

		truth in_selection;
		vec2 selection_start;
		vec2 selection_end;

		unsigned short painting_type;
		unsigned char painting_layer;
		std::vector<unsigned> gui_atlas_textures;
		truth fill_mode;

		unsigned physics_type;
		truth physics_edge_mode;
		unsigned physics_edge_point_num;
		vec2 physics_edge_start;
		vec2 physics_edge_end;
		truth snap_to_grid;

		truth light_placing_enabled;
		std::vector<Light*> selected_lights;

		ObjectEditor object_editor;
};


#endif
