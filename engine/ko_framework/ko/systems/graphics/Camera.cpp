#include <Camera.h>
#include <GameObject.h>

Camera::Camera()
:_projection_matrix(mat4(1.0f))
,_view_center_x(100.0f)
,_view_center_y(50.0f)
,_extents(vec2(1.0f,1.0f))//default, meaningless
,_right_edge(_view_center_x+_extents.x)
,_top_edge(_view_center_y+_extents.y)
,_move_rate(10.0f)
,_window_width( Global::GameWindow->GetWRef() )
,_window_height( Global::GameWindow->GetHRef() )
,_target(0)
{
}


void Camera::ModifyViewCenter(float dx, float dy)
{
    _view_center_x += dx;
    _view_center_y += dy;
    UpdateCameraMatrix();
}
void Camera::ModifyViewCenter(const vec2& delta)
{
    _view_center_x += delta.x;
    _view_center_y += delta.y;
    UpdateCameraMatrix();
}
void Camera::SetViewCenter(float x, float y)
{
    _view_center_x = x;
    _view_center_y = y;
    UpdateCameraMatrix();
}
void Camera::SetViewCenter(const vec2& center)
{
    _view_center_x = center.x;
    _view_center_y = center.y;
    UpdateCameraMatrix();
}

float Camera::GetViewCenterX() const 
{ 
	return _view_center_x; 
}

float Camera::GetViewCenterY() const 
{ 
	return _view_center_y; 
}

vec2 Camera::GetViewCenter() const 
{ 
	return vec2( _view_center_x, _view_center_y ); 
}

void Camera::ModifyWindowZoom(float delta)
{
	_zoom += delta;
	if(_zoom < Global::TINY)
	   _zoom = Global::TINY;
	UpdateCameraMatrix();
}

void Camera::SetWindowZoom(float zoom)
{
	_zoom = zoom;
	if(_zoom < Global::TINY)
	   _zoom = Global::TINY;
	UpdateCameraMatrix();
}
		
float Camera::GetWindowZoom() const 
{ 
	return _zoom; 
}

void Camera::ModifyMoveRate(float delta)
{
    _move_rate += delta;
    _move_rate = std::max<float>( _move_rate, Global::TINY );
}

void Camera::SetMoveRate(float rate)
{
    _move_rate = rate;
    _move_rate = std::max<float>( _move_rate, Global::TINY );
}

unsigned Camera::GetWindowWidth() const
{ 
	return _window_width; 
}

unsigned Camera::GetWindowHeight() const 
{
	return _window_height; 
}

vec2 Camera::ScreenToWorld(const vec2& screen_pos) const
{
    vec3 v_in( screen_pos.x, _window_height - screen_pos.y, 0 );
	vec3 v_out( 0, 0, 0 );
	mat4 model(1.0f);
	vec4 viewport( 0, 0, _window_width, _window_height );
	v_out = glm::unProject( v_in, model, GetProjectionMatrixGLM(), viewport );

	return vec2(v_out.x,v_out.y);
}

void Camera::SetTarget( GameObject* target )
{
	_target = target;
}

GameObject* Camera::GetTarget() const
{
	return _target;
}

void Camera::MoveToTarget(float dt, float catchup_speed, float min_follow_distance)
{
	if(!_target)
		return;

	if( catchup_speed <= Global::ZERO )
	{
		SetViewCenter( _target->_transform._pos );
		return;
	}

	const float k_min_follow_distance = min_follow_distance;
	const float k_catchup_speed       = catchup_speed;
	const float k_min_speed           = 0.01f;

	float distance_x = _target->_transform._pos.x - _view_center_x;
	float distance_y = _target->_transform._pos.y - _view_center_y;

	float xsgn = static_cast<float>(distance_x > 0 ? 1 : -1 );
	float ysgn = static_cast<float>(distance_y > 0 ? 1 : -1 );

	distance_x = fabs(distance_x);
	distance_y = fabs(distance_y);

	if( distance_x > k_min_follow_distance )
		distance_x = std::max<float>( distance_x * k_catchup_speed, k_min_speed );
	else
		distance_x = 0;

	if( distance_y > k_min_follow_distance )
		distance_y = std::max<float>( distance_y * k_catchup_speed, k_min_speed );
	else
		distance_y = 0;

	_view_center_x += xsgn * distance_x;// * dt;
	_view_center_y += ysgn * distance_y;// * dt;
	UpdateCameraMatrix();
}

const float* Camera::GetProjectionMatrix() const
{
    return glm::value_ptr( _projection_matrix );
}

const mat4& Camera::GetProjectionMatrixGLM() const 
{ 
	return _projection_matrix; 
}

void Camera::UpdateCameraMatrix()
{
    float ratio = (float)_window_width / (float)_window_height;

    float extent_x = ratio * _zoom;//25.0f;
    float extent_y = _zoom;//25.0f;

    float lower_x = _view_center_x - extent_x;
    float lower_y = _view_center_y - extent_y;
    float upper_x = _view_center_x + extent_x;
    float upper_y = _view_center_y + extent_y;
    _extents.x = _view_center_x + extent_x;
    _extents.y = _view_center_y + extent_y;
	_right_edge = glm::abs( _extents.x - _view_center_x );
	_top_edge = glm::abs( _extents.y - _view_center_y );

    mat4& pMat = _projection_matrix;

    float r_minus_l = upper_x - lower_x;
    float t_minus_b = upper_y - lower_y;

    //build the projection matrix
    pMat[0][0] = 2.0f / (r_minus_l);
    pMat[0][1] = pMat[0][2] = pMat[0][3] = 0;

    pMat[1][1] = 2.0f / (t_minus_b);
    pMat[1][0] = pMat[1][2] = pMat[1][3] = 0;

    pMat[2][2] = -1.0f;
    pMat[2][0] = pMat[2][1] = pMat[0][3] = 0;

    pMat[3][0] = -(upper_x + lower_x) / (r_minus_l);
    pMat[3][1] = -(upper_y + lower_y) / (t_minus_b);
    pMat[3][2] = 0;
    pMat[3][3] = 1.0f;

    //need to leave in for various immediate mode debug drawing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf( glm::value_ptr( pMat ) );

    // L/R/B/T
    //gluOrtho2D(lower_x, upper_x, lower_y, upper_y);
}













