#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec3 FragPos;
	vec2 texCoord;
	vec3 Normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
	vs_out.texCoord = aTexCoords;
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;

	gl_Position = projection * view * vec4(vs_out.FragPos, 1.0f);
} 