#if !defined(MESH_HPP)
#define MESH_HPP

#include <vector>
#include <array>

#include "triangle.hpp"

// object of triangular faces
// contains an initial color for further processing
struct mesh
{
	std::vector<triangle> faces;

	vec4d color;
};

#endif // MESH_HPP
