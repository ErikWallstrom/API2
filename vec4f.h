#ifndef VEC4F_H
#define VEC4F_H

#include "vec3f.h"

struct Vec4f
{
	union 
	{
		float scalars[4];
		struct 
		{
			union 
			{
				struct Vec3f xyz;
				struct
				{
					float x, y, z;
				};
			};
			float w;
		};
		struct 
		{
			union 
			{
				struct Vec3f rgb;
				struct
				{
					float r, g, b;
				};
			};
			float a;
		};
	};
};

#endif
