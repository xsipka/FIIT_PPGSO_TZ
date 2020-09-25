// Task 1 - Load a 512x512 image lena.raw
//        - Apply specified per-pixel transformation to each pixel
//        - Save as result.raw
#include <fstream>
#include <iostream>
#include <cstdlib>

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

//Checks if char value is between 0 and 255
unsigned char check_value(float value) {
    if (value > 255) {
        return 255;
    }
    if (value < 0) {
        return 0;
    }
    return (unsigned char)value;
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
      // Grayscale
      unsigned char grey = check_value(float (framebuffer[y][x].red * 0.3 + framebuffer[y][x].green * 0.59 + framebuffer[y][x].blue * 0.11));
      framebuffer[y][x].red = grey;
      framebuffer[y][x].green = grey;
      framebuffer[y][x].blue = grey;

      // Noise
      framebuffer[y][x].red = check_value(float (framebuffer[y][x].red + rand() % 100)); // NOLINT(cert-msc50-cpp)
      framebuffer[y][x].green = check_value(float (framebuffer[y][x].green + rand() % 100)); // NOLINT(cert-msc50-cpp)
      framebuffer[y][x].blue = check_value(float (framebuffer[y][x].blue + rand() % 100)); // NOLINT(cert-msc50-cpp)
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
