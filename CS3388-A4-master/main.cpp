//  module: CS 3388 Assignment 4
//  author: Zhen Yan
//    date: Fri, Apr 5, 2019
// purpose: ray trace a scene of generic objects

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <utility>
#include <limits>

#include <SFML/Graphics.hpp>

#include "matrix.hpp"
#include "vector.hpp"
#include "matrix_utils.hpp"
#include "light.hpp"
#include "plane.hpp"
#include "cone.hpp"
#include "sphere.hpp"

// trims a value between a max and a min
double clamp(double v, double max, double min);

template<typename T, size_t N>
vec<T, N> clamp(const vec<T, N> &v, double max, double min);

// find intersections in a scene given a ray
template<typename C>
std::optional<hit> find_intersection(const C &scene, const vec4d &ray_start, const vec4d &ray_end)
{
	std::vector<hit> hits;

	// get the intersections of this ray and every object in the scene
	for (auto &obj : scene)
	{
		auto intersection = obj->intersect(ray_start, ray_end);

		if (intersection)
			hits.push_back(intersection.value());
	}

	if (hits.size() < 1)
		return {};

	// return the shortest
	return *std::min_element(hits.begin(), hits.end(), [&](const hit &a, const hit &b)
	{
		auto ra = a.world_pt - cart(ray_start);
		auto rb = b.world_pt - cart(ray_start);

		return dot(ra, ra) < dot(rb, rb);
	});
}

int main()
{
	const size_t window_width = 1000, window_height = 600;
	sf::RenderWindow window(sf::VideoMode(window_width, window_height), "pew pew pew");

	sf::Image image; // colleciton of pixels. cannot be drawn directly by SFML

	sf::Texture texture; // need a texture to make a sprite
	sf::Sprite sprite; // SFML can draw sprites

	// camera params
	auto eye = vec3d{{ 0, 40, 80 }};
	auto gaze = vec3d{{ 0, 0, 0 }};
	auto up = vec3d{{ 0, 1, 0 }};

	// window, camera, perspective matrices
	auto ms = screen( 1.0 * window_width, 1.0 * window_height );
	auto mp = perspective( 10.0, 1.0, -1.0, 1.0, 0.6, -0.6 );
	auto mc = camera<double>( eye, gaze, up );
	auto mvp = ms * mp * mc;
	auto inv = invert(mvp);

	light bulb{ {{ 40.0, 80.0, 0.0, 1.0 }}, 1.0 };

	sphere ball;
	ball.transforms = translate(-20.0, 20.0, 0.0) * scale(20.0);
	ball.material = {
		.color = vec3d{{ 255, 150, 0 }},
		.k_ambient = 0.08,
		.k_diffuse = 1,
		.k_specular = 255,
		.fallout = 256
	};

	plane ground;
	ground.transforms = scale(100.0) * rotx(-M_PI / 2);
	ground.material = {
		.color = vec3d{{ 180, 180, 180 }},
		.k_ambient = 0.1,
		.k_diffuse = 1,
		.k_specular = 255,
		.fallout = 256
	};

	cone dunce;
	dunce.transforms = translate(40.0, 0.0, 0.0) * scale(20.0) * scale(1.0, 2.0, 1.0);
	dunce.material = {
		.color = vec3d{{ 0, 180, 180 }},
		.k_ambient = 0.12,
		.k_diffuse = 1,
		.k_specular = 255,
		.fallout = 256
	};

	std::array<surface *, 3> scene{{ &dunce, &ground, &ball }};

	image.create(window_width, window_height, sf::Color(0, 0, 0, 0)); // init to 100% transparent
	window.clear(sf::Color::White);

	// trace those rays!
	for (size_t x = 0; x < window_width; ++x)
	{
		for (size_t y = 0; y < window_height; ++y)
		{
			// screen space
			vec4d ray_end{{ 1.0 * x, 1.0 * y, 1, 1 }};

			// world space
			auto ray_start_w = homo(eye);
			auto ray_end_w = inv * ray_end;

			// find intersection
			auto intersection = find_intersection(scene, ray_start_w, ray_end_w);
			if (!intersection)
				continue;

			auto hit = intersection.value();

			// lighting computation
			double ambient = hit.obj->material.k_ambient;

			vec3d s = norm(cart(bulb.position) - hit.world_pt);
			double diffuse = bulb.intensity * hit.obj->material.k_diffuse * std::max(0.0, dot(s, hit.normal));

			vec3d ref = -s + hit.normal * 2 * dot(s, hit.normal);
			vec3d v = norm(eye - hit.world_pt);

			double specular = bulb.intensity * hit.obj->material.k_specular * std::pow(std::max(0.0, dot(ref, v)), hit.obj->material.fallout);

			auto r = static_cast<uint8_t>(clamp(std::round(diffuse * hit.obj->material.color.x() + ambient + specular), 255.0, 0.0));
			auto g = static_cast<uint8_t>(clamp(std::round(diffuse * hit.obj->material.color.y() + ambient + specular), 255.0, 0.0));
			auto b = static_cast<uint8_t>(clamp(std::round(diffuse * hit.obj->material.color.z() + ambient + specular), 255.0, 0.0));

			// trace those shadows!
			auto obstruction = find_intersection(scene, bulb.position, homo(hit.world_pt));
			if (obstruction)
			{
				auto pt = obstruction.value();
				auto d = hit.world_pt - pt.world_pt;
				if (dot(d, d) > std::numeric_limits<double>::epsilon())
				{
					// obstructed
					r = ambient * hit.obj->material.color.x();
					g = ambient * hit.obj->material.color.y();
					b = ambient * hit.obj->material.color.z();
				}
			}

			// for debugging
			// how that colorful sphere was made, I was checking its normals
//		    uint8_t r = static_cast<uint8_t>(clamp(hit.normal.x() * 255, 255, 0));
//		    uint8_t g = static_cast<uint8_t>(clamp(hit.normal.y() * 255, 255, 0));
//		    uint8_t b = static_cast<uint8_t>(clamp(hit.normal.z() * 255, 255, 0));

			image.setPixel(x, y, sf::Color{ r, g, b, 255 });
		}
	}

	texture.loadFromImage(image); // convert to texture
	sprite.setTexture(texture); // convert to sprite
	window.draw(sprite);
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

double clamp(double v, double max, double min)
{
	return std::max(std::min(v, max), min);
}

template<typename T, size_t N>
vec<T, N> clamp(const vec<T, N> &v, double max, double min)
{
	vec<T, N> result;

	for (size_t i = 0; i < N; ++i)
		result.at(i, 0) = clamp(v.at(i, 0), max, min);

	return result;
}
