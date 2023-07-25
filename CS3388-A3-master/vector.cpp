#include "vector.hpp"

vec3d cross(const vec3d &a, const vec3d &b)
{
    double ax = a.at(0, 0);
	double ay = a.at(1, 0);
	double az = a.at(2, 0);

	double bx = b.at(0, 0);
	double by = b.at(1, 0);
	double bz = b.at(2, 0);

	return {{
		ay * bz - by * az,
		az * bx - bz * ax,
		ax * by - bx * ay,
	}};
}

vec4d homo(const vec3d &v)
{
    return {{
        v.at(0, 0),
        v.at(1, 0),
        v.at(2, 0),
        1.0
    }};
}

vec3d cart(const vec4d &v)
{
    double w = v.at(3, 0);

    if (w == 0.0)
        return {{}};

    return {{
        v.at(0, 0) / w,
        v.at(1, 0) / w,
        v.at(2, 0) / w,
    }};
}
