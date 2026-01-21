//
// Created by andre on 2026-01-21.
//

#include "renderer.h"
#include <iostream>

renderer::renderer(int width, int height, const char *title) : window(nullptr),
                                                               camera(glm::vec3(960.0f, 540.0f, 2000.0f)),
                                                               width(width),
                                                               height(height),
                                                               title(title),
                                                               VAO(0), VBO(0), EBO(0), instanceVBO(0),
                                                               indexCount(0),
                                                               menuMode(false),
                                                               firstMouse(true),
                                                               tabPressed(false),
                                                               lastX(width / 2.0f),
                                                               lastY(height / 2.0f) {
    renderer::init();
}

renderer::~renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &instanceVBO);
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool renderer::init() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);

    // Setup mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }


    glfwGetWindowSize(window, &width, &height);

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glEnable(GL_DEPTH_TEST);

    return true;
}

void renderer::setupBuffers(const SphereData &sphereData, int numBodies) {
    indexCount = sphereData.indices.size();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Setup vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereData.vertices.size() * sizeof(float),
                 sphereData.vertices.data(), GL_STATIC_DRAW);

    // Setup element buffer (indices)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereData.indices.size() * sizeof(unsigned int),
                 sphereData.indices.data(), GL_STATIC_DRAW);

    // Vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    //vertex normal attribute
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(4);

    // Setup instance buffer
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, numBodies * 7 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

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
}

void renderer::renderFrame(const std::vector<body> &bodies, const Shader &shader) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!bodies.empty()) {
        std::vector<float> instanceData;
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
        glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(float),
                     instanceData.data(), GL_DYNAMIC_DRAW);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                static_cast<float>(width) / static_cast<float>(height),
                                                0.1f, 10000.0f);
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setVec3("lightPos", bodies[0].position);
        shader.setVec3("viewPos", camera.Position);

        glBindVertexArray(VAO);
        // Use glDrawElementsInstanced instead of glDrawArraysInstanced
        glDrawElementsInstanced(GL_TRIANGLES, indexCount,
                                GL_UNSIGNED_INT, nullptr, bodies.size());
    }
}

bool renderer::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void renderer::processInput(float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
        menuMode = !menuMode;
        if (menuMode) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true;
        }
        tabPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
        tabPressed = false;
    }
    if (!menuMode) {
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
}

void renderer::swapBuffers() const {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

renderer *renderer::getRenderer(GLFWwindow *window) {
    return static_cast<renderer *>(glfwGetWindowUserPointer(window));
}

void renderer::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    renderer *r = getRenderer(window);
    if (r) {
        r->width = width;
        r->height = height;
    }
}

void renderer::mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    renderer *r = getRenderer(window);
    if (!r || r->menuMode) return;
    if (r->firstMouse) {
        r->lastX = xpos;
        r->lastY = ypos;
        r->firstMouse = false;
    }

    float xoffset = xpos - r->lastX;
    float yoffset = r->lastY - ypos;

    r->lastX = xpos;
    r->lastY = ypos;

    r->camera.ProcessMouseMovement(xoffset, yoffset);
}

void renderer::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    renderer *r = getRenderer(window);
    if (!r || r->menuMode) return;

    r->camera.ProcessMouseScroll(yoffset);
}


