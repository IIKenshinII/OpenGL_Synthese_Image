#version 330

in vec3 vPositionVC;
in vec3 vNormaleVC;
in vec2 vTexPosition;

uniform vec3      uKd;
uniform vec3      uKs;
uniform float     uShininess;
uniform vec3 uLightPos_vs;
uniform vec3 uLightIntensity;
uniform sampler2D texture_diffuse1;

out vec3          fFragColor;


vec3 blinnPhongP()
{

    vec3 lightDirection = normalize(uLightPos_vs - vPositionVC);
    float d = distance(uLightPos_vs, vPositionVC);
    float diffuse = max(dot(normalize(vNormaleVC), lightDirection), 0.0);
    vec3 color;
    if (diffuse > 0.0) {
        color = (uLightIntensity / (d * d)) * (uKd * diffuse +
            uKs * pow(max(dot(normalize(reflect(-lightDirection, vNormaleVC)), normalize(-vPositionVC)), 0.0), uShininess));
    }
    else {
        color = vec3(0.0, 0.0, 0.0);
    }

    return color;
};

void main()
{
    fFragColor = texture(texture_diffuse1, vTexPosition).xyz + blinnPhongP();
}