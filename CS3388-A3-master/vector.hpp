#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "matrix.hpp"
#include <iostream>
#include <vector>

template<typename T, size_t N>
using vec = matrix<T, N, 1>;

template<size_t N>
using fvec = vec<float, N>;

template<size_t N>
using dvec = vec<double, N>;

template<typename T>
using vec4 = vec<T, 4>;

using vec3d = dvec<3>;
using vec3f = fvec<3>;
using vec4d = dvec<4>;
using vec4f = fvec<4>;

// perspective division
template<typename T>
vec4<T> normalize_w(const vec4<T> &v)
{
	T w = v.at(3, 0);

	if (w == 0)
		return {{0, 0, 0, 1}};
	else if (w == 1.0)
		return v;

	return v / w;
}

// perspective div on a batch of vectors
template<typename T>
std::vector<vec4<T>> normalize_w(const std::vector<vec4<T>> &obj)
{
	std::vector<vec4<T>> normed;

	for (auto &pt : obj)
	{
		T w = pt.at(3, 0);

		if (w == 0)
			normed.push_back({{0, 0, 0, 1.0}});
		else if (w == 1.0)
			normed.emplace_back(pt);
		else
			normed.emplace_back(pt / w);
	}
	
	return normed;
}

// dot product
template<typename T, size_t N>
T dot(const vec<T, N> &a, const vec<T, N> &b)
{
	return (a.transpose() * b).at(0, 0);
}

// magnitude of a vector
template<typename T, size_t N>
T magnitude(const vec<T, N> &v)
{
	return std::sqrt(dot(v, v));
}

// trim a vector to unit length
template<typename T, size_t N>
vec<T, N> norm(const vec<T, N> &v)
{
	return v / magnitude(v);
}

// 3d cross product
vec3d cross(const vec3d &a, const vec3d &b);

// converts a 3d cartesian vector to 3d homogeneous
vec4d homo(const vec3d &v);

// converts a 3d homogeneous vector to 3d cartesian
// performs perspective div
// if w is 0, result is (0, 0, 0, 1)
// else, it's always (x/w, y/w, z/w, 1)
vec3d cart(const vec4d &v);

#endif
