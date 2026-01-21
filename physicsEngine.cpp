//
// Created by andre on 2026-01-21.
//

#include "physicsEngine.h"

#include "config.h"

void physicsEngine::update(std::vector<body> &bodies, double deltaTime) {
    if (bodies.empty()) return;
    deltaTime *= CONFIG.timeScale;
    std::vector<glm::vec3> forces(bodies.size(), glm::vec3(0.0f));
    calculateForces(bodies, forces);
    applyForces(bodies, forces, deltaTime);
    collisionCheck(bodies);
}

void physicsEngine::calculateForces(std::vector<body> &bodies, std::vector<glm::vec3> &forces) {
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            const glm::vec3 force = bodies[i].calculateGravitationalForce(bodies[j]);
            forces[i] += force;
            forces[j] -= force;
        }
    }
}

void physicsEngine::applyForces(std::vector<body> &bodies, std::vector<glm::vec3> &forces, float deltaTime) {
    for (size_t i = 0; i < bodies.size(); i++) {
        glm::vec3 acceleration = forces[i] / bodies[i].mass;
        bodies[i].velocity += acceleration * deltaTime;
        bodies[i].position += bodies[i].velocity * deltaTime;
    }
}

void physicsEngine::collisionCheck(std::vector<body> &bodies) {
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            bodies[i].collisionCheck(bodies[j]);
        }
    }
}

