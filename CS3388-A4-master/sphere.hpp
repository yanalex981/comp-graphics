#ifndef A4_SPHERE_HPP
#define A4_SPHERE_HPP

#include "surface.hpp"

struct sphere : surface
{
	virtual std::optional<hit> intersect(const vec4d &ray_start, const vec4d &ray_end);
};

#endif //A4_SPHERE_HPP
