#version 460 core
out vec4 FragColor;

in vec3 fragColour;

void main() {
    FragColor = vec4(fragColour, 1.0);
}