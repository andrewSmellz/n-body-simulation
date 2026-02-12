#ifndef N_BODY_SIMULATION_GL_BODY_H
#define N_BODY_SIMULATION_GL_BODY_H
#include <vector>
#include "glm/vec3.hpp"

struct SphereData {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> normals;
};

class body {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 color;
    float mass;
    float radius;

    body(glm::vec3 position, glm::vec3 velocity, glm::vec3 color, float radius, float mass);

    static SphereData generateSphereVertices(float radius, int segments = 1);

    static std::vector<body> generateBodies(unsigned int numBodies);

    void collisionCheck(body &other);

    [[nodiscard]] glm::vec3 calculateGravitationalForce(const body &other) const;

    static body createStableOrbit(const body &central, float orbitRadius, float angle, float inclination,
                                  glm::vec3 color, float radius,
                                  float mass);
};


#endif //N_BODY_SIMULATION_GL_BODY_H
