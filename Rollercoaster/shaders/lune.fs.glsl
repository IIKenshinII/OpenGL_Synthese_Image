#version 330 core



in vec3 vPositionVC;
in vec3 vNormaleVC;
in vec2 vTexPosition;

uniform sampler2D texture_diffuse1;
out vec3 fFragColor;

void main() 
{
    fFragColor = texture(texture_diffuse1, vTexPosition).xyz;
}