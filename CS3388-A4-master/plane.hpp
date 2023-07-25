#ifndef A4_PLANE_HPP
#define A4_PLANE_HPP

#include "surface.hpp"

struct plane : public surface
{
	virtual std::optional<hit> intersect(const vec4d &ray_start, const vec4d &ray_end);
};

#endif //A4_PLANE_HPP
