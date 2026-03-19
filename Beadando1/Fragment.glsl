#version 330 core

in vec2 fragWorldPos;
out vec4 outColor;

uniform int  objectType;       
uniform vec2 circleCenter;     
uniform float radius;          
uniform bool isIntersecting;   

void main() {
    if (objectType == 1) {
        outColor = vec4(0.0, 0.0, 1.0, 1.0);
    } 
    else {
        float d = distance(fragWorldPos, circleCenter);
        
        if (d > radius) {
            discard;
        }
        
        float t = d / radius;
        
        vec3 centerColor;
        vec3 edgeColor;
        
        if (isIntersecting) {
            centerColor = vec3(1.0, 0.0, 0.0);
            edgeColor   = vec3(0.0, 1.0, 0.0);
        } else {
            centerColor = vec3(0.0, 1.0, 0.0);
            edgeColor   = vec3(1.0, 0.0, 0.0);
        }
        
        outColor = vec4(mix(centerColor, edgeColor, t), 1.0);
    }
}