#version 330

in vec4 point;
out vec2 texPoint;

uniform mat4 camera;

void main() {
    gl_Position = camera * point;
    texPoint = vec2(point.xz);
}
