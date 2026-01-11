#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aInstancePos;
layout (location = 2) in float aInstanceRadius;
layout (location = 3) in vec3 aInstanceColour;
layout (location = 4) in vec3 aNormal;

uniform mat4 view;
uniform mat4 projection;

out vec3 fragColour;
out vec3 fragPos;
out vec3 normal;

void main() {
    // Scale the sphere vertex by the instance radius
    vec3 scaledPos = aPos * aInstanceRadius;

    // Add to instance position to get world position
    vec3 worldPos = aInstancePos + scaledPos;

    // Transform to clip space
    gl_Position = projection * view * vec4(worldPos, 1.0);

    fragColour = aInstanceColour;

    normal = aNormal;
    fragPos = aPos;
}