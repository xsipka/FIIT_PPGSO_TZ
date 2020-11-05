// Task 6 - Generate a Bezier surface of variable density with UV coordinates.
//        - Confine the Bezier data and associated methods into a reusable class.
//        - Define a modelMatrix that uses position, rotation and scale.
//        - Render the generated mesh with texturing applied.
//        - Animate rotation.

#include <iostream>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/euler_angles.hpp>
//#include <glm/gtx/transform.hpp>

#include <ppgso/ppgso.h>

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

const unsigned int SIZE = 512;

// Object to represent Bezier patch
class BezierPatch {
private:
    // 3D vectors define points/vertices of the shape
    std::vector<glm::vec3> vertices;

    // Texture coordinates
    std::vector<glm::vec2> texCoords;

    // Define our face using indexes to 3 vertices
    struct face {
        GLuint v0, v1, v2;
    };

    // Define our mesh as collection of faces
    std::vector<face> indices;

    // These will hold the data and object buffers
    GLuint vao, vbo, tbo, ibo;
    glm::mat4 modelMatrix{1.0f};


    static float calculateBezierPoint(float point0, float point1, float point2, float point3, float t) {
       auto num = static_cast<float>(pow((1 - t), 3) * point0 + 3 * pow((1 - t), 2) * t * point1 +
                                     3 * (1 - t) * pow(t, 2) * point2 + pow(t, 3) * point3);
       return num;
    }

    static glm::vec3 bezierPoint(const glm::vec3 controlPoints[4], float t) {
        // TODO: Compute 3D point on bezier curve
        float x = 0;
        float y = 0;
        float z = 0;
        glm::vec3 temp(x, y, z);

        temp.x = calculateBezierPoint(controlPoints[0].x, controlPoints[1].x, controlPoints[2].x, controlPoints[3].x, t);
        temp.y = calculateBezierPoint(controlPoints[0].y, controlPoints[1].y, controlPoints[2].y, controlPoints[3].y, t);
        temp.z = calculateBezierPoint(controlPoints[0].z, controlPoints[1].z, controlPoints[2].z, controlPoints[3].z, t);

        return temp;
    }

    ppgso::Shader program{texture_vert_glsl, texture_frag_glsl};
    ppgso::Texture texture{ppgso::image::loadBMP("lena.bmp")};

public:
    // Public attributes that define position, color ..
    glm::vec3 position{0, 0, 0};
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 scale{1, 1, 1};

    // Initialize object data buffers
    explicit BezierPatch(glm::vec3 controlPoints[4][4]) {
        // Generate Bezier patch points and incidences
        unsigned int PATCH_SIZE = 10;
        for (unsigned int i = 0; i < PATCH_SIZE; i++) {

            auto u = i * (1.f / PATCH_SIZE);
            glm::vec3 temp[4];
            for (unsigned int k = 0; k < 4; k++) {
                temp[k] = (bezierPoint(controlPoints[k], u));
            }

            for (unsigned int j = 0; j < PATCH_SIZE; j++) {
                auto v = j * (1.f / PATCH_SIZE);
                vertices.push_back(bezierPoint(temp, v));
                texCoords.emplace_back(u, 1 - v);
            }
        }
        // Generate indices
        for (unsigned int i = 1; i < PATCH_SIZE; i++) {
            for (unsigned int j = 1; j < PATCH_SIZE; j++) {
                // TODO: Compute indices for triangle 1
                face temp;
                temp.v0 = i + PATCH_SIZE * (j - 1);
                temp.v1 = (i - 1) + PATCH_SIZE * j;
                temp.v2 = (i - 1) + PATCH_SIZE * (j - 1);
                indices.push_back(temp);

                // TODO: Compute indices for triangle 2
                temp.v0 = i + PATCH_SIZE * (j - 1);
                temp.v1 = (i - 1) + PATCH_SIZE * j;
                temp.v2 = i + PATCH_SIZE * j;
                indices.push_back(temp);
            }
        }

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

        // Copy texture positions to gpu
        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);

        // Set vertex program inputs
        auto texCoord_attrib = program.getAttribLocation("TexCoord");
        glEnableVertexAttribArray(texCoord_attrib);
        glVertexAttribPointer(texCoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        // Copy indices to gpu
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(face), indices.data(), GL_STATIC_DRAW);

    };

    // Clean up
    ~BezierPatch() {
        // Delete data from OpenGL
        glDeleteBuffers(1, &ibo);
        glDeleteBuffers(1, &tbo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    // Set the object transformation matrix
    void update() {
        // TODO: Compute transformation by scaling, rotating and then translating the shape
        modelMatrix = glm::mat4{1};
        modelMatrix = glm::scale(modelMatrix, scale);
        modelMatrix = glm::rotate(modelMatrix, rotation.x, {1, 0, 0});
        modelMatrix = glm::rotate(modelMatrix, rotation.y, {0, 1, 0});
        modelMatrix = glm::rotate(modelMatrix, rotation.z, {0, 0, 1});
    }

    // Draw polygons
    void render() {
        // Update transformation and color uniforms in the shader
        program.use();

        // Initialize projection
        // Create projection matrix (field of view (radians), aspect ratio, near plane distance, far plane distance)
        // You can think of this as the camera objective settings
        auto projection = glm::perspective((ppgso::PI / 180.f) * 60.0f, 1.0f, 0.1f, 10.0f);
        program.setUniform("ProjectionMatrix", projection);

        // Create view matrix (translate camera a bit backwards, so we can see the geometry)
        // This can be seen as the camera position/rotation in space
        auto view = glm::translate(glm::mat4{}, {0.0f, 0.0f, -3.0f});
        program.setUniform("ViewMatrix", view);

        // Set model position
        program.setUniform("ModelMatrix", modelMatrix);

        // Bind texture
        program.setUniform("Texture", texture);

        glBindVertexArray(vao);
        // TODO: Use correct rendering mode to draw the result
        glDrawElements(GL_TRIANGLES, (GLsizei) indices.size() * 3, GL_UNSIGNED_INT, nullptr);
    };
};

class BezierSurfaceWindow : public ppgso::Window {
private:
    // Define 16 control points
    glm::vec3 controlPoints[4][4]{
            {{-1, 1,   0}, {-0.5, 1,   0}, {.5, 1,   0}, {1, 1,   3},},
            {{-1, .5,  0}, {-0.5, .5,  0}, {.5, .5,  0}, {1, .5,  0},},
            {{-1, -.5, 0}, {-0.5, -.5, 0}, {.5, -.5, 0}, {1, -.5, -1},},
            {{-1, -1,  3}, {-0.5, -1,  0}, {.5, -1,  0}, {1, -1,  0},},
    };
    //BezierPatch bezier{controlPoints};

public:
    BezierSurfaceWindow() : Window{"task6_bezier_surface", SIZE, SIZE} {
        // Initialize OpenGL state
        // Enable Z-buffer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }

    void onIdle() final {
        // Set gray background
        glClearColor(.1f, .1f, .1f, 1.0f);

        // Clear depth and color buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Move and Render shape
        auto time = (float) glfwGetTime();

        controlPoints[2][2].x = static_cast<float>(sin(time) * 1.5f);
        controlPoints[2][2].y = static_cast<float>(sin(time) * 1.5f);
        controlPoints[2][2].z = static_cast<float>(sin(time) * 1.5f);

        controlPoints[3][1].x = static_cast<float>(cos(time) * 1.5f);
        controlPoints[3][1].y = static_cast<float>(cos(time) * 1.5f);
        controlPoints[3][1].z = static_cast<float>(sin(time) * 1.5f);

        BezierPatch bezier(controlPoints);

        bezier.rotation = {time, cos(time), 0.f};
        bezier.scale = {sin(time), 1.f, 1.f};

        bezier.update();
        bezier.render();
    }
};

int main() {
    // Create new window
    auto window = BezierSurfaceWindow{};

    // Main execution loop
    while (window.pollEvents()) {}

    return EXIT_SUCCESS;
}
