#version 330

in vec2 textCoords;
in vec2 inst;
out vec4 outColor;

in vec3 light;
in vec3 norm;

uniform sampler2D grassTexture;
uniform int islight;

void main() {

    float mc = 0.66 + inst.x / (inst.y * 3);
    vec4 morecolor = vec4(0, mc, 0, 0);

    if (islight == 0)
        outColor = texture(grassTexture, textCoords) * morecolor;// * max ( dot ( norm, light ), 0.3 );
    else
        outColor = texture(grassTexture, textCoords) * morecolor * max ( dot ( norm, light ), 0.3 );


}
