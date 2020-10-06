// Task 4 - Render the letter R using OpenGL
//        - Implement a function to generate bezier curve points
//        - Generate multiple points on each bezier curve
//        - Draw lines connecting the generated points using OpenGL

#include <vector>
#include <cmath>
#include <ppgso/ppgso.h>

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

const unsigned int SIZE = 512;

class BezierWindow final : public ppgso::Window {
private:

  // Control points for the bezier curves
  // First curve is 4 control points
  // Rest of the curves are 3 control points, each reusing the last curve end
  // Defines the letter "R"
  std::vector<glm::vec2> controlPoints = {
      { 0.000, 0.9500 },
      { 0.200, 0.9500 },
      { 0.400, 0.9500 },
      { 0.500, 0.9500 },
      { 0.452, 0.7720 },
      { 0.102, 0.6160 },
      { 0.027, 0.2200 },
      { 0.004, -0.063 },
      { 0.253, 0.0640 },
      { 0.410, -0.058 },
      { 0.477, -0.127 },
      { 0.437, -0.228 },
      { 0.252, -0.239 },
  };

  // This will hold the bezier curve geometry once we generate it
  std::vector<glm::vec3> points;

  // GLSL Program to use for rendering
  ppgso::Shader program = {color_vert_glsl, color_frag_glsl};

  // These numbers are used to pass buffer data to OpenGL
  GLuint vao = 0, vbo = 0;



  // Compute points for Bezier curve using 4 control points
  static glm::vec2 bezierPoint(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p3, const float t) {
    // TODO: Compute point on the Bezier curve
    float x = 0;
    float y = 0;
    glm::vec2 temp(x, y);

    temp.x = static_cast<float>(pow((1 - t), 3) * p0.x + 3 * pow((1 - t), 2) * t * p1.x +
                                3 * (1 - t) * pow(t, 2) * p2.x + pow(t, 3) * p3.x);
    temp.y = static_cast<float>(pow((1 - t), 3) * p0.y + 3 * pow((1 - t), 2) * t * p1.y +
                                3 * (1 - t) * pow(t, 2) * p2.y + pow(t, 3) * p3.y);
    return temp;
  }


  // Compute points for a sequence of Bezier curves defined by a vector of control points
  // Each bezier curve will reuse the end point of the previous curve
  // count - Number of points to generate on each curve
  void bezierShape(int count) {
      for(int i = 0; i < (int) controlPoints.size() - 1; i += 3) {
          for (int j = 0; j <= count; j++) {
            // TODO: Generate points for each Bezier curve and insert them
            float t = (float)j / (float)count;
            auto index = static_cast<unsigned long long int>(i);
            glm::vec2 point = bezierPoint(controlPoints[index], controlPoints[index + 1], controlPoints[index + 2], controlPoints[index + 3], t);
            points.emplace_back(point, 0);
          }
      }
  }


public:
  BezierWindow() : Window{"task3_bezier", SIZE, SIZE} {
    // Generate Bezier curve points
    bezierShape(100);

    // Generate a vertex array object
    // This keeps track of what attributes are associated with buffers
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate a vertex buffer object, this will feed data to the vertex shader
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // TODO: Pass the control points to the GPU
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(points[0]), &points[0], GL_STATIC_DRAW);

    // Setup vertex array lookup, this tells the shader how to pick data for the "Position" input
    auto position_attrib = program.getAttribLocation("Position");
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(position_attrib);

    // Set model matrix to identity
    program.setUniform("ModelMatrix", glm::mat4{});
    program.setUniform("ViewMatrix", glm::mat4{});
    program.setUniform("ProjectionMatrix", glm::mat4{});

    // Set the color uniform
    program.setUniform("OverallColor", glm::vec3{0.125f,0.706f,0.929f});
  }

  ~BezierWindow() final {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  void onIdle() final {
    // Set black background
    glClearColor(0,0,0,0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw shape
    glBindVertexArray(vao);

    // TODO: Define the correct render mode
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) points.size());
  }
};

int main() {
  // Create our window
  BezierWindow window;

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
