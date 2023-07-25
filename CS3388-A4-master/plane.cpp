#include <vector>

#include "plane.hpp"

std::optional<hit> plane::intersect(const vec4d &ray_start, const vec4d &ray_end)
{
	auto inv = invert(transforms);

	// ray warped into model space
	auto start = cart(inv * ray_start);
	auto end = cart(inv * ray_end);
	auto dir = end - start;

	// no z difference, ray is either parallel to the plane or in the plane. just return nothing
	if (dir.z() == 0)
		return {};

	// solution to the parametric eq
	auto t = -start.z() / dir.z();

	// intersecting behind the camera
	if (t < 0)
		return {};

	auto intersection = start + dir * t;
	auto u = intersection.x();
	auto v = intersection.y();

	// check if solution is in bounds of the 2x2 sq
 	if (-1.0 <= u && u <= 1.0 && -1.0 <= v && v <= 1.0)
    {
	    vec4d normal{{ 0.0, 0.0, 1.0, 1.0 }};

	    return {{
			norm(cart(dir_to_world(transforms, normal))),
			cart(transforms * homo(intersection)),
			this
	    }};
    }

	return {};
}
