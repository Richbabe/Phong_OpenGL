//灯源片段着色器源代码
#version 330 core
out vec4 FragColor;

uniform vec3 lightColor;//光源颜色

void main()
{
    FragColor = vec4(lightColor, 1.0f);//将向量的4个分量都设置为1.0，表示灯源发出白光
}