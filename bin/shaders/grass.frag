#version 330

in vec2 textCoords;
out vec4 outColor;

uniform sampler2D grassTexture;

void main() {
    outColor = texture(grassTexture, textCoords);
    //outColor = vec4(0, 1, 0, 0);
}
