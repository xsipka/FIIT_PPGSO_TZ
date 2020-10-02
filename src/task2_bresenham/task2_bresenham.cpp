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


// Prints lines that are more horizontal (delta x > delta y)
void drawLineAxisX (ppgso::Image& framebuffer, int x1, int x2, int y1, int y2) {
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;
    int y_shift = 1;
    int y = y1;
    int error = 2 * delta_y - delta_x;

    // Checks if line is increasing or decreasing on Y axis
    if (delta_y < 0) {
        y_shift = -1;
        delta_y *= -1;
    }
    // Iterate through X, because line is more horizontal
    for (int x = x1; x <= x2; x++) {
        framebuffer.setPixel(x, y, 32, 180, 237);
        if (error > 0) {
            y += y_shift;
            error += 2 * (delta_y - delta_x);
        } else {
            error += 2 * delta_y;
        }
    }
}


// Prints lines that are more vertical (delta y > delta x)
void drawLineAxisY (ppgso::Image& framebuffer, int x1, int x2, int y1, int y2) {
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;
    int x_shift = 1;
    int x = x1;
    int error = 2 * delta_x - delta_y;

    // Checks if line is increasing or decreasing on X axis
    if (delta_x < 0) {
        x_shift = -1;
        delta_x *= -1;
    }
    // Iterate through Y, because line is more vertical
    for (int y = y1; y <= y2; y++) {
        framebuffer.setPixel(x, y, 32, 180, 237);
        if (error > 0) {
            x += x_shift;
            error += 2 * (delta_x - delta_y);
        } else {
            error += 2 * delta_x;
        }
    }
}


// Bresenham drawing algorithm
void drawLine(ppgso::Image& framebuffer, Point& from, Point& to) {

    // TODO: Implement Bresenham drawing algorithm
    int delta_x = to.x - from.x;
    int delta_y = to.y - from.y;

    // Checks if lines are more horizontal or vertical
    if (abs(delta_y) < abs(delta_x)) {
        // Checks if lines are from left to right or from right to left
        // If from.x > to.x, then switch starting and ending coordinates
        if (from.x > to.x) {
            drawLineAxisX(framebuffer, to.x, from.x, to.y, from.y);
        } else {
            drawLineAxisX(framebuffer, from.x, to.x, from.y, to.y);
        }
    } else {
        // Checks if lines are from bottom to top or from top to bottom
        if (from.y > to.y) {
            drawLineAxisY(framebuffer, to.x, from.x, to.y, from.y);
        } else {
            drawLineAxisY(framebuffer, from.x, to.x, from.y, to.y);
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
