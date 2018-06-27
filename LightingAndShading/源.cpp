#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_s.h>
#include <Camera.h>

using namespace std;

int screenWidth = 800;//窗口宽度
int screenHeight = 600;//窗口高度

//摄像机参数
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));

//计时器
float deltaTime = 0.0f;//上一帧和当前帧的时间差
float lastFrame = 0.0f;//上一帧

//鼠标参数
bool firstMouse = true;
float lastX = screenWidth / 2.0;
float lastY = screenHeight / 2.0;

glm::vec3 lightPos(0.3f, 0.4f, 0.3f);//光源位置


//在每次窗口大小被调整的时候调用的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//若按下返回键(ESC)则关闭窗口
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	//按下W键
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	//按下S键
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	//按下A键
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	//按下D键
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

//在每次鼠标移动时调用回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	//如果是第一次鼠标输入则更新鼠标初始位置
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	//计算当前帧和上一帧鼠标位置的偏移量
	float xoffset = xpos - lastX;//计算x方向上的偏移量
	float yoffset = lastY - ypos;//计算y方向上的偏移量，注意这里是相反的，因为y坐标从底部往顶部依次增大
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

//当使用鼠标滚轮时调用回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

//当点击鼠标左键时的回调函数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (button) {
			case GLFW_MOUSE_BUTTON_LEFT:
				cout << "click!" << endl;
			default:
				return;
		}
	}
	return;
}

//画点函数
void drawPoints(float fx, float fy, float fz, float color[], Shader shader, int size) {
	float vertices[] = {
		fx, fy, fz,   color[0], color[1], color[2]
	};
	unsigned int points_VBO;//顶点缓冲对象
	unsigned int points_VAO;//顶点数组对象
	glGenVertexArrays(1, &points_VAO);//生成一个VAO对象
	glGenBuffers(1, &points_VBO);//生成一个VBO对象
	glBindVertexArray(points_VAO);//绑定VAO
	//把顶点数组复制到缓冲中供OpengGL使用
	glBindBuffer(GL_ARRAY_BUFFER, points_VBO);//把新创建的缓冲VBO绑定到GL_ARRAY_BUFFER目标上
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//把之前定义的顶点数据points_vertices复制到缓冲的内存中

	//链接顶点属性
	//位置属性，值为0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);//解析顶点数据
	glEnableVertexAttribArray(0);
	//颜色属性，值为1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));//解析顶点数据
	glEnableVertexAttribArray(1);
	shader.use();//激活着色器程序对象
	glPointSize(size);
	glBindVertexArray(points_VAO);//绑定VAO
	glDrawArrays(GL_POINTS, 0, 1);//绘制图元
	//glBindVertexArray(0);
	glDeleteVertexArrays(1, &points_VAO);
	glDeleteBuffers(1, &points_VBO);
}

//画立方体函数
void drawCube(glm::mat4 t,glm::mat4 v,glm::mat4 p,Shader objectShader,Shader lightingShader,glm::vec3 lightColor,float ambientStrength, float diffuseStrength, float specularStrength,int shiniess) {
	//立方体的32个顶点（一共6个面，每个面2个三角形，每个三角形3个顶点）
	float vertices[] = {
		//平面1                  //平面1的法向量
		-0.2f, -0.2f, -0.2f,     0.0f, 0.0f, -1.0f,
		 0.2f, -0.2f, -0.2f,     0.0f, 0.0f, -1.0f,
		 0.2f,  0.2f, -0.2f,     0.0f, 0.0f, -1.0f,
		 0.2f,  0.2f, -0.2f,     0.0f, 0.0f, -1.0f,
		-0.2f,  0.2f, -0.2f,     0.0f, 0.0f, -1.0f,
		-0.2f, -0.2f, -0.2f,     0.0f, 0.0f, -1.0f,
		//平面2                  //平面2的法向量
		-0.2f, -0.2f,  0.2f,     0.0f, 0.0f, 1.0f, 
		 0.2f, -0.2f,  0.2f,     0.0f, 0.0f, 1.0f,
		 0.2f,  0.2f,  0.2f,     0.0f, 0.0f, 1.0f,
		 0.2f,  0.2f,  0.2f,     0.0f, 0.0f, 1.0f,
		-0.2f,  0.2f,  0.2f,     0.0f, 0.0f, 1.0f,
		-0.2f, -0.2f,  0.2f,     0.0f, 0.0f, 1.0f,
		//平面3                  //平面3的法向量
		-0.2f,  0.2f,  0.2f,     -1.0f, 0.0f, 0.0f,
		-0.2f,  0.2f, -0.2f,     -1.0f, 0.0f, 0.0f,
		-0.2f, -0.2f, -0.2f,     -1.0f, 0.0f, 0.0f,
		-0.2f, -0.2f, -0.2f,     -1.0f, 0.0f, 0.0f,
		-0.2f, -0.2f,  0.2f,     -1.0f, 0.0f, 0.0f,
		-0.2f,  0.2f,  0.2f,     -1.0f, 0.0f, 0.0f,
		//平面4                  //平面4的法向量
		 0.2f,  0.2f,  0.2f,     1.0f, 0.0f, 0.0f,
		 0.2f,  0.2f, -0.2f,     1.0f, 0.0f, 0.0f,
		 0.2f, -0.2f, -0.2f,     1.0f, 0.0f, 0.0f,
		 0.2f, -0.2f, -0.2f,     1.0f, 0.0f, 0.0f,
		 0.2f, -0.2f,  0.2f,     1.0f, 0.0f, 0.0f,
		 0.2f,  0.2f,  0.2f,     1.0f, 0.0f, 0.0f,
		//平面5                  //平面5的法向量
		-0.2f, -0.2f, -0.2f,     0.0f, -1.0f, 0.0f,
		 0.2f, -0.2f, -0.2f,     0.0f, -1.0f, 0.0f,
		 0.2f, -0.2f,  0.2f,     0.0f, -1.0f, 0.0f,
		 0.2f, -0.2f,  0.2f,     0.0f, -1.0f, 0.0f,
		-0.2f, -0.2f,  0.2f,     0.0f, -1.0f, 0.0f,
		-0.2f, -0.2f, -0.2f,     0.0f, -1.0f, 0.0f,
		//平面6                  //平面6的法向量
		-0.2f,  0.2f, -0.2f,     0.0f, 1.0f, 0.0f,
		 0.2f,  0.2f, -0.2f,     0.0f, 1.0f, 0.0f,
		 0.2f,  0.2f,  0.2f,     0.0f, 1.0f, 0.0f,
		 0.2f,  0.2f,  0.2f,     0.0f, 1.0f, 0.0f,
		-0.2f,  0.2f,  0.2f,     0.0f, 1.0f, 0.0f,
		-0.2f,  0.2f, -0.2f,     0.0f, 1.0f, 0.0f,
	};
	//立方体
	unsigned int cube_VBO;//顶点缓冲对象
	unsigned int cube_VAO;//顶点数组对象
	glGenVertexArrays(1, &cube_VAO);//生成一个VAO对象
	glGenBuffers(1, &cube_VBO);//生成一个VBO对象
	glBindVertexArray(cube_VAO);//绑定VAO
	//把顶点数组复制到缓冲中供OpengGL使用
	glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);//把新创建的缓冲VBO绑定到GL_ARRAY_BUFFER目标上
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//把之前定义的顶点数据points_vertices复制到缓冲的内存中
    //链接顶点属性
	//位置属性，值为0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);//解析顶点数据
	glEnableVertexAttribArray(0);
	//法向量属性，值为1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));//解析顶点数据
	glEnableVertexAttribArray(1);

	//灯源
	unsigned int light_VAO;//顶点数组对象
	glGenVertexArrays(1, &light_VAO);//生成一个VAO对象
	glBindVertexArray(light_VAO);//绑定VAO
	//把顶点数组复制到缓冲中供OpengGL使用
	glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
	//链接顶点属性
	//位置属性，值为0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);//解析顶点数据
	glEnableVertexAttribArray(0);

	//绘制立方体
	objectShader.use();//激活着色器程序对象
	objectShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	objectShader.setVec3("lightColor", lightColor);
	objectShader.setVec3("lightPos", lightPos);
	objectShader.setVec3("viewPos", camera.Position);
	objectShader.setMat4("projection", p);
	objectShader.setMat4("view", v);
	objectShader.setMat4("transform", t);
	objectShader.setFloat("ambientStrength", ambientStrength);
	objectShader.setFloat("diffuseStrength", diffuseStrength);
	objectShader.setFloat("specularStrength", specularStrength);
	objectShader.setInt("shiniess", shiniess);
	glBindVertexArray(cube_VAO);//绑定VAO
	glDrawArrays(GL_TRIANGLES, 0, 36);//绘制图元

	//绘制灯
	lightingShader.use();
	glm::mat4 model(1);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));
	lightingShader.setMat4("projection", p);
	lightingShader.setMat4("view", v);
	lightingShader.setMat4("transform", model);
	lightingShader.setVec3("lightColor", lightColor);
	glBindVertexArray(light_VAO);//绑定VAO
	glDrawArrays(GL_TRIANGLES, 0, 36);//绘制图元
}

int main() {
	glfwInit();//初始化GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//设置GLFW主版本号为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//设置GLFW次版本号为3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//使用核心模式
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);(MAC系统解除注释)

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Cube", NULL, NULL);//声明窗口对象
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);//GLFW注册回调函数，当点击鼠标mouse_button_callback函数就会被调用
	glfwSetCursorPosCallback(window, mouse_callback);//GLFW注册回调函数，当鼠标一移动mouse_callback函数就会被调用
	glfwSetScrollCallback(window, scroll_callback);//GLFW注册回调函数，当滑动鼠标滚轮scroll_callback函数就会被调用

	//隐藏鼠标光标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//调用任何OpenGL的函数之前需要初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//启用深度测试
	glEnable(GL_DEPTH_TEST);

	//glViewport(0, 0, screenWidth, screenHeight);


	// Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	bool my_tool_active = true;//设置窗口是否有效

	bool isPhongShading = false;//设置是否是PhongShading
	bool isGouraudShading = false;//设置是否是GouraudShading
	bool isMoveableLight = false;//设置是否是移动光源
	bool isViewMode = false;//设置是否是观察模式

	ImVec4 change_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);//imGUI调色板生成的颜色

	Shader PhongShader("PhongShader.vs", "PhongShader.fs");//构造冯氏顶点着色器和冯氏片段着色器并链接
	Shader GouraudShader("GouraudShader.vs", "GouraudShader.fs");//构造Gouraud顶点着色器和Gouraud片段着色器并链接
	Shader lightingShader("lightingShader.vs", "lightingShader.fs");

	//光照参数声明
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);//光源颜色，默认是白光
	float r = 1.0f, g = 1.0f, b = 1.0f;//光源颜色的三个分量
	float ambientStrength = 0.1;//环境光照强度
	float diffuseStrength = 1.0;//漫反射强度
	float specularStrength = 0.5;//镜面强度
	int shiniess = 32;//反光度

	//如果GLFW被要求退出则结束循环
	while (!glfwWindowShouldClose(window))
	{
		//更新定时器
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//输入
		processInput(window);
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();

		//-----------------渲染指令-----------------
		static float f = 0.0f;

		//定义菜单栏
		ImGui::Begin("Cube", &my_tool_active, ImGuiWindowFlags_MenuBar);//开始GUI窗口渲染
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				if (ImGui::MenuItem("Phong Shading", "Ctrl+O")) {
					isPhongShading = true;
					isGouraudShading = false;
					isMoveableLight = false;
					isViewMode = false;
				}
				if (ImGui::MenuItem("Gouraud Shading", "Ctrl+S")) {
					isGouraudShading = true;
					isPhongShading = false;
					isMoveableLight = false;
					isViewMode = false;
				}
				if (ImGui::MenuItem("View Mode", "Ctrk + D")) {
					isViewMode = true;
					isGouraudShading = false;
					isPhongShading = false;
					isMoveableLight = false;
				}
				if (ImGui::MenuItem("Moveable Light", "Ctrl+W")) {
					isMoveableLight = true;
					isGouraudShading = false;
					isPhongShading = false;
					isViewMode = false;
				}
				if (ImGui::MenuItem("Close", "Ctrl+W")) {
					my_tool_active = false;
					glfwSetWindowShouldClose(window, true);//关闭窗口
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		//GUI设置isPhongShading的参数
		if (isPhongShading || isMoveableLight || isViewMode) {
			//设置光源颜色参数
			ImGui::SliderFloat("r", &r, 0.0, 1.0);
			ImGui::SliderFloat("g", &g, 0.0, 1.0);
			ImGui::SliderFloat("b", &b, 0.0, 1.0);
			lightColor.x = r;
			lightColor.y = g;
			lightColor.z = b;

			//设置环境光照强度
			ImGui::SliderFloat("ambientStrength", &ambientStrength, 0.0, 1.0);

			//设置漫反射强度
			ImGui::SliderFloat("diffuseStrength", &diffuseStrength, 0.0, 2.0);

			//设置镜面强度和反光度
			ImGui::SliderFloat("specularStrength", &specularStrength, 0.0, 2.0);
			ImGui::SliderInt("shiniess", &shiniess, 2, 256);
		}

		//GUI设置GouraudShading的参数
		if (isGouraudShading) {
			//设置光源颜色参数
			ImGui::SliderFloat("r", &r, 0.0, 1.0);
			ImGui::SliderFloat("g", &g, 0.0, 1.0);
			ImGui::SliderFloat("b", &b, 0.0, 1.0);
			lightColor.x = r;
			lightColor.y = g;
			lightColor.z = b;

			//设置环境光照强度
			ImGui::SliderFloat("ambientStrength", &ambientStrength, 0.0, 1.0);

			//设置漫反射强度
			ImGui::SliderFloat("diffuseStrength", &diffuseStrength, 0.0, 2.0);

			//设置镜面强度和反光度
			ImGui::SliderFloat("specularStrength", &specularStrength, 0.0, 2.0);
			ImGui::SliderInt("shiniess", &shiniess, 2, 256);
		}

		// Display contents in a scrolling region
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Created By Richbabe.");
		ImGui::BeginChild("Scrolling");
		/*
		for (int n = 0; n < 50; n++)
		ImGui::Text("%04d: Some text", n);
		*/
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::EndChild();
		ImGui::End();//结束GUI窗口渲染

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//设置清空屏幕所用的颜色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清空屏幕的颜色缓冲和深度缓冲

		//Phong Shading
		if (isPhongShading) {
			lightPos.x = 0.3;
			lightPos.y = 0.2;
			lightPos.z = 0.3;
			glm::mat4 trans(1);//变换矩阵
			glm::mat4 view(1);//观察矩阵
			view = camera.GetViewMatrix();
			glm::mat4 projection(1);//投影矩阵
			projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
			drawCube(trans, view, projection, PhongShader, lightingShader, lightColor, ambientStrength, diffuseStrength, specularStrength, shiniess);
		}

		//Gouraud Shading
		if (isGouraudShading) {
			lightPos.x = 0.3;
			lightPos.y = 0.2;
			lightPos.z = 0.3;
			glm::mat4 trans(1);//变换矩阵
			glm::mat4 view(1);//观察矩阵
			view = camera.GetViewMatrix();
			glm::mat4 projection(1);//投影矩阵
			projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
			drawCube(trans, view, projection, GouraudShader, lightingShader, lightColor, ambientStrength, diffuseStrength, specularStrength, shiniess);
		}

		//观察模式
		if (isViewMode) {
			lightPos.x = 0.0;
			lightPos.y = 0.0;
			lightPos.z = 0.3;
			glm::mat4 trans(1);//变换矩阵
			glm::mat4 view(1);//观察矩阵
			view = camera.GetViewMatrix();
			glm::mat4 projection(1);//投影矩阵
			projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
			drawCube(trans, view, projection, PhongShader, lightingShader, lightColor, ambientStrength, diffuseStrength, specularStrength, shiniess);
		}

		//可移动光源
		if (isMoveableLight) {
			float radius = 1.0f;
			lightPos.x = sin(glfwGetTime()) * radius;
			lightPos.y = sin(glfwGetTime() / 2.0) * 1.0f;
			lightPos.z = cos(glfwGetTime()) * radius;
			glm::mat4 trans(1);//变换矩阵
			glm::mat4 view(1);//观察矩阵
			view = camera.GetViewMatrix();
			glm::mat4 projection(1);//投影矩阵
			projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
			drawCube(trans, view, projection, PhongShader, lightingShader, lightColor, ambientStrength, diffuseStrength, specularStrength, shiniess);
		}

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		//检查并调用事件、交换缓冲
		glfwSwapBuffers(window);//交换颜色缓冲
		glfwPollEvents();//检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。
	}
	//释放资源;
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();//正确释放/删除之前的分配的所有资源

	return 0;
}