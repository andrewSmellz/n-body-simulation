#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include<vector>
#include "shader.h"
#include "body.h"
#include <iostream>
#include <random>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

constexpr unsigned int SCR_WIDTH = 1920;
constexpr unsigned int SCR_HEIGHT = 1080;
constexpr float GRAVITY = 0; //scaled by 10 so 100px = 1 meter

int main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "N-Body Simulation OpenGl", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize shader
    const Shader shader("../shader.vert", "../shader.frag");
    const glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT),
                                            -1.0f, 1.0f);
    constexpr auto view = glm::mat4(1.0f);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> pos_dist(0.f, static_cast<float>(SCR_HEIGHT)); // adjust range as needed
    std::uniform_real_distribution<float> vel_dist(-10.0f, 10.0f); // small velocities
    std::uniform_real_distribution<float> radius_dist(170.0f, 400.0f);
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> mass_dist(1.0f, 10.0f);
    std::uniform_real_distribution<float> body_radius_dist(8.0f, 15.0f);

    std::vector<body> bodies;


    float centralMass = 10000.0f;
    body sun(glm::vec3(960, 540, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 0), 300, centralMass);
    bodies.push_back(sun);
    float offcentermass = 100.0f;
    body sun2(glm::vec3(1740, 500, 0), glm::vec3(0, 0, 0), glm::vec3(0.1, 1, 0.5), 150, offcentermass);
    bodies.push_back(sun2);
    body sun3(glm::vec3(180, 500, 0), glm::vec3(0, 0, 0), glm::vec3(0.1, 1, 0.5), 150, offcentermass);
    bodies.push_back(sun3);

    int numBodies = 200;
    for (size_t i = 0; i < numBodies; i++) {
        float radius = radius_dist(gen);
        float angle = angle_dist(gen);
        glm::vec3 colour(std::sin(i), std::cos(i), 1);
        //std::abs(std::sin(i)) - std::abs(std::cos(i)) i like this it looks cool
        float mass = mass_dist(gen);
        float bodyRadius = body_radius_dist(gen);
        bodies.emplace_back(body::createStableOrbit(sun, radius, angle, colour, bodyRadius, mass));
    }

    const std::vector<float> vertices = body::generateCircleVertices(1.0f);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position attribute (for circle vertices)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // Setup instance VBO (per-instance data: position.xy, radius = 3 floats per instance)
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    // Allocate space for instance data (you'll update this in your render loop)
    glBufferData(GL_ARRAY_BUFFER, bodies.size() * 6 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Instance attribute: position (x, y)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1); // Update per instance

    // Instance attribute: radius
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // Instance attribute: Colour
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Set shader uniforms
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    float lastTime = glfwGetTime();

    std::cout << "begin simulation...." << std::endl;
    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;
        float fps = 1.0f / dt;
        printf("FPS: %.2f     \r", fps);


        processInput(window);

        // Update instance buffer with current body data
        if (!bodies.empty()) {
            std::vector<float> instanceData;

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
                bodies[i].velocity += acceleration * dt;
                bodies[i].position += bodies[i].velocity * dt;
            }

            for (size_t i = 0; i < bodies.size(); i++) {
                for (size_t j = i + 1; j < bodies.size(); j++) {
                    bodies[i].collisionCheck(bodies[j]);
                }
            }

            for (auto &b: bodies) {
                if (b.position.y - b.radius <= 0.0f && b.velocity.y <= 0.0f) {
                    b.position.y = b.radius;
                    b.velocity.y = -b.velocity.y;
                }
                if (b.position.y + b.radius >= SCR_HEIGHT && b.velocity.y >= 0.0f) {
                    b.position.y = SCR_HEIGHT - b.radius;
                    b.velocity.y = -b.velocity.y;
                }
                if (b.position.x - b.radius <= 0.0f && b.velocity.x <= 0.0f) {
                    b.position.x = b.radius;
                    b.velocity.x = -b.velocity.x;
                }
                if (b.position.x + b.radius >= SCR_WIDTH && b.velocity.x >= 0.0f) {
                    b.position.x = SCR_WIDTH - b.radius;
                    b.velocity.x = -b.velocity.x;
                }
            }


            for (auto &b: bodies) {
                instanceData.push_back(b.position.x);
                instanceData.push_back(b.position.y);
                instanceData.push_back(b.radius);
                instanceData.push_back(b.colour.r);
                instanceData.push_back(b.colour.g);
                instanceData.push_back(b.colour.b);
            }

            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(float), instanceData.data(), GL_DYNAMIC_DRAW);
        }

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (!bodies.empty()) {
            shader.use();
            glBindVertexArray(VAO);
            glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, vertices.size() / 2, bodies.size());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &instanceVBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, const int width, const int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
