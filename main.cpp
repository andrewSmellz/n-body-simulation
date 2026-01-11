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
#include "camera.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

constexpr unsigned int SCR_WIDTH = 1920;
constexpr unsigned int SCR_HEIGHT = 1080;

// Camera
Camera camera(glm::vec3(960.0f, 540.0f, 2000.0f));

// Mouse
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "N-Body Simulation OpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Setup mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);

    // Initialize shader
    const Shader shader("../shader.vert", "../shader.frag");

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

    int numBodies = 100;
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
    auto [vertices, indices,normals] = body::generateSphereVertices(1.0f, 32);

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Setup vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);

    // Setup element buffer (indices)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    // Vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    //vertex normal attribute
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(4);

    // Setup instance buffer
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, bodies.size() * 7 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Instance position attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // Instance radius attribute
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // Instance colour attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void *>(4 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader.use();

    std::cout << "Begin simulation...." << std::endl;
    std::cout << "number of bodies: " << bodies.size() << std::endl;
    std::cout << "Sphere vertices: " << vertices.size() / 3 << std::endl;
    std::cout << "Sphere indices: " << indices.size() << std::endl;

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        processInput(window);

        // Update camera matrices
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                                0.1f, 10000.0f);

        // Physics update
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
                bodies[i].velocity += acceleration * deltaTime;
                bodies[i].position += bodies[i].velocity * deltaTime;
            }

            for (size_t i = 0; i < bodies.size(); i++) {
                for (size_t j = i + 1; j < bodies.size(); j++) {
                    bodies[i].collisionCheck(bodies[j]);
                }
            }

            for (auto &b: bodies) {
                instanceData.push_back(b.position.x);
                instanceData.push_back(b.position.y);
                instanceData.push_back(b.position.z);
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!bodies.empty()) {
            shader.use();
            shader.setMat4("projection", projection);
            shader.setMat4("view", view);
            shader.setVec3("lightPos", bodies[0].position);
            shader.setVec3("viewPos", camera.Position);

            glBindVertexArray(VAO);
            // Use glDrawElementsInstanced instead of glDrawArraysInstanced
            glDrawElementsInstanced(GL_TRIANGLES, indices.size(),
                                    GL_UNSIGNED_INT, nullptr, bodies.size());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &instanceVBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, const int width, const int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

}
