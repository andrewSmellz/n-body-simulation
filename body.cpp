//
// Created by andrew on 12/14/25.
//

#include "body.h"
#include <cmath>

#include "glm/detail/func_geometric.inl"

body::body(glm::vec3 position, glm::vec3 velocity, glm::vec3 colour, float radius, float mass)
    : position(position), velocity(velocity), colour(colour), radius(radius), mass(mass) {
}

std::vector<float> body::generateCircleVertices(float radius, int segments) {
    std::vector<float> vertices;
    constexpr float PI = 3.14159265359f;

    for (int i = 0; i < segments; i++) {
        const float angle = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
        vertices.push_back(radius * std::cos(angle)); // x
        vertices.push_back(radius * std::sin(angle)); // y
    }

    return vertices;
}

void body::collisionCheck(body &other) {
    float distance = glm::distance(this->position, other.position);
    if (distance > this->radius + other.radius) return;

    // Collision normal (from other to this)
    glm::vec3 collisionNormal = glm::normalize(this->position - other.position);

    // Separate overlapping bodies based on mass
    float overlap = (this->radius + other.radius) - distance;
    float totalMass = this->mass + other.mass;
    this->position += collisionNormal * (overlap * other.mass / totalMass);
    other.position -= collisionNormal * (overlap * this->mass / totalMass);

    // Relative velocity
    glm::vec3 relativeVelocity = this->velocity - other.velocity;
    float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

    // Don't resolve if velocities are separating
    if (velocityAlongNormal > 0) return;
    // Coefficient of restitution (1.0 = perfectly elastic, 0.0 = perfectly inelastic)
    float restitution = 1.0f;

    // Calculate impulse scalar (from momentum conservation)
    float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal;
    impulseMagnitude /= (1.0f / this->mass + 1.0f / other.mass);

    // Apply impulse
    glm::vec3 impulse = impulseMagnitude * collisionNormal;
    this->velocity += impulse / this->mass;
    other.velocity -= impulse / other.mass;
}

glm::vec3 body::calculateGravitationalForce(const body &other) {
    //constexpr float G = 6.67430e-11f; true value
    constexpr float G = 1000.0f;

    float distance = glm::distance(this->position, other.position);
    float force = (G * this->mass * other.mass) / (distance * distance);
    glm::vec3 forceDirection = other.position - this->position;
    forceDirection = glm::normalize(forceDirection);
    return forceDirection * force;
}

body body::createStableOrbit(const body &central, float orbitRadius, float angle, glm::vec3 colour, float radius,
                             float mass) {
    constexpr float G = 1000.0f;
    float x = central.position.x + orbitRadius * std::cos(angle);
    float y = central.position.y + orbitRadius * std::sin(angle);
    glm::vec3 position(x, y, 0);
    float orbitSpeed = std::sqrt(G * central.mass / orbitRadius);
    float vx = -orbitSpeed * std::sin(angle);
    float vy = orbitSpeed * std::cos(angle);
    glm::vec3 velocity(vx, vy, 0);

    return {position, velocity, colour, radius, mass};
}



