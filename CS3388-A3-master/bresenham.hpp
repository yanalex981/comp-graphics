#ifndef BRESENHAM_HPP
#define BRESENHAM_HPP

#include <iostream>

#include <SFML/Graphics.hpp>

#include "geom.hpp"

// computes a list of pixels that form a line using Bresenham's algorithm, where endpoints are p1 and p2
// start is p1: (x1, y1), end is p2: (x2, y2)
// post: output size is between [1, max(dx, dy) + 1] px, all coords are unique, contiguous (no more than 1 px away, list is in order from end to end
vpoints<int> Bresenham(int x1, int y1, int x2, int y2)
{
	auto dx = x2 - x1;
	if (x2 < x1) // if p2 is to the left of p1, swap p1 and p2, narrow down to quadrants I & IV
		return Bresenham(x2, y2, x1, y1);
	
	auto dy = y2 - y1;
	// if too steep, reflect y=x to graph in terms of f(y), then reflect x&y of each resulting pixel
	// narrows down to lines that are +/- 45 deg
	if (std::abs(dy) > dx)
	{
		auto flipped = Bresenham(y1, x1, y2, x2);
		for (auto &point : flipped)
			std::swap(point.first, point.second);
		
		return flipped;
	}
	
	// if line slopes down, draw it sloping up, reflect about y=y1, reflect back when done
	// narrows down to only +45 deg
	if (dy < 0)
	{
		auto mirrored = Bresenham(x1, y1, x2, y1 - dy);
		for (auto &point : mirrored)
		{
			auto d = point.second - y1; // pixel's y-dist from y1
			point.second = y1 - d; // sink it that far below y1
		}
		
		return mirrored; // should be sloping down now
	}
	
	vpoints<int> points;
	points.reserve(dx + 1);
	
	std::cout << '(' << x1 << ", " << y1 << ") -> (" << x2 << ", " << y2 << ')' << std::endl;

	auto d_error = 0;
	for (int x = x1, y = y1; x <= x2; ++x)
	{
		points.push_back({x, y});
		d_error += dy; // for each px, accumulate error, error += dy/dx

		if (2 * d_error >= dx) // if error > dx/2, then the line has left the current pixel, and entered the one above
		{
			++y; // pixel moves to the adjacent above
			d_error -= dx; // "reset" the error
		}
	}
	
	return points;
}

// draw a line from p1 to p2 on an sfml image using Bresenham(int, int, int, int)
void Bresenham(sf::Image &image, int x1, int y1, int x2, int y2)
{
	auto points = Bresenham(x1, y1, x2, y2);
	
	for (auto &point : points)
	{
		auto size = image.getSize();
		auto width = size.x;
		auto height = size.y;

		if (point.first < 0 || static_cast<size_t>(point.first) >= width) continue;
		if (point.second < 0 || static_cast<size_t>(point.second) >= height) continue;
		
		image.setPixel(point.first, point.second, sf::Color::Black);
	}
}

#endif
