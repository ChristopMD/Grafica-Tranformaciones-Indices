#version 330 core
out vec4 Color;
in vec4 color_vertice;
void main(){
	Color = color_vertice;
}