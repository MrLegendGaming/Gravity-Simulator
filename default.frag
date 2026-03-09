#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;

uniform vec3 ampColor;


void main()
{
	gl_FragColor = vec4(vec3(0.0f, 0.5f, 1.0f) + (ampColor + vec3(0.5f)), 1.0f);
}