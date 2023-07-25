#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "matrix.hpp"

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

#endif
