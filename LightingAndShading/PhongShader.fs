//冯氏片段着色器源代码
#version 330 core
out vec4 FragColor;

in vec3 Normal;//法向量
in vec3 FragPos;//片段位置

uniform vec3 lightPos;//光源位置
uniform vec3 objectColor;//物体反射各个颜色分量的大小
uniform vec3 lightColor;//光源颜色
uniform vec3 viewPos;//摄像机位置

uniform float ambientStrength;//环境光照强度
uniform float diffuseStrength;//漫反射强度
uniform float specularStrength;//镜面强度
uniform int shiniess;//反光度

void main()
{
	//环境光照
	vec3 ambient = ambientStrength * lightColor;//环境光照

	//漫反射光照
	vec3 norm = normalize(Normal);//标准化后的法向量
	vec3 lightDir = normalize(lightPos - FragPos);//标准化后的光的方向向量
	float diff = max(dot(norm,lightDir),0.0f);//计算光源对当前片段实际的漫反射影响，即夹角
	vec3 diffuse = diff * lightColor * diffuseStrength;//漫反射分量

	//镜面光照
	vec3 viewDir = normalize(viewPos - FragPos);//视线方向向量
	vec3 reflectDir = reflect(-lightDir,norm);//反射向量
	float spec = pow(max(dot(viewDir,reflectDir),0.0),shiniess);
	vec3 specular = specularStrength *  spec * lightColor;//镜面分量

	vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0f);
}