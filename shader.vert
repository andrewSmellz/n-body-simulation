#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aInstancePos;
layout (location = 2) in float aInstanceRadius;
layout (location = 3) in vec3 aInstanceColour;

uniform mat4 view;
uniform mat4 projection;

out vec3 fragColour;

void main() {
    vec2 scaledPos = aPos * aInstanceRadius + aInstancePos;
    gl_Position = projection * view *  vec4(scaledPos, 0.0, 1.0);
    fragColour = aInstanceColour;
}