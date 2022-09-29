
#include <Editor.h>

truth IsInside(const vec2& p, const vec2& pos, const vec2& size)
{
	if( (p.x < pos[0]) )
		return 0;
	if( (p.x > pos[0] + size[0]) )
		return 0;
	if( (p.y < pos[1]) )
		return 0;
	if( (p.y > pos[1] + size[1]) )
		return 0;
	return 1;
}

void CreateSelectionBox(vec2& bottom_left, vec2& top_right)
{
	float left_x = 0;
	float right_x = 0;
	float bottom_y = 0;
	float top_y = 0;

	if( bottom_left.x < top_right.x )
	{
		left_x = bottom_left.x;
		right_x = top_right.x;
	}
	else
	{
		left_x = top_right.x;
		right_x = bottom_left.x;
	}

	if( bottom_left.y < top_right.y )
	{
		bottom_y = bottom_left.y;
		top_y = top_right.y;
	}
	else
	{
		bottom_y = top_right.y;
		top_y = bottom_left.y;
	}

	bottom_left = vec2(left_x, bottom_y);
	top_right = vec2(right_x, top_y);
}