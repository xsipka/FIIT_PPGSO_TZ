// Task 7 - Dynamically generate objects in a 3D scene
//        - Implement a particle system where particles have position and speed
//        - Any object can be a generator and can add objects to the scene
//        - Create dynamic effect such as fireworks, rain etc.
//        - Encapsulate camera in a class

#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <cstdlib>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <ppgso/ppgso.h>

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

const unsigned int SIZE = 512;
const float G = 9.81f;

class Camera {
public:
  // TODO: Add parameters
  glm::vec3 position{0,0,0};
  glm::vec3 center{0,0,-1};
  glm::vec3 world_up{0,1,0};

  glm::mat4 viewMatrix;
  glm::mat4 projectionMatrix;

  /// Representaiton of
  /// \param fov - Field of view (in degrees)
  /// \param ratio - Viewport ratio (width/height)
  /// \param near - Distance of the near clipping plane
  /// \param far - Distance of the far clipping plane
  Camera(float fov = 45.0f, float ratio = 1.0f, float near = 0.1f, float far = 10.0f) {
    // TODO: Initialize perspective projection (hint: glm::perspective)
      float fov_final = (ppgso::PI/180.0f) * fov;
      projectionMatrix = glm::perspective(fov_final, ratio, near, far);
  }

  /// Recalculate viewMatrix from position, rotation and scale
  void update() {
    // TODO: Update viewMatrix (hint: glm::lookAt)
      viewMatrix = glm::lookAt(position, center, world_up);
  }
};

/// Abstract renderable object interface
class Renderable; // Forward declaration for Scene
using Scene = std::list<std::unique_ptr<Renderable>>; // Type alias

class Renderable {
public:
  // Virtual destructor is needed for abstract interfaces
  virtual ~Renderable() = default;

  /// Render the object
  /// \param camera - Camera to use for rendering
  virtual void render(const Camera& camera) = 0;

  virtual bool update(float dTime, Scene &scene) = 0;

  virtual std::pair<bool, glm::vec3> is_water() = 0;
};

/// Basic particle that will render a sphere
/// TODO: Implement Renderable particle
class WaterParticle final : public Renderable {
  // Static resources shared between all particles
  static std::unique_ptr<ppgso::Mesh> mesh;
  static std::unique_ptr<ppgso::Shader> shader;

  // TODO: add more parameters as needed
  glm::vec3 position;
  glm::vec3 speed;
  glm::vec3 color;
  glm::vec3 rotation;
  glm::vec3 scale;
  glm::mat4 modelMatrix;


public:
    WaterParticle(glm::vec3 p, glm::vec3 s, glm::vec3 c) {
    // First particle will initialize resources
    if (!shader) shader = std::make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");

    position = p;
    speed = s;
    scale = c;
    color = glm::vec3(0.f, 0.5f, 0.8f);
  }

  bool update(float dTime, Scene &scene) override {

    if (position.y < -3.f) { return false; }

    position += speed * dTime * 0.5f;
    speed += glm::vec3{0,-G,0} * dTime;

    modelMatrix = glm::mat4(1.f);
    modelMatrix = translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);

    return true;
  }

  void render(const Camera& camera) override {
    // TODO: Render the object
    shader->use();
    shader->setUniform("ProjectionMatrix", camera.projectionMatrix);
    shader->setUniform("ViewMatrix", camera.viewMatrix);
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("OverallColor", color);
    mesh->render();
  }

    std::pair <bool, glm::vec3> is_water() override {

        std::pair <bool, glm::vec3> values;
        values.first = true;
        values.second = position;
        return values;
    }
};

class CoronaParticle final : public Renderable {
    // Static resources shared between all particles
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;

    // TODO: add more parameters as needed
    glm::vec3 position;
    glm::vec3 speed;
    glm::vec3 color;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 modelMatrix;
    float time_to_live;

public:
    CoronaParticle(glm::vec3 p, glm::vec3 s, glm::vec3 c) {
        // First particle will initialize resources
        if (!shader) shader = std::make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
        if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");

        position = p;
        speed = s;
        scale = c;
        color = glm::vec3(0.25f, 1.f, 0.25f);
        time_to_live = 2.75f;
    }

    bool update(float dTime, Scene &scene) override {

        time_to_live -= dTime;
        if (time_to_live <= 0) { return false; }
        //if (position.y < -3.f) { return false; }

        //position += speed * dTime * 0.25f;
        //speed += glm::vec3{-10.5, -G,-5} * dTime;

        position += speed * dTime * 0.25f;
        speed += randomize_speed(0, 50) * dTime;

        modelMatrix = glm::mat4(1.f);
        modelMatrix = translate(modelMatrix, position);
        modelMatrix = glm::scale(modelMatrix, scale);

        return true;
    }

    void render(const Camera& camera) override {
        // TODO: Render the object
        shader->use();
        shader->setUniform("ProjectionMatrix", camera.projectionMatrix);
        shader->setUniform("ViewMatrix", camera.viewMatrix);
        shader->setUniform("ModelMatrix", modelMatrix);
        shader->setUniform("OverallColor", color);
        mesh->render();
    }

    static glm::vec3 randomize_speed(float min, float max) {

        float x = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max - min)));
        float y = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max - min)));
        float z = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max - min)));

        if (!chance()) {
            auto values = glm::vec3 (-x * 0.5f, -y, -z);
            return values;
        }
        else {
            auto values = glm::vec3(-x * 0.5f, y, z);
            return values;
        }
    }

    static bool chance() {
        bool chance = true;

        if ((rand() % 2) + 1 ==  2) {
            chance = false;
        }
        return chance;
    }

    std::pair <bool, glm::vec3> is_water() override {

        std::pair <bool, glm::vec3> values;
        values.first = false;
        return values;
    }
};

// Static resources need to be instantiated outside of the class as they are globals
std::unique_ptr<ppgso::Mesh> WaterParticle::mesh;
std::unique_ptr<ppgso::Shader> WaterParticle::shader;

std::unique_ptr<ppgso::Mesh> CoronaParticle::mesh;
std::unique_ptr<ppgso::Shader> CoronaParticle::shader;


class ParticleWindow : public ppgso::Window {
private:
  // Scene of objects
  Scene scene;

  // Create camera
  Camera camera = {120.0f, (float)width/(float)height, 1.0f, 400.0f};
  // Store keyboard state
  std::map<int, int> keys;
public:
  ParticleWindow() : Window{"task7_particles", SIZE, SIZE} {
    // Initialize OpenGL state
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
  }

  void onKey(int key, int scanCode, int action, int mods) override {
    // Collect key state in a map
    keys[key] = action;
    if (keys[GLFW_KEY_SPACE]) {
      // TODO: Add renderable object to the scene
      for (auto& i : scene) {
          auto obj = i.get();
          auto drop = obj->is_water();

          if (drop.first) {
              auto speed = randomize_vec3(-1.5f, 1.5f, 'S');
              auto scale = randomize_vec3(0.05f, 0.15f, 'C');
              scene.push_back(std::make_unique<CoronaParticle>(drop.second, speed, scale));
          }
      }
    }
  }

  static glm::vec3 randomize_vec3(float min, float max, char type) {

      glm::vec3 values;
      float x, y, z;
      float y_min = 5.5f;
      float y_max = 7.5f;
      float z_min = -1.5f;
      float z_max = -4.5f;

      switch (type) {
          case 'P':
              x = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max - min)));
              z = z_min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(z_max - z_min)));
              y = y_min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(y_max - y_min)));
              values = glm::vec3 (x, y, z);
              break;
          case 'S':
              x = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max - min)));
              y = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max - min)));
              z = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max - min)));
              values = glm::vec3 (x, y, z);
              break;
          case 'C':
              x = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max - min)));
              y = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max - min)));
              values = glm::vec3 (x, y, 0.1f);
              break;
          default:
              break;
      }

      return values;
  }

  static bool create_new_drop() {
      bool chance = (rand() % 100) < 1;
      return chance;
  }

  void onIdle() override {
    // Track time
    static auto time = static_cast<float>(glfwGetTime());
    // Compute time delta
    float dTime = static_cast<float>(glfwGetTime()) - time;
    time = static_cast<float>(glfwGetTime());

    // Set gray background
    glClearColor(.1f,.1f,.1f,1.0f);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (create_new_drop()) {
        auto pos = randomize_vec3(1, 5, 'P');
        auto speed = randomize_vec3(0.f, 0.05f, 'S');
        auto scale = randomize_vec3(.1f, 0.35f, 'C');
        scene.push_back(std::make_unique<WaterParticle>(pos, speed, scale));
    }

    auto i = std::begin(scene);
    while (i != std::end(scene)) {
      // Update object and remove from list if needed
      auto obj = i->get();
      if (!obj->update(dTime, scene)) {
          i = scene.erase(i);
      }
      else {
          ++i;
      }
    }

    // Render every object in scene
    for(auto& object : scene) {
      object->render(camera);
    }
  }
};

int main() {
  // Create new window
  auto window = ParticleWindow{};

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
