#ifndef CAMERA_H
#define CAMERA_H

#include <globals.h>
class GameObject;

class Camera
{
	public:

		Camera();

		void ModifyViewCenter(float dx, float dy);
		void ModifyViewCenter(const vec2& delta);
		void SetViewCenter(float x, float y);
		void SetViewCenter(const vec2& center);
		float GetViewCenterX() const;
		float GetViewCenterY() const;
		vec2 GetViewCenter() const;

		void ModifyWindowZoom(float delta);
		void SetWindowZoom(float zoom);
		float GetWindowZoom() const;

		void ModifyMoveRate(float delta);
		void SetMoveRate(float rate);

		unsigned int GetWindowWidth() const;
		unsigned int GetWindowHeight() const;
		vec2 ScreenToWorld(const vec2& screen_pos) const;

		void SetTarget( GameObject* target );
		GameObject* GetTarget() const;

		//if catchup speed is <= 0 then the camera moves to the target instantly
		void MoveToTarget(float dt = 0.016f, float catchup_speed = 0.01f, float min_follow_distance = 2.0f);

		const float* GetProjectionMatrix() const;
		const mat4& GetProjectionMatrixGLM() const;
		
		//inline to make near-camera checks as fast as possible
		inline const vec2& GetCameraExtents() const { return _extents; }
		inline const float& GetViewCenterXRef() const { return _view_center_x; }
		inline const float& GetViewCenterYRef() const { return _view_center_y; }
		inline const float& GetTopEdge() const { return _top_edge; }
		inline const float& GetRightEdge() const { return _right_edge; }

		//called by member functions whenever the camera's state is changed
		void UpdateCameraMatrix();

	private:

		mat4  _projection_matrix;

		float _view_center_x;
		float _view_center_y;
		vec2  _extents;
		float _right_edge;
		float _top_edge;
		float _zoom;
		float _move_rate;

		const unsigned& _window_width;
		const unsigned& _window_height;

		GameObject* _target;
};


#endif

