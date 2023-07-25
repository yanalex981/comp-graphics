#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <array>
#include <tuple>
#include <cmath>

// statically dimensioned matrix class
// M is the rows
// N is the cols
// column major
template<typename T, size_t M, size_t N>
class matrix
{
	std::array<std::array<T, M>, N> columns{};

public:
	constexpr static auto row_size = M;
	constexpr static auto col_size = N;

	matrix() = default;
	matrix(std::array<std::array<T, M>, N> cols);

	T &at(size_t m, size_t n);
	const T &at(size_t m, size_t n) const;

	matrix<T, N, M> transpose() const;

	template<size_t K>
	matrix<T, M, K> operator*(const matrix<T, N, K> &other) const;
	matrix<T, M, N> operator*(T k) const;
	matrix<T, M, N> operator/(T k) const;
	matrix<T, M, N> operator+(const matrix<T, M, N> &other) const;
	matrix<T, M, N> operator-(const matrix<T, M, N> &other) const;
	matrix<T, M, N> operator-() const;
};

using mat4d = matrix<double, 4, 4>;

template<typename T, size_t M, size_t N>
matrix<T, M, N>::matrix(std::array<std::array<T, M>, N> cols) :
	columns{cols}
{}

template<typename T, size_t M, size_t N>
T &matrix<T, M, N>::at(size_t row, size_t col)
{
	return columns[col][row];
}

template<typename T, size_t M, size_t N>
const T &matrix<T, M, N>::at(size_t row, size_t col) const
{
	return columns[col][row];
}

template<typename T, size_t M, size_t N>
matrix<T, N, M> matrix<T, M, N>::transpose() const
{
	matrix<T, N, M> result;

	for (size_t i = 0; i < N; ++i)
		for (size_t j = 0; j < M; ++j)
			result.at(i, j) = this->at(j, i);

	return result;
}

// multiply by matrix
template<typename T, size_t M, size_t N>
template<size_t K>
matrix<T, M, K> matrix<T, M, N>::operator*(const matrix<T, N, K> &other) const
{
	matrix<T, M, K> result;

	for (size_t k = 0; k < K; ++k) // col
		for (size_t m = 0; m < M; ++m) // row
			for (size_t n = 0; n < N; ++n)
				result.at(m, k) += this->at(m, n) * other.at(n, k);

	return result;
}

// multiply by a constant
template<typename T, size_t M, size_t N>
matrix<T, M, N> matrix<T, M, N>::operator*(T k) const
{
	matrix<T, M, N> result;

	for (size_t m = 0; m < M; ++m)
		for (size_t n = 0; n < N; ++n)
			result.at(m, n) = k * this->at(m, n);

	return result;
}

// division by constant
template<typename T, size_t M, size_t N>
matrix<T, M, N> matrix<T, M, N>::operator/(T k) const
{
	return *this * (1 / k);
}

// matrix addition
template<typename T, size_t M, size_t N>
matrix<T, M, N> matrix<T, M, N>::operator+(const matrix<T, M, N> &other) const
{
	matrix<T, M, N> result;

	for (size_t m = 0; m < M; ++m)
		for (size_t n = 0; n < N; ++n)
			result.at(m, n) = this->at(m, n) + other.at(m, n);

	return result;
}

// matrix subtraction
template<typename T, size_t M, size_t N>
matrix<T, M, N> matrix<T, M, N>::operator-(const matrix<T, M, N> &other) const
{
	return *this + (-other);
}

// matrix negation
template<typename T, size_t M, size_t N>
matrix<T, M, N> matrix<T, M, N>::operator-() const
{
	matrix<T, M, N> mat;

	for (size_t m = 0; m < M; ++m)
		for (size_t n = 0; n < N; ++n)
			mat.at(m, n) = -this->at(m, n);
	
	return mat;
}

// returns a square identity matrix
template<size_t N = 4, typename T = double>
matrix<T, N, N> identity()
{
	matrix<T, N, N> m;

	for (size_t i = 0; i < N; ++i)
		m.at(i, i) = 1;

	return m;
}

// generates a 4x4 homogeneous translation matrix
template<typename T = double>
matrix<T, 4, 4> translate(T x, T y, T z)
{
	auto m = identity();

	m.at(0, 3) = x;
	m.at(1, 3) = y;
	m.at(2, 3) = z;

	return m;
}

// generates a 4x4 homogeneous scaling matrix
template<typename T = double>
matrix<T, 4, 4> scale(T x, T y, T z)
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
matrix<T, 4, 4> scale(T k)
{
	auto m = identity();
	m.at(3, 3) = 1 / k;

	// return scale(k, k, k);
	return m;
}

// generates a 4x4 homogeneous matrix to rotate by the x axis
template<typename T = double>
matrix<T, 4, 4> rotx(T rad)
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
matrix<T, 4, 4> roty(T rad)
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
matrix<T, 4, 4> rotz(T rad)
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
matrix<T, 4, 4> rotate(T xrad, T yrad, T zrad)
{
	return rotx(xrad) * roty(yrad) * rotz(zrad);
}

#endif
