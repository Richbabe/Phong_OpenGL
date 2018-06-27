//Gouraud片段着色器源代码
#version 330 core
out vec4 FragColor;

in vec3 LightingColor;

uniform vec3 objectColor;//物体反射各个颜色分量的大小

void main(){
	FragColor = vec4(LightingColor * objectColor,1.0f);
}