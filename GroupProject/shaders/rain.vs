#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexxCoords;

uniform mat4 projection;
uniform mat4 view;
//uniform mat4 model;

void main()
{
    gl_Position = projection * view * aInstanceMatrix * vec4(position, 1.0f); 
    TexxCoords = texCoords;
}
