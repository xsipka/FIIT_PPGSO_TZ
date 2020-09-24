// Task 1 - Load a 512x512 image lena.raw
//        - Apply specified per-pixel transformation to each pixel
//        - Save as result.raw
#include <fstream>
#include <iostream>

// Size of the framebuffer
const unsigned int SIZE = 512;

// A simple RGB struct will represent a pixel in the framebuffer
struct Pixel {
  // TODO: Define correct data type for r, g, b channel
  unsigned char red;
  unsigned char green;
  unsigned char blue;
};


int main() {
  // Initialize a framebuffer
  auto framebuffer = new Pixel[SIZE][SIZE];

  // TODO: Open file lena.raw (this is 512x512 RAW GRB format)
  FILE *file = fopen("lena.raw", "rb");

  // TODO: Read data to framebuffer and close the file
    if (file) {
        fread(framebuffer, sizeof(Pixel), SIZE*SIZE, file );
        while (!feof(file)) {
            fread(framebuffer, sizeof(Pixel), SIZE * SIZE, file);
        }
    }
    else {
        std::cout << "File not found ... " << std::endl;
        exit(1);
    }

  // Traverse the framebuffer
  for (unsigned int y = 0; y < SIZE; y++) {
    for (unsigned int x = 0; x < SIZE; x++) {
      // TODO: Apply pixel operation
      framebuffer[y][x].red = framebuffer[y][x].red;
      framebuffer[y][x].green = framebuffer[y][x].green;
      framebuffer[y][x].blue = framebuffer[y][x].blue;
    }
  }

  // TODO: Open file result.raw
  FILE *result = fopen("result.raw", "wb");
  std::cout << "Generating result.raw file ..." << std::endl;

  // TODO: Write the framebuffer to the file and close it
  fwrite(framebuffer, sizeof(Pixel), SIZE * SIZE, result);
  fclose(result);
  std::cout << "Done." << std::endl;

  delete[] framebuffer;
  return EXIT_SUCCESS;
}
