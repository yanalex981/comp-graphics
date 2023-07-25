#ifndef A4_MATRIX_UTILS_HPP
#define A4_MATRIX_UTILS_HPP

#include "matrix.hpp"
#include "vector.hpp"

template<typename T, size_t M, size_t N>
std::ostream &operator<<(std::ostream &os, const matrix<T, M, N> &m)
{
	os << m.row_size << 'x' << m.col_size;

	if (m.row_size <= 1) std::cout << ' ';
	else std::cout << std::endl;

	for (size_t i = 0; i < m.row_size; ++i)
	{
		for (size_t j = 0; j < m.col_size; ++j)
			os << m.at(i, j) << ' ';

		os << std::endl;
	}

	return os;
}

// returns a square identity matrix
template<size_t N = 4, typename T = double>
constexpr matrix<T, N, N> identity()
{
	matrix<T, N, N> m;

	for (size_t i = 0; i < N; ++i)
		m.at(i, i) = 1;

	return m;
}

template<typename T, size_t N>
constexpr matrix<T, N - 1, N - 1> minor(const matrix<T, N, N> &m, size_t row, size_t col)
{
	matrix<T, N - 1, N - 1> result;

	for (size_t i = 0, res_i = 0; i < N; ++i)
	{
		if (i == row)
			continue;

		for (size_t j = 0, res_j = 0; j < N; ++j)
		{
			if (j == col)
				continue;

			result.at(res_i, res_j) = m.at(i, j);

			res_j += 1;
		}

		res_i += 1;
	}

	return result;
}

template<typename T>
constexpr T det(const matrix<T, 1, 1> &m)
{
	return m.at(0, 0);
}

template<typename T, size_t N>
constexpr T det(const matrix<T, N, N> &m)
{
	double sign = 1.0;
	double d = 0.0;

	for (size_t i = 0; i < N; ++i, sign = -sign)
	{
		d += sign * m.at(0, i) * det(minor(m, 0, i));
	}

	return d;
}

template<typename T, size_t N>
constexpr matrix<T, N, N> adjugate(const matrix<T, N, N> &m)
{
	matrix<T, N, N> adjugate;

	double sign = 1;
	for (size_t i = 0; i < N; ++i)
	{
		for (size_t j = 0; j < N; ++j)
		{
			sign = (i + j) % 2 == 0? 1 : -1;
			adjugate.at(i, j) = sign * det(minor(m, j, i));
		}
	}

	return adjugate;
}

template<typename T, size_t N>
constexpr matrix<T, N, N> invert(const matrix<T, N, N> &m)
{
	return adjugate(m) / det(m);
}

// generates a 4x4 homogeneous translation matrix
template<typename T = double>
constexpr matrix<T, 4, 4> translate(T x, T y, T z)
{
	auto m = identity();

	m.at(0, 3) = x;
	m.at(1, 3) = y;
	m.at(2, 3) = z;

	return m;
}

// generates a 4x4 homogeneous scaling matrix
template<typename T = double>
constexpr matrix<T, 4, 4> scale(T x, T y, T z)
{
	matrix<T, 4, 4> m;

	m.at(0, 0) = x;
	m.at(1, 1) = y;
	m.at(2, 2) = z;
	m.at(3, 3) = 1;

	return m;
}

// generates a 4x4 homogeneous matrix that scales xyz axis uniformly
template<typename T = double>
constexpr matrix<T, 4, 4> scale(T k)
{
	auto m = identity();
	m.at(3, 3) = 1 / k;

	// return scale(k, k, k);
	return m;
}

// generates a 4x4 homogeneous matrix to rotate by the x axis
template<typename T = double>
constexpr matrix<T, 4, 4> rotx(T rad)
{
	auto m = identity();

	m.at(1, 1) =  std::cos(rad);
	m.at(1, 2) = -std::sin(rad);
	m.at(2, 1) =  std::sin(rad);
	m.at(2, 2) =  std::cos(rad);

	return m;
}

// generates a 4x4 homogeneous matrix to rotate by the y axis
template<typename T = double>
constexpr matrix<T, 4, 4> roty(T rad)
{
	auto m = identity();

	m.at(0, 0) =  std::cos(rad);
	m.at(0, 2) =  std::sin(rad);
	m.at(2, 0) = -std::sin(rad);
	m.at(2, 2) =  std::cos(rad);

	return m;
}

// generates a 4x4 homogeneous matrix to rotate by the y axis
template<typename T = double>
constexpr matrix<T, 4, 4> rotz(T rad)
{
	auto m = identity();

	m.at(0, 0) =  std::cos(rad);
	m.at(0, 1) = -std::sin(rad);
	m.at(1, 0) =  std::sin(rad);
	m.at(1, 1) =  std::cos(rad);

	return m;
}

// generates a 4x4 homogeneous matrix to rotate by the z, y, and then x axis
template<typename T = double>
constexpr matrix<T, 4, 4> rotate(T xrad, T yrad, T zrad)
{
	return rotx(xrad) * roty(yrad) * rotz(zrad);
}

template<typename T>
constexpr matrix<T, 4, 4> camera(const matrix<T, 3, 1> &eye, const matrix<T, 3, 1> &gaze, const matrix<T, 3, 1> &up)
{
	auto dir = norm(eye - gaze);
	auto u = norm(cross(up, dir));
	auto v = norm(cross(dir, u));

	matrix<T, 4, 4> m{{
		u.at(0, 0), u.at(1, 0), u.at(2, 0), 0,
		v.at(0, 0), v.at(1, 0), v.at(2, 0), 0,
		dir.at(0, 0), dir.at(1, 0), dir.at(2, 0), 0,
		eye.at(0, 0), eye.at(1, 0), eye.at(2, 0), 1
	}};

	return invert(m);
}

template<typename T>
constexpr matrix<T, 4, 4> perspective(T far, T near, T left, T right, T top, T bottom)
{
	T a = -(far + near) / (far - near);
	T b = -2 * far * near / (far - near);

	matrix<T, 4, 4> mp{{
		near, 0, 0, 0,
		0, near, 0, 0,
		0, 0, a, -1,
		0, 0, b, 0
	}};

	matrix<T, 4, 4> s1{{
		2 / (right - left), 0 ,0, 0,
		0, 2 / (top - bottom), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	}};

	matrix<T, 4, 4> t1{{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-(right + left) / 2, -(top + bottom) / 2, 0, 1
	}};

	return s1 * t1 * mp;
}
template<typename T>
constexpr matrix<T, 4, 4> screen(T width, T height)
{
	matrix<T, 4, 4> t2{{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		1, 1, 0, 1
	}};

	matrix<T, 4, 4> s2{{
		width / 2, 0, 0, 0,
		0, height / 2, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	}};

	matrix<T, 4, 4> w{{
		1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, 1, 0,
		0, height, 0, 1
	}};

	return w * s2 * t2;
}

#endif //A4_MATRIX_UTILS_HPP
