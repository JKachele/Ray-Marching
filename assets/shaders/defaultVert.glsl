#version 460 core
layout (location=0) in vec3 aPos;       // Position
layout (location=1) in vec4 aColor;     // Normal
layout (location=2) in vec2 aUVCoords;  // UV Coordinates

out vec2 fUVCoords;
out vec4 fColor;

void main() {
    fUVCoords = aUVCoords;
    fColor = aColor;
    gl_Position = vec4(aPos.xy, 0.0, 1.0);
}