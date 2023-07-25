#ifndef A4_CONE_HPP
#define A4_CONE_HPP

#include "surface.hpp"

struct cone : public surface
{
	virtual std::optional<hit> intersect(const vec4d &ray_start, const vec4d &ray_end);
};


#endif //A4_CONE_HPP
