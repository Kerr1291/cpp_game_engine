#ifndef LIGHT_H
#define LIGHT_H

#include <globals.h>

struct Light
{
	Light()
	{
		radius = 1.0f;
		intensity = 1.0f;
		color = vec4(1,1,1,1);
		position = vec2(0,0);
		direction = vec2(0,-1);
	}
	float radius;
	float intensity;
	vec4 color;
	vec2 position;
	vec2 direction;
	float angle;
};


#endif