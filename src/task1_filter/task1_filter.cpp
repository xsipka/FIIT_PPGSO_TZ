// Task 1 - Load a 512x512 image lena.raw
//        - Apply specified per-pixel transformation to each pixel
//        - Save as result.raw
#include <fstream>
#include <iostream>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

using namespace std;

// Size of the framebuffer
const unsigned int SIZE = 512;

// A simple RGB struct will represent a pixel in the framebuffer
struct Pixel {
  // TODO: Define correct data type for r, g, b channel
  unsigned char red;
  unsigned char green;
  unsigned char blue;
};

// Checks if char value is between 0 and 255
unsigned char check_value(double value) {
    if (value > 255) {
        return 255;
    }
    if (value < 0) {
        return 0;
    }
    return (unsigned char)value;
}

double modulo(double a, double b) {
    int ans = int (a / b);
    return (a - (ans * b));
}

double calculate_hue_value(double max, double min, double r, double g, double b, double diff) {
    double hue = -1;

    if (max != min) {
        if (max == r) {
            hue = 60 * ((g - b) / diff);
        }
        else if (max == g) {
            hue = 60 * ((b - r) / diff + 2);
        }
        else if (max == b) {
            hue = 60 * ((r - g) / diff + 4);
        }
    } else {
        hue = 0;
    }
    if (hue < 0) {
        hue += 360;
    }
    return hue;
}

double calculate_saturation_value(double max, double diff) {
    if (max == 0) {
        return 0;
    } else {
        return diff / max;
    }
}

double absolute_value(double val) {
    if (val < 0) {
        return val * -1;
    } else {
        return val;
    }
}

struct Pixel calculate_rgb_value(Pixel pixel, double h, double s, double v) {
    double red = pixel.red;
    double green = pixel.green;
    double blue = pixel.blue;

    double chroma = v * s;
    double X = chroma * (1 - absolute_value(modulo(h, 2) - 1));
    double m = v - chroma;

    if (h <= 1) {
        red = chroma + m;
        green = X + m;
        blue = 0 + m;
    }
    else if (h <= 2) {
        red = X + m;
        green = chroma + m;
        blue = 0 + m;
    }
    else if (h <= 3) {
        red = 0 + m;
        green = chroma + m;
        blue = X + m;
    }
    else if (h <= 4) {
        red = 0 + m;
        green = X + m;
        blue = chroma + m;
    }
    else if (h <= 5) {
        red = X + m;
        green = 0 + m;
        blue = chroma + m;
    }
    else if (h <= 6) {
        red = chroma + m;
        green = 0 + m;
        blue = X + m;
    }
    pixel.red = static_cast<unsigned char>(red * 255);
    pixel.green = static_cast<unsigned char>(green * 255);
    pixel.blue = static_cast<unsigned char>(blue * 255);
    return pixel;
}

int main() {
  // Initialize a framebuffer
  auto framebuffer = new Pixel[SIZE][SIZE];

  // TODO: Open file lena.raw (this is 512x512 RAW GRB format)
  ifstream file ("lena.raw", ios::binary);

  // TODO: Read data to framebuffer and close the file
  if (file) {
      file.read((char * )(framebuffer), sizeof(Pixel) * SIZE * SIZE);
      file.close();
  }
  else {
      std::cout << "File not found ... " << std::endl;
      exit(1);
  }

  // Traverse the framebuffer
  for (unsigned int y = 0; y < SIZE; y++) {
    for (unsigned int x = 0; x < SIZE; x++) {
      // TODO: Apply pixel operation

      // RGB to HSV
      double red = double (framebuffer[y][x].red ) / 255;
      double green = double (framebuffer[y][x].green ) / 255;
      double blue = double (framebuffer[y][x].blue ) / 255;

      double color_max = MAX(red, MAX (green, blue));
      double color_min = MIN(red, MIN (green, blue));
      double diff = color_max - color_min;

      double hue = calculate_hue_value(color_max, color_min, red, green, blue, diff);
      double saturation = calculate_saturation_value(color_max, diff);
      double value = color_max;

      // Do something with hue or saturation
      hue *= double (0.05);
      saturation *= double (0.75);
      value *= double (0.75);

      // HSV to RGB
      framebuffer[y][x] = calculate_rgb_value(framebuffer[y][x], hue / 60, saturation, value);

      // Change colors to grayscale
      //unsigned char grey = check_value(double (framebuffer[y][x].red * 0.3 + framebuffer[y][x].green * 0.59 + framebuffer[y][x].blue * 0.11));
      //framebuffer[y][x].red = grey;
      //framebuffer[y][x].green = grey;
      //framebuffer[y][x].blue = grey;
    }
  }

  // TODO: Open file result.raw
  ofstream result;
  result.open("result.raw", ios::binary);
  std::cout << "Generating result.raw file ..." << std::endl;

  // TODO: Write the framebuffer to the file and close it
  result.write((const char *)(framebuffer), sizeof(Pixel) * SIZE * SIZE);
  std::cout << "Done." << std::endl;
  result.close();

  delete[] framebuffer;
  return EXIT_SUCCESS;
}
