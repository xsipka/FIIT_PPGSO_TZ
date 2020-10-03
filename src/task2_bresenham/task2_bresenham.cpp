// Task 3 - Implement Bresenham drawing alg.
//        - Draw a star using lines
//        - Make use of std::vector to define the shape
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>

#include <ppgso/ppgso.h>


#define _USE_MATH_DEFINES

// Size of the framebuffer
const unsigned int SIZE = 512;

struct Point {
    int x;
    int y;
};


// Bresenham drawing algorithm
void bresenham(ppgso::Image& framebuffer, int x1, int x2, int y1, int y2, bool check) {
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;
    int shift = 1;
    int y = y1;
    int error = 2 * delta_y - delta_x;

    // Checks if line is increasing or decreasing
    if (delta_y < 0) {
        shift = -1;
        delta_y *= -1;
    }
    for (int x = x1; x <= x2; x++) {
        // If bool check is false, then X and Y are reversed
        if (check) {
            framebuffer.setPixel(x, y, 32, 180, 237);
        } else {
            framebuffer.setPixel(y, x, 32, 180, 237);
        }
        if (error > 0) {
            y += shift;
            error += 2 * (delta_y - delta_x);
        } else {
            error += 2 * delta_y;
        }
    }
}


// Decides which method of bresenham should be executed
void drawLine(ppgso::Image& framebuffer, Point& from, Point& to) {

    // TODO: Implement Bresenham drawing algorithm
    int delta_x = to.x - from.x;
    int delta_y = to.y - from.y;

    // Checks if lines are more horizontal or vertical
    if (abs(delta_y) < abs(delta_x)) {
        // Checks if lines are from left to right or from right to left
        // If from.x > to.x, then switch starting and ending coordinates
        if (from.x > to.x) {
            bresenham(framebuffer, to.x, from.x, to.y, from.y, true);
        } else {
            bresenham(framebuffer, from.x, to.x, from.y, to.y, true);
        }
    // If delta_y > delta_x, then X and Y are reversed
    } else {
        // Checks if lines are from bottom to top or from top to bottom
        if (from.y > to.y) {
            bresenham(framebuffer, to.y, from.y, to.x, from.x, false);
        } else {
            bresenham(framebuffer, from.y, to.y, from.x, to.x, false);
        }
    }
}


int main() {
    // Use ppgso::Image as our framebuffer
    ppgso::Image framebuffer(SIZE, SIZE);

    // TODO: Generate star points
    std::vector<Point> points;
    for (int i = 0; i < 8; i++) {
        Point vertex;
        vertex.x = (int) round(200 * cos((2 * M_PI * i)/ 7) + 256);
        vertex.y = (int) round(200 * sin((2 * M_PI * i)/ 7) + 256);
        points.push_back(vertex);
    }

    // Draw lines
    for (unsigned int i = 0; i < points.size() - 1; i++) {
        for (unsigned int j = i; j < points.size() - 1; j++) {
            drawLine(framebuffer, points[i], points[j]);
        }
    }

    // Save the result
    std::cout << "Generating task2_bresenham.bmp file ..." << std::endl;
    ppgso::image::saveBMP(framebuffer, "task2_bresenham.bmp");

    std::cout << "Done." << std::endl;
    return EXIT_SUCCESS;
}
