#ifndef GEOM_HPP
#define GEOM_HPP

#include <cstddef>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include "vector.hpp"
#include "mesh.hpp"

// prints out a matrix/vector, helps with debugging
template<typename T, size_t M, size_t N>
std::ostream &operator<<(std::ostream &os, const matrix<T, M, N> &m);

// 2D point, simple alias is enough
template<typename T>
using point = std::pair<T, T>;

// alias for list of 2D point, to avoid typing
template<typename T>
using vpoints = std::vector<point<T>>;

// applies a transformation matrix to a collection of vertices
template<typename T, size_t M, size_t N>
std::vector<vec<T, M>> operator*(const matrix<T, M, N> &m, const std::vector<vec<T, N>> &verts)
{
	std::vector<vec<T, M>> result;

	for (auto &vert : verts)
	{
		auto t = m * vert;
		result.push_back(t);
	}

	return result;
}

// turns a list of verts [a, b, c, ...] into
// [a, b, b, c, c, d ...]
// such that every pair (a, b), (b, c) ... are the endpoints of a line
std::vector<vec4d> to_edges(const std::vector<vec4d> &points);

// makes a list of vertices r away from the center, evenly distributed, on the xy plane
// segments are the number of segments the circumference of the circle is divided into
// more segments make a rounder circle
// the first point is repeated as the last element so that when it's made into edges,
// the last line segment completing the circle will be made
// points to a "circle" of 4 segments: [a, b, c, d, a]
// to_edges: [a, b,   b, c,   c, d,   d, a], 4 segments
// whereas [a, b, c, d] yields [a, b,   b, c,   c, d] and there's an open gap
std::vector<vec4d> make_circle(double radius, size_t segments);

// makes a collection of edges for a wireframe of a torus, can be drawn directly
// r_torus is the radius of the ring
// r_tube is the radius of the tube that runs latitudinally
// more segments make the object smoother
std::vector<vec4d> make_torus(double r_torus, double r_tube, size_t torus_segments, size_t tube_segments);

// makes edges for a "sphere". found out that some torus are spheres, so I'm using that
// too lazy to write out a dedicated method
std::vector<vec4d> make_sphere(double r, size_t lat_segments, size_t long_segments);

// r is radius of the base
// height is the height of the cone
// more segments make the cone smoother
std::vector<vec4d> make_cone(double r, double height, size_t base_segments);

// computes a point on a sphere based on theta and phi (spherical coords)
vec4d make_sphere_pt(double radius, double theta, double phi);

// make a sphere using triangles
mesh make_sphere_mesh(double radius, size_t lat_divs, size_t long_divs);

// computes the radius of a section of a cone
double cone_radius(double base_radius, double height, double h);

// make a cone using triangles
mesh make_cone_mesh(double radius, double height, size_t lat_divs, size_t long_divs);

#endif
