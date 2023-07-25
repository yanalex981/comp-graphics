//  module: CS 3388 Assignment 1
//  author: Zhen Yan
//    date: Mon, Jan 21, 2019
// purpose: draw lines using Bresenham's algorithm, display graphics using the SFML library

#include <iostream>
#include <cmath>
#include <vector>

#include <SFML/Graphics.hpp>

// 2D point, simple alias is enough
template<typename T>
using point = std::pair<T, T>;

// alias for list of 2D point, to avoid typing
template<typename T>
using vpoints = std::vector<point<int>>;

void draw_compare(sf::Image &image);

// computes a list of pixels that form a line using Bresenham's algorithm, where endpoints are p1 and p2
// start is p1: (x1, y1), end is p2: (x2, y2)
// post: output size is between [1, max(dx, dy) + 1] px, all coords are unique, contiguous (no more than 1 px away, list is in order from end to end
vpoints<int> Bresenham(int x1, int y1, int x2, int y2);

// draw a line from p1 to p2 on an sfml image using Bresenham(int, int, int, int)
void Bresenham(sf::Image &image, int x1, int y1, int x2, int y2);

// draws Steve's spiral loop for comparison
void draw_test_frag(sf::Image &image);

int main()
{
    const size_t window_size = 512;
    sf::RenderWindow window(sf::VideoMode(window_size, window_size), "It's a bird, no, it's a plane!");

    sf::Image image; // colleciton of pixels. cannot be drawn directly by SFML
    image.create(window_size, window_size, sf::Color(0, 0, 0, 0)); // init to 100% transparent
//    draw_test_frag(image);
    draw_compare(image);
    
//    Bresenham(image, 0, 0, 256, 256);
//    Bresenham(image, 0, 200, 200, 500);
//    Bresenham(image, 200, 300, 0, 0);

    sf::Transform flip_y; // origin is upper left by default, y+ down. flipping to y+ up, origin bottom left
    flip_y.scale(1, -1); // flip y axis
    flip_y.translate(0, -static_cast<float>(window_size)); // shift down 1 sq
    
    sf::Sprite sprite; // SFML can draw sprites directly
    sf::Texture texture; // need a texture to make a sprite
    texture.loadFromImage(image); // convert to texture
    sprite.setTexture(texture); // convert to sprite
    
    window.clear(sf::Color::White); // init background to white as per assignments
    window.draw(sprite, flip_y); // draw only once
    window.display(); // swap buffer?
    
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

vpoints<int> line(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;
    
    vpoints<int> points;
    
    for(;;){
//        setPixel(x0,y0);
        points.push_back({x0, y0});
        if (x0==x1 && y0==y1) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
    
    return points;
}

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

void other_draw(sf::Image &image, int x1, int y1, int x2, int y2)
{
    auto points = line(x1, y1, x2, y2);
    
    for (auto &point : points)
        image.setPixel(point.first, point.second, sf::Color(0, 0, 0, 255));
}

void draw_compare(sf::Image &image)
{
    auto dt = 2.0 * M_PI / 200.0 ;
    for (auto t = 0.0; t < 2.0 * M_PI;) {
        auto x1 = 256 + (int)100.0*(1.5*cos(t) - cos(13.0*t));
        auto y1 = 256 + (int)100.0*(1.5*sin(t) - sin(13.0*t));
        t += dt ;
        auto x2 = 256 + (int)100.0*(1.5*cos(t) - cos(13.0*t));
        auto y2 = 256 + (int)100.0*(1.5*sin(t) - sin(13.0*t));
        other_draw(image, x1, y1, x2, y2);
    }
}

// draw a line from p1 to p2 on an sfml image using Bresenham(int, int, int, int)
void Bresenham(sf::Image &image, int x1, int y1, int x2, int y2)
{
    auto points = Bresenham(x1, y1, x2, y2);
    
    for (auto &point : points)
        image.setPixel(point.first, point.second, sf::Color::Black);
}

// draws Steve's spiral loop for comparison
void draw_test_frag(sf::Image &image)
{
    auto dt = 2.0 * M_PI / 200.0 ;
    for (auto t = 0.0; t < 2.0 * M_PI;) {
        auto x1 = 256 + (int)100.0*(1.5*cos(t) - cos(13.0*t));
        auto y1 = 256 + (int)100.0*(1.5*sin(t) - sin(13.0*t));
        t += dt ;
        auto x2 = 256 + (int)100.0*(1.5*cos(t) - cos(13.0*t));
        auto y2 = 256 + (int)100.0*(1.5*sin(t) - sin(13.0*t));
        Bresenham(image, x1, y1, x2, y2);
    }
}
