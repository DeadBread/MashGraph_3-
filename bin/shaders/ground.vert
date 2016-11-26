#version 330

in vec4 point;
out vec2 texPoint;

out vec3 light;
out vec3 normal;

uniform mat4 camera;

void main() {
    gl_Position = camera * point;
    texPoint = vec2(point.xz);

    vec4 lightpos = vec4(1.5, 0.1, 1.5, 1);

    light = normalize(vec3(lightpos - point));
    normal = normalize(vec3(point.x, 1, point.y));

}
