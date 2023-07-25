#include "geom.hpp"

// turns a list of verts [a, b, c, ...] into
// [a, b, b, c, c, d ...]
// such that every pair (a, b), (b, c) ... are the endpoints of a line
std::vector<vec4d> to_edges(const std::vector<vec4d> &points)
{
	std::vector<vec4d> edges;

	for (size_t i = 1; i < points.size(); ++i)
	{
		edges.push_back(points[i]);
		edges.push_back(points[i - 1]);
	}

	return edges;
}

// makes a list of vertices r away from the center, evenly distributed, on the xy plane
// segments are the number of segments the circumference of the circle is divided into
// more segments make a rounder circle
// the first point is repeated as the last element so that when it's made into edges,
// the last line segment completing the circle will be made
// points to a "circle" of 4 segments: [a, b, c, d, a]
// to_edges: [a, b,   b, c,   c, d,   d, a], 4 segments
// whereas [a, b, c, d] yields [a, b,   b, c,   c, d] and there's an open gap
std::vector<vec4d> make_circle(double radius, size_t segments)
{
	std::vector<vec4d> points;

	for (size_t i = 0; i <= segments; ++i)
	{
		double x = radius * std::cos(i * (2 * M_PI / segments));
		double y = radius * std::sin(i * (2 * M_PI / segments));

		points.push_back({{x, y, 0, 1}});
	}

	return to_edges(points);
}

// makes a collection of edges for a wireframe of a torus, can be drawn directly
// r_torus is the radius of the ring
// r_tube is the radius of the tube that runs latitudinally
// more segments make the object smoother
std::vector<vec4d> make_torus(double r_torus, double r_tube, size_t torus_segments, size_t tube_segments)
{
	std::vector<vec4d> edges;

	// longitudinal circle
	auto long_circle = translate(r_torus, 0.0, 0.0) * make_circle(r_tube, tube_segments);

	// spin it around to make the donut
	const double torus_div = 2 * M_PI / torus_segments;
	for (size_t i = 0; i < torus_segments; ++i)
	{
		auto slice = roty(i * torus_div) * long_circle;
		edges.insert(edges.end(), slice.begin(), slice.end());
	}

	// make latitude circles
	const double tube_div = 2 * M_PI / tube_segments;
	for (size_t i = 0; i < tube_segments; ++i)
	{
		double radius = r_torus + r_tube * std::cos(i * tube_div);
		double vert_offset = r_tube * std::sin(i * tube_div);

		auto lat_circle = translate(0.0, vert_offset, 0.0) * rotx(M_PI / 2) * make_circle(radius, torus_segments);
		
		edges.insert(edges.begin(), lat_circle.begin(), lat_circle.end());
	}

	return edges;
}

// makes edges for a "sphere". found out that some torus are spheres, so I'm using that
// too lazy to write out a dedicated method
std::vector<vec4d> make_sphere(double r, size_t lat_segments, size_t long_segments)
{
	return make_torus(0, r, lat_segments, long_segments);
}

// r is radius of the base
// height is the height of the cone
// more segments make the cone smoother
std::vector<vec4d> make_cone(double r, double height, size_t base_segments)
{
	// make circle on the xy plane, rotate around x to lie on the xz plane
	auto base_circle = rotx(M_PI / 2) * make_circle(r, base_segments);

	std::vector<vec4d> edges{base_circle}; // add the base just made

	// wedge shape, vertical profile of the cone
	auto wall = to_edges({
		{{0, height, 0, 1}}, // tip
		{{r, 0, 0, 1}}, // side of the base
		{{0, 0, 0, 1}} // center of the base
	});

	// spin the wedge around and duplicate to make a cone
	for (size_t i = 0; i < base_segments; ++i)
	{
		auto rot = roty(i * 2 * M_PI / base_segments);
		auto slice = rot * wall;

		edges.insert(edges.end(), slice.begin(), slice.end());
	}

	return edges;
}

vec4d make_sphere_pt(double radius, double theta, double phi)
{
	return {{
		radius * std::cos(theta) * std::sin(phi),
		radius * std::sin(theta) * std::sin(phi),
		radius * std::cos(phi),
		1.0
	}};
}

mesh make_sphere_mesh(double radius, size_t lat_divs, size_t long_divs)
{
	std::vector<triangle> faces;

	const double d_phi = M_PI / long_divs;
	const double d_theta = 2 * M_PI / lat_divs;

	// top
	for (double theta = 0; theta < 2 * M_PI; theta += d_theta)
	{
		triangle t;
		t.points = {
			{{0.0, 0.0, radius, 1.0}},
			make_sphere_pt(radius, theta, d_phi),
			make_sphere_pt(radius, theta + d_theta, d_phi)
		};
		
		faces.push_back(t);
	}

	// middle
	for (double theta = 0; theta < 2 * M_PI; theta += d_theta)
	{
			for (double phi = d_phi; phi < M_PI - d_phi; phi += d_phi)
			{
			std::array<vec4d, 4> quad{
				make_sphere_pt(radius, theta, phi), // top left
				make_sphere_pt(radius, theta, phi + d_phi), // bottom left
				make_sphere_pt(radius, theta + d_theta, phi + d_phi), // bottom right
				make_sphere_pt(radius, theta + d_theta, phi), // top right
			};

			triangle t1;
			t1.points = {quad[0], quad[1], quad[2]};
			faces.push_back(t1);

			triangle t2;
			t2.points = {quad[2], quad[3], quad[0]};
			faces.push_back(t2);
			}
	}

	// bottom
	for (double theta = 0; theta < 2 * M_PI; theta += d_theta)
	{
		triangle t;
		t.points = {
			{{0.0, 0.0, -radius, 1.0}},
			make_sphere_pt(radius, theta + d_theta, M_PI - d_phi),
			make_sphere_pt(radius, theta, M_PI - d_phi),
		};

		faces.push_back(t);
	}

	return mesh{faces, {}};
}

double cone_radius(double base_radius, double height, double h)
{
	return base_radius - h * base_radius / height;
}

mesh make_cone_mesh(double radius, double height, size_t lat_divs, size_t long_divs)
{
	const double d_theta = 2 * M_PI / lat_divs;
	const double dy = height / long_divs;

	std::vector<triangle> faces;

	// tip
	for (double theta = 0; theta < 2 * M_PI; theta += d_theta)
	{
		double r = cone_radius(radius, height, height - dy);

		triangle t;
		t.points = {
			{{0.0, height, 0.0, 1.0}}, // top
			{{r * std::sin(theta), height - dy, r * std::cos(theta), 1.0}}, // base left
			{{r * std::sin(theta + d_theta), height - dy, r * std::cos(theta + d_theta), 1.0}}, // base right
		};

		faces.push_back(t);
	}

	// body
	for (double theta = 0; theta < 2 * M_PI; theta += d_theta)
	{
		for (double h = height - dy; h > dy; h -= dy)
		{
			double r1 = cone_radius(radius, h, dy);
			double r2 = cone_radius(radius, h - dy, dy);
			
			std::vector<vec4d> quad{
				{{r1 * std::sin(theta), h, r1 * std::sin(theta), 1.0}}, // top left
				{{r2 * std::sin(theta), h - dy, r2 * std::sin(theta), 1.0}}, // down
				{{r2 * std::sin(theta + d_theta), h - dy, r2 * std::sin(theta + d_theta), 1.0}}, // right
				{{r1 * std::sin(theta + d_theta), h, r1 * std::sin(theta + d_theta), 1.0}}, // top right
			};

			triangle t1;
			t1.points = {quad[0], quad[1], quad[2]};
			triangle t2;
			t2.points = {quad[2], quad[3], quad[0]};
			faces.push_back(t1);
			faces.push_back(t2);
		}
	}

	return mesh{faces, {}};
}
