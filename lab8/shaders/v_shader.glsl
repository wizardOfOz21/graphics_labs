#version 150

in vec3 position;
in vec2 texcoord;
in vec3 normal;

out vec2 Texcoord;
out vec3 Normal;
out vec3 Pos;

uniform mat4 proj;
uniform mat4 model;

void main()
{
    Texcoord = texcoord;
    Normal = mat3(transpose(inverse(model))) * normal;
    Pos = vec3(model*vec4(position,1));
    gl_Position = proj * model * vec4(position, 1);
}

