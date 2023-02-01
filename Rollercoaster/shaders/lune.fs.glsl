#version 330 core



in vec3 vPositionVC;
in vec3 vNormaleVC;
in vec2 vTexPosition;

uniform vec3      uKd;
uniform vec3      uKs;
uniform float     uShininess;
uniform vec3 uLightDir_vs;
uniform vec3 uLightIntensity;

uniform sampler2D texture_diffuse1;
out vec3 fFragColor;




vec3 blinnPhong()
{
    vec3 color = uLightIntensity * (uKd * max(dot(normalize(vNormaleVC), normalize(uLightDir_vs)), 0.0) +
        uKs * pow(max(dot(normalize(reflect(-normalize(uLightDir_vs), vNormaleVC)), normalize(-vPositionVC)), 0.0), uShininess));
    return color;


};

void main() 
{
    fFragColor = texture(texture_diffuse1, vTexPosition).xyz + blinnPhong();
}