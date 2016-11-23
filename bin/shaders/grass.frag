#version 330

in vec2 textCoords;
in vec2 inst;
out vec4 outColor;

uniform sampler2D grassTexture;

void main() {

    float mc = 0.5 + inst.x / (inst.y * 3);
    vec4 morecolor = vec4(0, mc, 0, 0);
    outColor = texture(grassTexture, textCoords) * morecolor;
    //outColor = vec4(0, 1, 0, 0);
}
