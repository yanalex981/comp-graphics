#include <iostream>
#include <cmath>
#include <optional>

#include "cone.hpp"
#include "matrix_utils.hpp"
#include "vector.hpp"

// returns the normal of the cone based on a point of intersection
vec4d normal(const vec3d &v)
{
	return roty(atan2(v.x(), v.z())) * vec4d{{ 0, 1, 1, 1 }};
}

std::optional<hit> cone::intersect(const vec4d &ray_start, const vec4d &ray_end)
{
	// warp ray into model space
	auto inv = invert(transforms);
	auto start = cart(inv * ray_start);
	auto end = cart(inv * ray_end);
	auto dir = norm(end - start);

	auto tip = vec3d{{ 0.0, 1.0, 0.0 }}; // tip of the cone
	auto origin = vec3d{{ 0.0, 0.0, 0.0 }}; // origin

	auto v = norm(origin - tip); // vec from tip going towards origin
	auto co = start - tip; // vec from tip towards ray's starting point in model space

	// solutions to intersection
	double a = std::pow(dot(dir, v), 2) - 0.5;
	double b = 2 * ( dot(dir, v) * dot(co, v) - dot(dir, co) / 2.0 );
	double c = std::pow(dot(co, v), 2) - dot(co, co) / 2.0;

	double discrim = b * b - 4 * a * c;

	// not intersecting
	if (discrim < 0)
		return {};

	// 2 intersections
	double t1 = (-b - std::sqrt(discrim)) / 2 / a;
	double t2 = (-b + std::sqrt(discrim)) / 2 / a;

	// intersecting behind the camera
	if (t1 < 0 && t2 < 0)
		return {};

	// various vectors and dots for one intersection
	auto i2 = start + dir * t2; // intersection on cone surface in model coords
	auto w2 = cart(transforms * homo(i2)); // intersection in world coords
	auto n2 = cart(dir_to_world(transforms, normal(i2))); // normal dir in world coords
	auto dot2 = dot(i2 - tip, v); // shadow of the intersection on the axis
	bool d2_in_range = 0 <= dot2 && dot2 <= 1; // the shadow should be positive and less than 1
	// >1 means the point is below the base of the cone, which is not actually part of the cone we want
	// <0 means the point is on the shadow cone above the tip
	hit hit2{ n2, w2, this };
	if (t1 < 0) // t1 is behind the camera
	{
		// check if the other point is overextending
		if (d2_in_range)
			return hit2;
		else
			return {};
	}

	// various vectors and dots for the other intersection
	auto i1 = start + dir * t1;
	auto w1 = cart(transforms * homo(i1));
	auto n1 = cart(dir_to_world(transforms, normal(i1)));
	auto dot1 = dot(i1 - tip, v);
	bool d1_in_range = 0 <= dot1 && dot1 <= 1;
	hit hit1{ n1, w1, this };
	if (t2 < 0) // t2 is behind the camera
	{
		if (d1_in_range)
			return hit1;
		else
			return {};
	}

	if (!d1_in_range && !d2_in_range)
		return {}; // false alarm, intersecting the shadow cone, or below the base

	// both are in range, return the closest one 'cause the other one is actually behind it
	if (d1_in_range && d2_in_range)
	{
		if (t1 < t2)
			return hit1;

		return hit2;
	}

	// one is negative, return either
	if (d2_in_range)
		return hit2;

	return hit1;
}
