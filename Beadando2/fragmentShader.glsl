#version 330 core

out vec4 outColor;

uniform vec4 uColor;
uniform bool uIsPoint;

void main(void) {
    if (uIsPoint) {
        vec2 ptCoord = gl_PointCoord - vec2(0.5, 0.5);
        if (dot(ptCoord, ptCoord) > 0.25) {
            discard;
        }
    }
    
    outColor = uColor;
}