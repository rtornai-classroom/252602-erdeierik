#version 330 core

layout (location = 0) in vec2 aPosition;

uniform mat4 matModelView;
uniform mat4 matProjection;

out vec2 fragWorldPos;

void main() {
    vec4 worldPos = matModelView * vec4(aPosition, 0.0, 1.0);
    fragWorldPos = worldPos.xy;
    
    gl_Position = matProjection * worldPos;
}