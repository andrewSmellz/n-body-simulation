#ifndef N_BODY_SIMULATION_GL_CONFIG_H
#define N_BODY_SIMULATION_GL_CONFIG_H


struct Config {
    //window settings
    unsigned int screenWidth = 1980;
    unsigned int screenHeight = 1080;
    const char *windowTitle = "N-Body Simulation OpenGL";

    //simulation settings
    unsigned int numBodies = 1;
    bool paused = false;

    //physics settings
    float gravitationalConstant = 1000.0f;
    float timeScale = 1.0f;

    //body generation settings
    float centralBodyMass = 10000.0f;
    float centralBodyRadius = 100.0f;
    float minOrbitRadius = 170.0f;
    float maxOrbitRadius = 400.0f;
    float minBodyMass = 1.0f;
    float maxBodyMass = 10.0f;
    float minBodyRadius = 15.0f;
    float maxBodyRadius = 30.0f;
};

inline Config config;


#endif //N_BODY_SIMULATION_GL_CONFIG_H
