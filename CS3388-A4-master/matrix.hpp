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

	constexpr matrix() = default;
	constexpr matrix(std::array<std::array<T, M>, N> cols);

	template<size_t R>
	constexpr matrix<T, M, N> &set_col(const matrix<T, 1, N> &col);
	template<size_t R>
	constexpr matrix<T, 1, N> get_col() const;

	constexpr T &at(size_t m, size_t n);
	constexpr const T &at(size_t m, size_t n) const;

	constexpr matrix<T, N, M> transpose() const;

	template<size_t K>
	constexpr matrix<T, M, K> operator*(const matrix<T, N, K> &other) const;
	constexpr matrix<T, M, N> operator*(T k) const;
	constexpr matrix<T, M, N> operator/(T k) const;
	constexpr matrix<T, M, N> operator+(const matrix<T, M, N> &other) const;
	constexpr matrix<T, M, N> operator-(const matrix<T, M, N> &other) const;
	constexpr matrix<T, M, N> operator-() const;

	constexpr T &x();
	constexpr T &y();
	constexpr T &z();
	constexpr T &w();

	constexpr const T &x() const;
	constexpr const T &y() const;
	constexpr const T &z() const;
	constexpr const T &w() const;
};

using mat4d = matrix<double, 4, 4>;

template<typename T, size_t M, size_t N>
constexpr matrix<T, M, N>::matrix(std::array<std::array<T, M>, N> cols) :
	columns{cols}
{}

template<typename T, size_t M, size_t N>
constexpr T &matrix<T, M, N>::at(size_t row, size_t col)
{
	return columns[col][row];
}

template<typename T, size_t M, size_t N>
constexpr const T &matrix<T, M, N>::at(size_t row, size_t col) const
{
	return columns[col][row];
}

template<typename T, size_t M, size_t N>
constexpr matrix<T, N, M> matrix<T, M, N>::transpose() const
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
constexpr matrix<T, M, K> matrix<T, M, N>::operator*(const matrix<T, N, K> &other) const
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
constexpr matrix<T, M, N> matrix<T, M, N>::operator*(T k) const
{
	matrix<T, M, N> result;

	for (size_t m = 0; m < M; ++m)
		for (size_t n = 0; n < N; ++n)
			result.at(m, n) = k * this->at(m, n);

	return result;
}

// division by constant
template<typename T, size_t M, size_t N>
constexpr matrix<T, M, N> matrix<T, M, N>::operator/(T k) const
{
	return *this * (1 / k);
}

// matrix addition
template<typename T, size_t M, size_t N>
constexpr matrix<T, M, N> matrix<T, M, N>::operator+(const matrix<T, M, N> &other) const
{
	matrix<T, M, N> result;

	for (size_t m = 0; m < M; ++m)
		for (size_t n = 0; n < N; ++n)
			result.at(m, n) = this->at(m, n) + other.at(m, n);

	return result;
}

// matrix subtraction
template<typename T, size_t M, size_t N>
constexpr matrix<T, M, N> matrix<T, M, N>::operator-(const matrix<T, M, N> &other) const
{
	return *this + (-other);
}

// matrix negation
template<typename T, size_t M, size_t N>
constexpr matrix<T, M, N> matrix<T, M, N>::operator-() const
{
	matrix<T, M, N> mat;

	for (size_t m = 0; m < M; ++m)
		for (size_t n = 0; n < N; ++n)
			mat.at(m, n) = -this->at(m, n);
	
	return mat;
}

template<typename T, size_t M, size_t N>
constexpr T &matrix<T, M, N>::x()
{
	static_assert((M == 1 && N >= 1) || (N == 1 && M >= 1), "Needs to be a row/col vector >= 1D");

	if (M == 1)
		return at(0, 0);

	return at(0, 0);
}

template<typename T, size_t M, size_t N>
constexpr T &matrix<T, M, N>::y()
{
	static_assert((M == 1 && N >= 2) || (N == 1 && M >= 2), "Needs to be a row/col vector >= 2D");

	if (M == 1)
		return at(0, 1);

	return at(1, 0);
}

template<typename T, size_t M, size_t N>
constexpr T &matrix<T, M, N>::z()
{
	static_assert((M == 1 && N >= 3) || (N == 1 && M >= 3), "Needs to be a row/col vector >= 3D");

	if (M == 1)
		return at(0, 2);

	return at(2, 0);
}

template<typename T, size_t M, size_t N>
constexpr T &matrix<T, M, N>::w()
{
	static_assert((M == 1 && N >= 4) || (N == 1 && M >= 4), "Needs to be a row/col vector >= 4D");

	if (M == 1)
		return at(0, 3);

	return at(3, 0);
}

template<typename T, size_t M, size_t N>
constexpr const T &matrix<T, M, N>::x() const
{
	static_assert((M == 1 && N >= 1) || (N == 1 && M >= 1), "Needs to be a row/col vector >= 1D");

	return at(0, 0);
}

template<typename T, size_t M, size_t N>
constexpr const T &matrix<T, M, N>::y() const
{
	static_assert((M == 1 && N >= 2) || (N == 1 && M >= 2), "Needs to be a row/col vector >= 2D");

	if (M == 1)
		return at(0, 1);

	return at(1, 0);
}

template<typename T, size_t M, size_t N>
constexpr const T &matrix<T, M, N>::z() const
{
	static_assert((M == 1 && N >= 3) || (N == 1 && M >= 3), "Needs to be a row/col vector >= 3D");

	if (M == 1)
		return at(0, 2);

	return at(2, 0);
}

template<typename T, size_t M, size_t N>
constexpr const T &matrix<T, M, N>::w() const
{
	static_assert((M == 1 && N >= 4) || (N == 1 && M >= 4), "Needs to be a row/col vector >= 4D");

	if (M == 1)
		return at(0, 3);

	return at(3, 0);
}

#endif
