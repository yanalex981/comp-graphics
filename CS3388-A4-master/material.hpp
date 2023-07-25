#ifndef A4_MATERIAL_HPP
#define A4_MATERIAL_HPP

#include "light.hpp"
#include "matrix.hpp"
#include "vector.hpp"

struct material
{
	vec3d color;
	double k_ambient, k_diffuse, k_specular, k_reflect, fallout;
};

#endif //A4_MATERIAL_HPP
