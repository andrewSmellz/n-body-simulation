//
// Created by andre on 2026-01-21.
//

#ifndef N_BODY_SIMULATION_GL_PHYSICSENGINE_H
#define N_BODY_SIMULATION_GL_PHYSICSENGINE_H
#include <vector>

#include "body.h"


class physicsEngine {
public:
    static void update(std::vector<body> &bodies, double deltaTime);

private:
    static void calculateForces(std::vector<body> &bodies, std::vector<glm::vec3> &forces);

    static void applyForces(std::vector<body> &bodies, std::vector<glm::vec3> &forces, float deltaTime);

    static void collisionCheck(std::vector<body> &bodies);
};


#endif //N_BODY_SIMULATION_GL_PHYSICSENGINE_H
