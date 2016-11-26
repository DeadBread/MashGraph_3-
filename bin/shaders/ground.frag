#version 330

out vec4 outColor;
in vec2 texPoint;
in vec3 normal;
in vec3 light;

uniform int islight;
uniform sampler2D groundTexture;

void main() {


    //TODO: добавить сюда матрицу перевода в объектное пространство и попытаться адекватно реализовать свет
    if (islight == 0)
        outColor = texture(groundTexture, texPoint);
    else
        outColor = texture(groundTexture, texPoint) * max(dot(normal, light), 0);
    //outColor = vec4(0.5, 0.5, 0, 0);
}
