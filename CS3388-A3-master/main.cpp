//  module: CS 3388 Assignment 3
//  author: Zhen Yan
//    date: Thu, Mar 18, 2019
// purpose: render shaded 3D geometry

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <tuple>
#include <chrono>
#include <utility>

#include <SFML/Graphics.hpp>

#include "bresenham.hpp"
#include "matrix.hpp"
#include "light.hpp"

// prints out a matrix/vector, helps with debugging
template<typename T, size_t M, size_t N>
std::ostream &operator<<(std::ostream &os, const matrix<T, M, N> &m);

// draws a scene, applies perspective division right before drawing
void draw_scene(const std::vector<std::vector<vec4d>> &objects, sf::Image &image);

// makes edges (pairs of points) from a triangle
std::vector<std::pair<vec4d, vec4d>> edges_of(const std::vector<vec4d> &t);

// removes horizontal edges so that scanline fill doesn't intersect
std::vector<std::pair<vec4d, vec4d>> remove_horizontal_edges(const std::vector<std::pair<vec4d, vec4d>> &edges);

// find vertical range of a triangle, or the number of scanlines required to fill the triangle
std::pair<int, int> find_range(const std::vector<vec4d> &t);

// finds intersections between the edges at a scanline y
std::vector<int> find_intersections(const std::vector<std::pair<vec4d, vec4d>> &edges, int y);

// trims a value between a max and a min
double clamp(double v, double max, double min);

// computes the color for each triangle of a mesh
void compute_color(std::vector<mesh> &meshes, const light &light, const vec4d &eye);

// fills the triangles of a mesh with scanline algorithm
void fill_triangles(const std::vector<mesh> &meshes, sf::Image &image);

// shading is kind of a mix between flat and Phong
// no interpolation, but diffuse, ambient, and specular lighting is implemented
// When the faces are smaller than a pixel, it basically becomes Phong
int main()
{
	const size_t window_width = 1000, window_height = 600;
	sf::RenderWindow window(sf::VideoMode(window_width, window_height), "It's not a torus, it's actually a ball!");

	sf::Transform flip_y; // origin is upper left by default, y+ down. flipping to y+ up
	flip_y.scale(1, -1); // flip y axis
	flip_y.translate(0, -static_cast<float>(window_height)); // shift down 1 sq, origin is now bottom left

	sf::Image image; // colleciton of pixels. cannot be drawn directly by SFML
	image.create(window_width, window_height, sf::Color(0, 0, 0, 0)); // init to 100% transparent

	sf::Texture texture; // need a texture to make a sprite
	sf::Sprite sprite; // SFML can draw sprites

	double eyex = 0;
	double eyey = 300;
	double eyez = 300;

	auto view = rotx(M_PI / 4) * translate(-eyex, -eyey, -eyez) * scale(0.75);
	auto screen = translate(window_width / 2.0, window_height / 2.0, 0.0);

	const std::vector<vec4d> sq = {
		{{200.0, 200.0, 0.0, 1.0}},
		{{-200.0, 200.0, 0.0, 1.0}},
		{{-200.0, -200.0, 0.0, 1.0}},
		{{200.0, -200.0, -50.0, 1.0}},
	};

	light bulb{ {{0, 400, 400, 1.0}}, 1 };

	mesh sphere = make_sphere_mesh(200, 1000, 1000);
	sphere.color = vec4d{{255, 127, 0.0, 255.0}};

	for (auto &tri : sphere.faces) // transform the sphere
		tri.points = normalize_w(screen * view * translate(-300.0, 0.0, 0.0) * tri.points);
	
	mesh cone = make_cone_mesh(150, 250, 800, 1);
	cone.color = vec4d{{0.0, 127, 0.0, 255}};

	for (auto &tri : cone.faces) // transform the cone
		tri.points = normalize_w(screen * view * translate(100.0, 0.0, 0.0) * tri.points);

	std::vector<mesh> meshes;
	meshes.push_back(cone);
	meshes.push_back(sphere);

	compute_color(meshes, bulb, vec4d{{eyex, eyey, eyez, 1.0}});
	fill_triangles(meshes, image);

	window.clear(sf::Color::White);

	texture.loadFromImage(image); // convert to texture
	sprite.setTexture(texture); // convert to sprite
	window.draw(sprite, flip_y);
	window.display();

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
	}
	
	return 0;
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
			
			Bresenham(image, std::round(start.at(0, 0)), std::round(start.at(1, 0)), std::round(end.at(0, 0)), std::round(end.at(1, 0)));
		}
	}
}

std::vector<std::pair<vec4d, vec4d>> edges_of(const std::vector<vec4d> &t)
{
	std::vector<std::pair<vec4d, vec4d>> edges;

	return {{
		{t[0], t[1]},
		{t[1], t[2]},
		{t[2], t[0]},
	}};
}

std::vector<std::pair<vec4d, vec4d>> remove_horizontal_edges(const std::vector<std::pair<vec4d, vec4d>> &edges)
{
	std::vector<std::pair<vec4d, vec4d>> filtered;

	for (const auto &edge : edges)
	{
		auto dy = edge.second.at(1, 0) - edge.first.at(1, 0);

		if (dy == 0)
			continue;

		filtered.push_back(edge);
	}

	return filtered;
}

std::pair<int, int> find_range(const std::vector<vec4d> &t)
{
	return {
		std::round(std::max(std::max(t[0].at(1, 0), t[1].at(1, 0)), t[2].at(1, 0))),
		std::round(std::min(std::min(t[0].at(1, 0), t[1].at(1, 0)), t[2].at(1, 0)))
	};
}

std::vector<int> find_intersections(const std::vector<std::pair<vec4d, vec4d>> &edges, int y)
{
	std::vector<int> xs;
	
	for (const auto &edge : edges)
	{
		double y2 = edge.second.at(1, 0);
		double y1 = edge.first.at(1, 0);

		double dy = y2 - y1;
		
		double x2 = edge.second.at(0, 0);
		double x1 = edge.first.at(0, 0);

		double dx = x2 - x1;

		double t = (y - y1) / dy;

		double x = x1 + t * dx;

		if (1 >= t && t >= 0)
			xs.push_back(std::round(x));
	}

	return xs;
}

double clamp(double v, double max, double min)
{
	return std::max(std::min(v, max), min);
}

void compute_color(std::vector<mesh> &meshes, const light &light, const vec4d &eye)
{
	for (auto &mesh : meshes)
	{
		for (auto &face : mesh.faces)
		{
			double ambient = light.intensity;
			// ambient = 0;
			
			vec3d n = cart(face.normal());

			vec3d s = cart(light.position) - cart(face.center());
			double diffuse = light.intensity * std::max(0.0, dot(norm(s), n));
			// diffuse = 0;

			vec3d rvec = -s + n * 2 * (dot(s, n) / dot(n, n));
			vec3d v = -cart(eye) + cart(face.center());
			double specular = light.intensity * std::pow(std::max(0.0, dot(rvec, v) / (magnitude(rvec) * magnitude(v))), 50);

			double r = (diffuse + ambient + specular) * mesh.color.at(0, 0);
			double g = (diffuse + ambient + specular) * mesh.color.at(1, 0);
			double b = (diffuse + ambient + specular) * mesh.color.at(2, 0);

			face.color = vec4d{{r, g, b, 1.0}};
		}
	}
}

void fill_triangles(const std::vector<mesh> &meshes, sf::Image &image)
{
	for (const auto &mesh : meshes)
	{
		for (const auto &t : mesh.faces)
		{
			if (t.normal().at(2, 0) < 0)
				continue;
				
			auto edges = remove_horizontal_edges(edges_of(t.points));
			auto range = find_range(t.points);

			// std::cout << range.first << ", " << range.second << std::endl;
			for (int y = range.first; y >= range.second; --y)
			{
				auto intersections = find_intersections(edges, y);

				if (intersections.size() == 0)
					continue;
				
				auto start = std::min(intersections[0], intersections[1]);
				auto end = std::max(intersections[0], intersections[1]);
				
				// TODO check bounds
				for (int x = start; x <= end; ++x)
				{
					uint8_t r = std::round(clamp(t.color.at(0, 0), 255, 0));
					uint8_t g = std::round(clamp(t.color.at(1, 0), 255, 0));
					uint8_t b = std::round(clamp(t.color.at(2, 0), 255, 0));

					image.setPixel(x, y, sf::Color{r, g, b, 255});
				}
			}
		}
	}
}
