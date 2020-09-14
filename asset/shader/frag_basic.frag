#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D uniTexture;

void main()
{
  //FragColor = vec4(1.0f, 1.0f, 1.0f,1.0f); // White Color
  FragColor = texture(uniTexture, texCoord);
}