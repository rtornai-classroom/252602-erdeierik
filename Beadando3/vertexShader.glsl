#version 420

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main(void) {
    FragPos = vec3(matModel * vec4(aPosition, 1.0));
    Normal = mat3(transpose(inverse(matModel))) * aNormal;
    TexCoord = aTexture;
    gl_Position = matProjection * matView * matModel * vec4(aPosition, 1.0);
}