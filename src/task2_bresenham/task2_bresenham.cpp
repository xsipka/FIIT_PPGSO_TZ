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
void drawLine(ppgso::Image& framebuffer, Point& from, Point& to) {
    // TODO: Implement Bresenham drawing algorithm
}

int main()
{
    // Use ppgso::Image as our framebuffer
    ppgso::Image framebuffer(SIZE, SIZE);

    // TODO: Generate star points
    std::vector<Point> points;
    for (int i = 0; i <= 7; i++) {
        Point vertex;
        vertex.x = 100 * cos((2 * M_PI * i)/ 7) + 250;
        vertex.y = 100 * sin((2 * M_PI * i)/ 7) + 250;
        points.push_back(vertex);
    }

    // Draw lines
    for(unsigned int i = 0; i < points.size() - 1; i++) {
        drawLine(framebuffer, points[i], points[i + 1]);
    }

    // Save the result
    std::cout << "Generating task2_bresenham.bmp file ..." << std::endl;
    ppgso::image::saveBMP(framebuffer, "task2_bresenham.bmp");

    std::cout << "Done." << std::endl;
    return EXIT_SUCCESS;
}
