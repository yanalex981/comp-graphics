//  module: CS 3388 Assignment 2
//  author: Zhen Yan
//    date: Sun, Feb 24, 2019
// purpose: render 3D geometry

#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>
#include <chrono>

#include <SFML/Graphics.hpp>

#include "bresenham.hpp"
#include "matrix.hpp"

void draw_test_frag(sf::Image &image);

// prints out a matrix/vector, helps with debugging
template<typename T, size_t M, size_t N>
std::ostream &operator<<(std::ostream &os, const matrix<T, M, N> &m);

// draws a scene, applies perspective division right before drawing
void draw_scene(const std::vector<std::vector<vec4d>> &objects, sf::Image &image);

int main()
{
	const size_t window_width = 1000, window_height = 600;
	sf::RenderWindow window(sf::VideoMode(window_width, window_height), "It's a ball, no, it's a torus!");

	std::vector<std::vector<vec4d>> scene{
		translate(0.0, 0.0, 200.0) * make_torus(160, 60, 48, 32), // make a torus, place it in (0, 0, 200)
		translate(200.0, 0.0, -200.0) * make_torus(0, 200, 48, 48), // make a ball, place it in (200, 0, -200)
		translate(-200.0, 0.0, -200.0) * make_cone(200, 400, 32) // make a cone
	};

	sf::Transform flip_y; // origin is upper left by default, y+ down. flipping to y+ up
	flip_y.scale(1, -1); // flip y axis
	flip_y.translate(0, -static_cast<float>(window_height)); // shift down 1 sq, origin is now bottom left

	sf::Image image; // colleciton of pixels. cannot be drawn directly by SFML
	sf::Texture texture; // need a texture to make a sprite
	sf::Sprite sprite; // SFML can draw sprites

	auto view = rotx(M_PI / 4) * translate(0.0, 0.0, 0.0) * scale(0.75);
	auto screen = translate(window_width / 2.0, window_height / 2.0, 0.0);

	auto prog_start = std::chrono::high_resolution_clock::now();

	while (window.isOpen()) // poll for input while window is open
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			// if close button, or Q button pressed, close window gracefully
			if (event.type == sf::Event::Closed ||
			   (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Q))
				window.close();
		}

		// auto draw_start = std::chrono::high_resolution_clock::now(); // benchmarking, I'm curious how -mavx changes things
		window.clear(sf::Color::White);
		image.create(window_width, window_height, sf::Color(0, 0, 0, 0)); // init to 100% transparent

		auto current_time = std::chrono::high_resolution_clock::now();
		double angle = 0.5 * std::chrono::duration_cast<std::chrono::duration<double>>(current_time - prog_start).count();

		std::vector<std::vector<vec4d>> transformed;
		for (auto &obj : scene)
			transformed.push_back(screen * view * roty(angle) * obj);
		draw_scene(transformed, image);

		texture.loadFromImage(image); // convert to texture
		sprite.setTexture(texture); // convert to sprite
		window.draw(sprite, flip_y);
		window.display();
		// auto draw_end = std::chrono::high_resolution_clock::now();
		// std::cout << std::chrono::duration_cast<std::chrono::microseconds>(draw_end - draw_start).count() << std::endl;
	}
	
	return 0;
}

// draws Steve's spiral loop for comparison
void draw_test_frag(sf::Image &image)
{
	auto dt = 2.0 * M_PI / 200.0;
	for (auto t = 0.0; t < 2.0 * M_PI;) {
		auto x1 = 256 + (int)100.0*(1.5*cos(t) - cos(13.0*t));
		auto y1 = 256 + (int)100.0*(1.5*sin(t) - sin(13.0*t));
		t += dt ;
		auto x2 = 256 + (int)100.0*(1.5*cos(t) - cos(13.0*t));
		auto y2 = 256 + (int)100.0*(1.5*sin(t) - sin(13.0*t));
		Bresenham(image, x1, y1, x2, y2);
	}
}

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

void draw_scene(const std::vector<std::vector<vec4d>> &objects, sf::Image &image)
{
	for (auto &lines : objects)
	{
		for (size_t i = 1; i < lines.size(); i += 2)
		{
			auto start = normalize_w(lines[i - 1]);
			auto end = normalize_w(lines[i]);
			
			Bresenham(image, start.at(0, 0), start.at(1, 0), end.at(0, 0), end.at(1, 0));
		}
	}
}
