#include "triangle.hpp"

vec4d triangle::center() const
{
	return (points[0] + points[1] + points[2]) / 3.0;
}

vec4d triangle::normal() const
{
    vec3d a = norm(cart(points[1]) - cart(points[0]));
    vec3d b = norm(cart(points[2]) - cart(points[0]));

    return homo(norm(cross(a, b)));
}
