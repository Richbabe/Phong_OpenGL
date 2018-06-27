//灯源顶点着色器源代码
#version 330 core
layout (location = 0) in vec3 aPos;//位置变量的属性位置值为0

uniform mat4 transform;//变换矩阵
uniform mat4 view;//观察矩阵
uniform mat4 projection;//投影矩阵

void main()
{
    gl_Position = projection * view * transform * vec4(aPos, 1.0f);//获得变换后的位置
}