// Task 5 - Draw an origin with 3 axis and an animated cube rotating around one of the axis
//        - Encapsulate the Cube using a class, it will be used for the rotating object and the axis
//        - Use color_vert/frag shader to display the polygon
//        - Animate the rotation of the all 3 axis and the cube.

#include <cmath>
#include <iostream>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <ppgso/ppgso.h>

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

const unsigned int WIDTH = 640;
const unsigned  int HEIGHT = 480;

// Object to represent a 3D cube
class Cube {
private:
    // 2D vectors define points/vertices of the shape
    // TODO: Define cube vertices
    std::vector<glm::vec3> vertices = {
            { -0.5f, 0.5f, 0.5f },
            { -0.5f, -0.5f, 0.5f },
            { 0.5f, -0.5f, 0.5f },
            { 0.5f, 0.5f, 0.5f },
            { 0.5f, 0.5f, -0.5f },
            { 0.5f, -0.5f, -0.5f },
            { -0.5f, -0.5f, -0.5f },
            { -0.5f, 0.5f, -0.5f }
    };

    GLuint indices[36] = {
            0, 1, 2,
            0, 2, 3,
            7, 6, 1,
            7, 1, 0,
            4, 5, 6,
            4, 6, 7,
            3, 2, 5,
            3, 5, 4,
            1, 2, 6,
            6, 5, 2,
            0, 3, 7,
            3, 7, 4
    };
    unsigned indices_num = sizeof(indices) / sizeof(GLuint);

    // Program to associate with the object
    ppgso::Shader program = {color_vert_glsl, color_frag_glsl};

    // These will hold the data and object buffers
    GLuint vao, vbo, cbo, ibo;
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;

public:
    // Public attributes that define position, color ..
    glm::vec3 position{0, 0, 0};
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 scale{1, 1, 1};
    glm::vec3 color{1, 0, 0};


    // Initialize object data buffers
    Cube() {

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

        // Copy indices to gpu
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_num * sizeof(GLuint), indices, GL_STATIC_DRAW);

        // Set projection matrices to identity
        program.setUniform("ProjectionMatrix",
                           glm::perspective(glm::radians(90.f),
                           static_cast<float>(WIDTH) / static_cast<float>(HEIGHT),
                           0.1f, 100.0f));
        program.setUniform("ViewMatrix", viewMatrix);
    };

    // Clean up
    ~Cube() {
        // Delete data from OpenGL
        glDeleteBuffers(1, &ibo);
        glDeleteBuffers(1, &cbo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    // Set the object transformation matrix
    void updateModelMatrix() {
        // Compute transformation by scaling, rotating and then translating the shape
        // TODO: Update model matrix: modelMatrix = ... use position, rotation and scale
        modelMatrix = glm::mat4(1.f);
        //modelMatrix = translate(modelMatrix, position);
        modelMatrix = rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        modelMatrix = rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        modelMatrix = rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        modelMatrix = glm::scale(modelMatrix, scale);
    }

    // Update camera position
    void updateViewMatrix(glm::vec3 viewRotation) {
        // Compute transformation by scaling, rotating and then translating the shape
        // TODO: Update view matrix: modelMatrix = ... use translation -20 in Z and viewRotation
        viewMatrix = glm::mat4(1.f);
        viewMatrix = glm::lookAt(viewRotation, glm::vec3(0.f),  glm::vec3(0.f, 0.f, 1.f));
    }

    // Draw polygons
    void render() {
        // Update transformation and color uniforms in the shader
        program.use();
        program.setUniform("OverallColor", color);
        program.setUniform("ModelMatrix", modelMatrix);
        program.setUniform("ViewMatrix", viewMatrix);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_num), GL_UNSIGNED_INT, nullptr);
    };
};

class OriginWindow : public ppgso::Window {
private:
    Cube axisX, axisY, axisZ;
    Cube cube;

    glm::vec3 viewRotation{0, 0, 0};

public:
    OriginWindow() : Window{"task5_3d_origin", WIDTH, HEIGHT} {

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // TODO: Set axis colors to red,green and blue...and cube color to grey
        axisX.color = {1, 0, 0};
        axisY.color = {0, 1, 0};
        axisZ.color = {0, 0, 1};
        cube.color  = {0.5f, 0.5f, 0.5f};

        const float scaleMin = 0.035f;
        const float scaleMax = 10.00f;

        // TODO: Set axis scaling in X,Y,Z directions...hint use scaleMin in tangent directions and scaleMax in the axis direction
        axisX.scale = {scaleMax, scaleMin, scaleMin};
        axisY.scale = {scaleMin, scaleMax, scaleMin};
        axisZ.scale = {scaleMin, scaleMin, scaleMax};
        cube.scale = {0.35f, 0.35f, 0.35f};
    }

    void onIdle() override {
        // Set gray background
        glClearColor(0, 0, 0, 0);
        // Clear depth and color buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Move and Render shape\    // Get time for animation
        auto t = static_cast<float>(glfwGetTime());
        float radius = 2.f;
        float speed = 100.f;

        cube.rotation = {t * speed, t * speed, t * speed};

        viewRotation.x = static_cast<float>(std::sin(t) * radius);
        viewRotation.y = static_cast<float>(std::cos(t) * radius);
        viewRotation.z = static_cast<float>(std::cos(t) * radius);

        // TODO: update view matrix of X,Y,Z axis and cube
        // TODO: update model matrix
        cube.updateViewMatrix(viewRotation);
        axisX.updateViewMatrix(viewRotation);
        axisY.updateViewMatrix(viewRotation);
        axisZ.updateViewMatrix(viewRotation);

        cube.updateModelMatrix();
        axisX.updateModelMatrix();
        axisY.updateModelMatrix();
        axisZ.updateModelMatrix();

        cube.render();
        axisX.render();
        axisY.render();
        axisZ.render();
    }
};

int main() {
    // Create our window
    auto window = OriginWindow{};

    // Main execution loop
    while (window.pollEvents()) {}

    return EXIT_SUCCESS;
}
