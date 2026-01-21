#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <random>

#include "renderer.h"
#include "shader.h"
#include "body.h"
#include "menuGUI.h"


void processInput(GLFWwindow *window);

constexpr unsigned int SCR_WIDTH = 1980;
constexpr unsigned int SCR_HEIGHT = 1080;


// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    renderer renderEngine(SCR_WIDTH, SCR_HEIGHT, "N-Body Simulation OpenGL");
    menuGUI menu(renderEngine.getWindow());
    Shader shader("../shader.vert", "../shader.frag");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> pos_dist(0.f, static_cast<float>(SCR_HEIGHT));
    std::uniform_real_distribution<float> vel_dist(-10.0f, 10.0f);
    std::uniform_real_distribution<float> radius_dist(170.0f, 400.0f);
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> inclination_dist(-0.8f, 0.8f);
    std::uniform_real_distribution<float> mass_dist(1.0f, 10.0f);
    std::uniform_real_distribution<float> body_radius_dist(15.0f, 30.0f);

    std::vector<body> bodies;

    float centralMass = 10000.0f;
    body sun(glm::vec3(960, 540, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 0), 100, centralMass);
    bodies.push_back(sun);

    int numBodies = 1000;
    for (size_t i = 0; i < numBodies; i++) {
        float radius = radius_dist(gen);
        float angle = angle_dist(gen);
        float inclination = inclination_dist(gen);
        glm::vec3 colour(std::sin(i), std::cos(i), 1);
        float mass = mass_dist(gen);
        float bodyRadius = body_radius_dist(gen);
        body b = body::createStableOrbit(sun, radius, angle, inclination, colour, bodyRadius, mass);
        std::uniform_real_distribution<float> z_dist(-200.0f, 200.0f);
        b.position.z += z_dist(gen);
        bodies.push_back(b);
    }

    // Generate sphere with indices
    auto sphereData = body::generateSphereVertices(1.0f, 32);

    renderEngine.setupBuffers(sphereData, numBodies);

    while (!renderEngine.shouldClose()) {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        renderEngine.processInput(deltaTime);

        std::vector<glm::vec3> forces(bodies.size(), glm::vec3(0.0f));
        for (size_t i = 0; i < bodies.size(); i++) {
            for (size_t j = i + 1; j < bodies.size(); j++) {
                const glm::vec3 force = bodies[i].calculateGravitationalForce(bodies[j]);
                forces[i] += force;
                forces[j] -= force;
            }
        }
        for (size_t i = 0; i < bodies.size(); i++) {
            glm::vec3 acceleration = forces[i] / bodies[i].mass;
            bodies[i].velocity += acceleration * deltaTime;
            bodies[i].position += bodies[i].velocity * deltaTime;
        }
        for (size_t i = 0; i < bodies.size(); i++) {
            for (size_t j = i + 1; j < bodies.size(); j++) {
                bodies[i].collisionCheck(bodies[j]);
            }
        }
        renderEngine.renderFrame(bodies, shader);
        menuGUI::newFrame();
        menu.render();
        renderEngine.swapBuffers();
    }
    return 0;
}



