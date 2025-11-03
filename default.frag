#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;

uniform vec3 ampColor;


void main()
{
	gl_FragColor = vec4(vec3(1.0f, 0.4f, 0.7f) - vec3(0.3f, 0.5f, 0.5f) * ampColor, 1.0f);
}