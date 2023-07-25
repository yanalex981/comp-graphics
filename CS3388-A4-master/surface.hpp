#ifndef A4_SURFACE_HPP
#define A4_SURFACE_HPP

#include <vector>
#include <optional>

#include "matrix.hpp"
#include "matrix_utils.hpp"
#include "vector.hpp"
#include "material.hpp"

struct hit;

// extend to define surfaces
struct surface
{
	mat4d transforms = identity(); // transforms on this surface, to be able to create variations of this shape

	material material; // parameters for Phong lighting

	// returns a potential intersection given a ray
	virtual std::optional<hit> intersect(const vec4d &ray_start, const vec4d &ray_end) = 0;
};

// holds results from intersection checks
struct hit
{
	vec3d normal;
	vec3d world_pt;
	surface *obj;
};

#endif //A4_SURFACE_HPP
