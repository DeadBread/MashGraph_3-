#version 330

in vec2 textCoords;
in vec2 inst;
out vec4 outColor;

in vec3 l;
in vec3 n;

uniform sampler2D grassTexture;

void main() {

    //const vec4 diffColor = vec4 ( 0.5, 0.0, 0.0, 1.0 );

    float mc = 0.75 + inst.x / (inst.y * 4);
    vec4 morecolor = vec4(0, mc, 0, 0);

    vec4 dark = vec4(1.0);
    dark.y = textCoords.x / 2.0 + 0.5;

    vec3 n2   = normalize ( n );
    vec3 l2   = normalize ( l );

    //vec4 clr =
    //dark.x = dark.y;

    outColor = texture(grassTexture, textCoords) * morecolor * dark * max ( dot ( n2, l2 ), 0.0 );

    //outColor = diffColor * max ( dot ( n2, l2 ), 0.0 );

    //outColor = vec4(0, 1, 0, 0);
}
