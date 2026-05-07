#version 420

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 outColor;

layout (binding=0) uniform sampler2D textureSampler;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform bool isLightSource;
uniform bool lightEnabled;

void main(void) {
    if (isLightSource) {
        vec4 texColor = texture(textureSampler, TexCoord);
        outColor = texColor * vec4(lightColor, 1.0);
    } else {
        vec3 resultColor = objectColor;
        
        if (lightEnabled) {
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            
            vec3 ambient = 0.2 * lightColor;
            resultColor = (ambient + diffuse) * objectColor;
        }
        
        outColor = vec4(resultColor, 1.0);
    }
}