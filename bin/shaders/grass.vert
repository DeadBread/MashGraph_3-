#version 330

in vec4 point;
in vec2 position;
in vec4 variance;

out vec2 textCoords;
out vec2 inst;

out vec3 light;
out vec3 norm;

uniform mat4 camera;
uniform int instanceNum;


void main() {

    mat4 scaleMatrix = mat4(1.0);
    scaleMatrix[0][0] = 0.01;
    scaleMatrix[1][1] = 0.1;

    mat4 positionMatrix = mat4(1.0);
    positionMatrix[3][0] = position.x;
    positionMatrix[3][2] = position.y;

    mat4 turn = mat4(0.0);

    float i;
    i = atan(gl_InstanceID);

    turn[0][0] = cos(gl_InstanceID);
    turn[0][2] = -sin(gl_InstanceID);
    turn[1][1] = 1.0;
    turn[2][0] = sin(gl_InstanceID);
    turn[2][2] = cos(gl_InstanceID);
    turn[3][3] = 1.0;

    mat4 reScale = mat4(0.0);

    float factor = 1 - (gl_InstanceID / (instanceNum * 2.0));
    reScale[0][0] = 1;
    reScale[1][1] = factor;
    reScale[2][2] = 1;
    reScale[3][3] = 1;

    textCoords = vec2(point.yx);
    inst.x = gl_InstanceID;
    inst.y = instanceNum;

    //float p = exp( point.y / 2) - 1;

    vec4 normal = vec4(- point.z, point.yx, 1);

    vec4 lightpos = vec4(1.5, 1, 1.5, 1);

    float p = pow(point.y, 3) / 20 + pow (point.y, 2) / 30 + pow (point.y, 1.5) / 15; // + pow(point.y, 2);

    mat4 mvMatrix = positionMatrix * reScale * turn * scaleMatrix;

    mat4 nMatrix = transpose(inverse (mvMatrix));

    vec3 pos = vec3 (mvMatrix * point);

    light = normalize(vec3(lightpos) - pos);

    norm = normalize(vec3( nMatrix * normal ));

	gl_Position = camera * (positionMatrix * reScale * turn * scaleMatrix * point + variance * p);
}
