//Gouraud顶点着色器源代码
#version 330 core
layout (location = 0) in vec3 aPos;//位置变量的属性位置值为0
layout (location = 1) in vec3 aNormal;//法向量的属性位置值为1

out vec3 LightingColor;//通过Gouraud着色后的光照颜色

uniform mat4 transform;//变换矩阵
uniform mat4 view;//观察矩阵
uniform mat4 projection;//投影矩阵

uniform vec3 lightPos;//光源位置
uniform vec3 lightColor;//光源颜色
uniform vec3 viewPos;//摄像机位置

uniform float ambientStrength;//环境光照强度
uniform float diffuseStrength;//漫反射强度
uniform float specularStrength;//镜面强度
uniform int shiniess;//反光度

void main()
{
    gl_Position = projection * view * transform * vec4(aPos, 1.0f);//获得变换后的位置

    vec3 Position = vec3(transform * vec4(aPos, 1.0f));//片段位置为顶点位置属性乘变换矩阵
    vec3 Normal = mat3(transpose(inverse(transform))) * aNormal;

    //环境光照
	vec3 ambient = ambientStrength * lightColor;//环境光照

	//漫反射光照
	vec3 norm = normalize(Normal);//标准化后的法向量
	vec3 lightDir = normalize(lightPos - Position);//标准化后的光的方向向量
	float diff = max(dot(norm,lightDir),0.0f);//计算光源对当前片段实际的漫发射影响，即夹角
	vec3 diffuse = diff * lightColor * diffuseStrength;//漫反射分量

	//镜面光照
	//float specularStrength = 1.0;//镜面强度,设的比冯氏模型高是为了显示Gouraud的不同
	vec3 viewDir = normalize(viewPos - Position);//视线方向向量
	vec3 reflectDir = reflect(-lightDir,norm);//反射向量
	float spec = pow(max(dot(viewDir,reflectDir),0.0),shiniess);
	vec3 specular = specularStrength *  spec * lightColor;//镜面分量

	LightingColor = ambient + diffuse + specular;

}