#if !defined(TRIANGLE_HPP)
#define TRIANGLE_HPP

#include <array>
#include <vector>

#include "vector.hpp"

struct triangle
{
	std::vector<vec4d> points;

	vec4d color;

	// center of a triangle
	vec4d center() const;

	// normal of this face
	vec4d normal() const;
};

#endif // TRIANGLE_HPP
