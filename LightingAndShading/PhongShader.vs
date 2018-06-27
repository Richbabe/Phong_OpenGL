//冯氏顶点着色器源代码
#version 330 core
layout (location = 0) in vec3 aPos;//位置变量的属性位置值为0
layout (location = 1) in vec3 aNormal;//法向量的属性位置值为1

uniform mat4 transform;//变换矩阵
uniform mat4 view;//观察矩阵
uniform mat4 projection;//投影矩阵

out vec3 FragPos;//片段位置，即顶点在世界空间中的顶点位置
out vec3 Normal;//顶点法向量

void main()
{
    gl_Position = projection * view * transform * vec4(aPos, 1.0f);//获得变换后的位置
    FragPos = vec3(transform * vec4(aPos, 1.0f));//片段位置为顶点位置属性乘变换矩阵
    Normal = mat3(transpose(inverse(transform))) * aNormal;
}