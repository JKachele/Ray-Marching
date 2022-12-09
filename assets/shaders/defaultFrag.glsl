#version 460 core

in vec2 fUVCoords;
in vec4 fColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

out vec4 color;

void main() {
    color = texture(texture2, fUVCoords) * fColor;
//    color = fColor;
}