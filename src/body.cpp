#include "body.h"
#include "config.h"
#include <cmath>
#include <random>
#include "glm/detail/func_geometric.inl"

body::body(glm::vec3 position, glm::vec3 velocity, glm::vec3 colour, float radius, float mass)
    : position(position), velocity(velocity), colour(colour), mass(mass), radius(radius) {
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
            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
            sphere.normals.push_back(normal.x);
            sphere.normals.push_back(normal.y);
            sphere.normals.push_back(normal.z);
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

std::vector<body> body::generateBodies(unsigned int numBodies) {
    std::vector<body> bodies;
    body sun(
        glm::vec3(CONFIG.screenWidth / 2.0f, CONFIG.screenHeight / 2.0f, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(1, 1, 0),
        CONFIG.centralBodyRadius,
        CONFIG.centralBodyMass
    );
    bodies.push_back(sun);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> radius_dist(CONFIG.minOrbitRadius, CONFIG.maxOrbitRadius);
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> inclination_dist(-0.8f, 0.8f);
    std::uniform_real_distribution<float> mass_dist(CONFIG.minBodyMass, CONFIG.maxBodyMass);
    std::uniform_real_distribution<float> body_radius_dist(CONFIG.minBodyRadius, CONFIG.maxBodyRadius);
    std::uniform_real_distribution<float> z_dist(-200.0f, 200.0f);


    for (size_t i = 0; i < numBodies; i++) {
        float radius = radius_dist(gen);
        float angle = angle_dist(gen);
        float inclination = inclination_dist(gen);
        glm::vec3 colour(std::sin(i), std::cos(i), 1);
        float mass = mass_dist(gen);
        float bodyRadius = body_radius_dist(gen);
        body b = body::createStableOrbit(sun, radius, angle, inclination, colour, bodyRadius, mass);
        b.position.z += z_dist(gen);
        bodies.push_back(b);
    }
    return bodies;
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
    //float G = 6.67430e-11f; true value
    float G = CONFIG.gravitationalConstant;

    float distance = glm::distance(this->position, other.position);
    float force = (G * this->mass * other.mass) / (distance * distance);
    glm::vec3 forceDirection = other.position - this->position;
    forceDirection = glm::normalize(forceDirection);
    return forceDirection * force;
}

body body::createStableOrbit(const body &central, float orbitRadius, float angle, float inclination, glm::vec3 colour,
                             float radius,
                             float mass) {
    float G = CONFIG.gravitationalConstant;

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
