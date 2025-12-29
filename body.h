//
// Created by andrew on 12/14/25.
//

#ifndef N_BODY_SIMULATION_GL_OBJECT_H
#define N_BODY_SIMULATION_GL_OBJECT_H
#include <vector>

#include "glm/vec3.hpp"


class body {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 colour;
    float mass;
    float radius;

    body(glm::vec3 position, glm::vec3 velocity, glm::vec3 colour, float radius, float mass);

    static std::vector<float> generateCircleVertices(float radius, int segments = 128);

    void collisionCheck(body &other);

    glm::vec3 calculateGravitationalForce(const body &other);

    static body createStableOrbit(const body &central, float orbitRadius, float angle, glm::vec3 colour, float radius,
                                  float mass);
};


#endif //N_BODY_SIMULATION_GL_OBJECT_H
