#version 330

out vec4 outColor;
in vec2 texPoint;
uniform sampler2D groundTexture;

void main() {

    outColor = texture(groundTexture, texPoint);
    //outColor = vec4(0.5, 0.5, 0, 0);
}
