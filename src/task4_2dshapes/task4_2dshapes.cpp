// Task 5 - Draw a 2D shape using polygons and animate it
//        - Encapsulate the shape using a class
//        - Use color_vert/frag shader to display the polygon
//        - Animate the object position, rotation and scale.

#include <iostream>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <ppgso/ppgso.h>

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

const unsigned int SIZE = 512;
bool right_dir = true;
float counter = 0;


// Object to represent 2D OpenGL shape
class Shape {
private:
    // 2D vectors define points/vertices of the shape
    // TODO: Define your shape points
    std::vector<glm::vec3> vertices{
        {0.2,   0.6,   0},
        {0.4,   0.4,   0},
        {.35,   0.1,   0},
        {0.2,   0.23,  0},
        {.05,   0.1,   0},
        {0.112, 0.305, 0},
        {0.0,   0.4,   0},
        {0.14,  0.4,   0},
    };


    // Structure representing a triangular face, usually indexes into vertices
    struct Face {
        // TODO: Define your face structure
        GLuint a;
        GLuint b;
        GLuint c;
    };

    // Indices define triangles that index into vertices
    // TODO: Define your mesh indices
    std::vector<Face> mesh {
            {0, 5, 2},
            {4, 7, 1},
            {6, 1, 3}
    };

    // Program to associate with the object
    ppgso::Shader program = {color_vert_glsl, color_frag_glsl};

    // These will hold the data and object buffers
    GLuint vao, vbo, cbo, ibo;
    glm::mat4 modelMatrix{1.0f};


public:
    // Public attributes that define position, color ..
    glm::vec3 position{0, 0, 0};
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 scale{1, 1, 1};
    glm::vec3 color{1, 0, 0};

    // Initialize object data buffers
    Shape() {
        // Copy data to OpenGL
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Copy positions to gpu
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        // Set vertex program inputs
        auto position_attrib = program.getAttribLocation("Position");
        glEnableVertexAttribArray(position_attrib);
        glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // Copy mesh indices to gpu
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.size() * sizeof(Face), mesh.data(), GL_STATIC_DRAW);

        // Set projection matrices to identity
        program.setUniform("ProjectionMatrix", glm::mat4{1.0f});
        program.setUniform("ViewMatrix", glm::mat4{1.0f});
    };

    // Clean up
    ~Shape() {
        // Delete data from OpenGL
        glDeleteBuffers(1, &ibo);
        glDeleteBuffers(1, &cbo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    // Set the object transformation matrix
    void update(Shape &shape, bool fixed) {
        // TODO: Compute transformation by scaling, rotating and then translating the shape
        // Red / green star rotation
        if (!fixed) {
            modelMatrix = glm::mat4(1.f);
            modelMatrix = translate(modelMatrix, position);
            modelMatrix = rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
            modelMatrix = translate(modelMatrix, -position);
            modelMatrix = glm::scale(modelMatrix, scale);
        }
        else {
            // Blue star follows green star
            modelMatrix = glm::mat4(1.f);
            modelMatrix = translate(modelMatrix, position);
            modelMatrix = rotate(modelMatrix, glm::radians(shape.rotation.z), glm::vec3(0, 0, 1));
            modelMatrix = translate(modelMatrix, -position);
            // Rotation around green star
            modelMatrix = translate(modelMatrix, -shape.position);
            modelMatrix = rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
            modelMatrix = translate(modelMatrix, shape.position);
            modelMatrix = glm::scale(modelMatrix, scale);
        }
    }

    // Draw polygons
    void render() {
        // Update transformation and color uniforms in the shader
        program.use();
        program.setUniform("OverallColor", color);
        program.setUniform("ModelMatrix", modelMatrix);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, (GLsizei) mesh.size() * 3, GL_UNSIGNED_INT, nullptr);
    };
};

class ShapeWindow : public ppgso::Window {
private:
    Shape shape1, shape2, shape3;
public:
    ShapeWindow() : Window{"task4_2D Shapes", SIZE, SIZE} {
        shape1.color = {1, 0, 0};
        shape2.color = {0, 1, 0};
        shape3.color = {0, 0, 1};

        shape1.scale = {0.65, 0.65, 0.65};
        shape2.scale = {0.65, 0.65, 0.65};
        shape3.scale = {0.35, 0.35, 0.35};
    }

    void onIdle() override {
        // Set gray background
        glClearColor(.1f, .1f, .1f, 1.0f);
        // Clear depth and color buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Move and Render shape\    // Get time for animation
        auto t = (float) glfwGetTime();

        // TODO: manipulate shape1 and shape2 position to rotate clockwise
        shape1.position.x = 0.0f;
        shape1.position.y = 0.52f;
        shape2.position = {0.f, -0.3f, 0.f};
        shape3.position.x = shape2.position.x;
        shape3.position.y = shape2.position.y;

        // Manipulate rotation of the shape
        shape1.rotation.z = t * -70.0f;
        shape3.rotation.z = t * 70.0f;

        // Pendulum motion
        float angle = fmodf(t * 60.f, 360);
        if (angle < 1) { angle += 1; }

        if (right_dir && angle < 270) {
            std::cout << angle << " right\n";
            shape2.rotation.z = angle;
        }
        if (angle >= 270 && angle < 360) {
            right_dir = false;
            std::cout << angle << " left\n";
            shape2.rotation.z = -90.f - counter;
            counter += 0.037f;
        } else if (angle >= 0 && angle <= 90 && !right_dir) {
            std::cout << angle << " left\n";
            shape2.rotation.z = -90.f - counter;
            counter += 0.037f;
            right_dir = false;
        } else if (angle > 90 && !right_dir) {
            std::cout << angle << " right\n";
            shape2.rotation.z = angle - 20;
            right_dir = true;
            counter = 0;
        }

        // Update and render each shape
        shape1.update(shape1, false);
        shape2.update(shape2, false);
        shape3.update(shape2, true);

        shape1.render();
        shape2.render();
        shape3.render();
    }
};

int main() {
    // Create our window
    auto window = ShapeWindow{};

    // Main execution loop
    while (window.pollEvents()) {}

    return EXIT_SUCCESS;
}
