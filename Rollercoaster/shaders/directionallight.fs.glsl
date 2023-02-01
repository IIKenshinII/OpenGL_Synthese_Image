#version 330

in vec3 vPositionVC;
in vec3 vNormaleVC;
in vec2 vTexPosition;

uniform vec3      uKd;
uniform vec3      uKs;
uniform float     uShininess;
uniform vec3 uLightDir_vs;
uniform vec3 uLightIntensity;

out vec3          fFragColor;

vec3 blinnPhong()
{
    vec3 color = uLightIntensity * (uKd * max(dot(normalize(vNormaleVC), normalize(uLightDir_vs)), 0.0) +    
                                    uKs * pow(max(dot(normalize(reflect(-normalize(uLightDir_vs), vNormaleVC)), normalize(-vPositionVC)), 0.0), uShininess)); 
    return color;


};

void main()
{
    fFragColor =vec3(0.69,0.88,0.9)+blinnPhong();
}