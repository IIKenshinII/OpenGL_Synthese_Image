#version 330 core

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aTexPosition;

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;

out vec3 vPositionVC;
out vec3 vNormaleVC;
out vec2 vTexPosition;

void main()
{
    vPositionVC = (uMVMatrix*vec4(aVertexPosition, 1) ).xyz;
    vNormaleVC   = (uNormalMatrix*vec4(aVertexNormal, 0)).xyz;
    vTexPosition = aTexPosition;

    gl_Position = uMVPMatrix * vec4(aVertexPosition, 1);
}
