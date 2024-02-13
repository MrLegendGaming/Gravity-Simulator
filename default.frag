#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 ampColor;


void main()
{
	gl_FragColor = vec4(ourColor + ampColor, 1.0f);

}