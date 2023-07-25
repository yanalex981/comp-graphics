#include <iostream>
#include <cmath>

#include "sphere.hpp"
#include "vector.hpp"
#include "matrix_utils.hpp"

std::optional<hit> sphere::intersect(const vec4d &ray_start, const vec4d &ray_end)
{
	auto inv = invert(transforms);
	auto start = cart(inv * ray_start);
	auto end = cart(inv * ray_end);
	auto dir = norm(end - start); // warp the ray into model space

	// solution to intersection
	double a = dot(dir, dir);
	double b = 2 * dot(dir, start);
	double c = dot(start, start) - 1;

	double discrim = b*b - 4*a*c;

	// discriminant has to be >= 0
	// no hits
	if (discrim < 0)
		return {};

	double t1 = (-b + std::sqrt(discrim)) / (2 * a);
	double t2 = (-b - std::sqrt(discrim)) / (2 * a);

	// "intersects" behind camera, false alarm
	if (t1 < 0 && t2 < 0)
		return {};

	// values for one intersection
	auto i1 = start + dir * t1;
	auto w1 = cart(transforms * homo(i1));
	auto n1 = norm(cart(dir_to_world(transforms, homo(i1))));
	hit hit1{ n1, w1, this };

	// values for the alternate intersection
	auto i2 = start + dir * t2;
	auto w2 = cart(transforms * homo(i2));
	auto n2 = norm(cart(dir_to_world(transforms, homo(i2))));
	hit hit2{ n2, w2, this };

	// both in front of camera, pick the closest one
	if (t1 >= 0 && t2 >= 0)
	{
		if (t1 < t2)
			return hit1;

		return hit2;
	}

	// one or the other is in front of the camera, pick that one
	if (t1 >= 0)
		return hit1;

	return hit2;
}
