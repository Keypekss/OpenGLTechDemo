#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec2 texCoord;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
	FragPos = vec3(model * vec4(aPos, 1.0f));
	texCoord = vec2(aTexCoord.x, aTexCoord.y);
	Normal = mat3(transpose(inverse(model))) * aNormal;

	gl_Position = projection * view * vec4(FragPos, 1.0f);
} 