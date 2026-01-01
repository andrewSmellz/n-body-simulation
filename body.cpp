//
// Created by andrew on 12/14/25.
//

#include "body.h"
#include <cmath>

#include "glm/detail/func_geometric.inl"

body::body(glm::vec3 position, glm::vec3 velocity, glm::vec3 colour, float radius, float mass)
    : position(position), velocity(velocity), colour(colour), radius(radius), mass(mass) {
}

SphereData body::generateSphereVertices(float radius, int segments) {
    SphereData sphere;
    constexpr float PI = 3.14159265359f;

    // Generate vertices
    for (int i = 0; i <= segments; i++) {
        float theta = PI * static_cast<float>(i) / static_cast<float>(segments);
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (int j = 0; j <= segments; j++) {
            float phi = 2.0f * PI * static_cast<float>(j) / static_cast<float>(segments);
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            // Spherical to Cartesian coordinates
            float x = radius * sinTheta * cosPhi;
            float y = radius * cosTheta;
            float z = radius * sinTheta * sinPhi;

            sphere.vertices.push_back(x);
            sphere.vertices.push_back(y);
            sphere.vertices.push_back(z);
        }
    }

    // Generate indices for triangles
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < segments; j++) {
            int first = i * (segments + 1) + j;
            int second = first + segments + 1;

            // First triangle
            sphere.indices.push_back(first);
            sphere.indices.push_back(second);
            sphere.indices.push_back(first + 1);

            // Second triangle
            sphere.indices.push_back(second);
            sphere.indices.push_back(second + 1);
            sphere.indices.push_back(first + 1);
        }
    }

    return sphere;
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

body body::createStableOrbit(const body &central, float orbitRadius, float angle, float inclination, glm::vec3 colour,
                             float radius,
                             float mass) {
    constexpr float G = 1000.0f;

    // Calculate 3D position with inclination
    float x = central.position.x + orbitRadius * std::cos(angle) * std::cos(inclination);
    float y = central.position.y + orbitRadius * std::sin(angle) * std::cos(inclination);
    float z = central.position.z + orbitRadius * std::sin(inclination);
    glm::vec3 position(x, y, z);

    // Calculate orbital speed
    float orbitSpeed = std::sqrt(G * central.mass / orbitRadius);

    // Get the radius vector (from central body to this body)
    glm::vec3 radiusVector = position - central.position;
    radiusVector = glm::normalize(radiusVector);

    // For inclined orbits, we need to define the orbital plane normal
    // The orbital plane contains the radius vector and is tilted by inclination
    // We'll use a vector perpendicular to both the radius and a reference vector

    // Create a reference "up" vector for the orbital plane
    glm::vec3 referenceUp(0.0f, 0.0f, 1.0f);

    // If radius vector is parallel to referenceUp, use a different reference
    if (std::abs(glm::dot(radiusVector, referenceUp)) > 0.99f) {
        referenceUp = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    // Calculate velocity direction perpendicular to radius in the orbital plane
    glm::vec3 velocityDirection = glm::normalize(glm::cross(referenceUp, radiusVector));
    // Now cross again to get the actual orbital plane normal
    glm::vec3 orbitalNormal = glm::normalize(glm::cross(radiusVector, velocityDirection));

    // Final velocity perpendicular to radius in the inclined orbital plane
    velocityDirection = glm::normalize(glm::cross(orbitalNormal, radiusVector));

    glm::vec3 velocity = velocityDirection * orbitSpeed;

    return {position, velocity, colour, radius, mass};
}
