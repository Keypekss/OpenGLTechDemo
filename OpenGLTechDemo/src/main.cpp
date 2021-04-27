#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw.h"
#include "IMGUI/imgui_impl_opengl3.h"

#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const * path);

const unsigned int SCR_WIDTH = 1280, SCR_HEIGHT = 720;

// camera
Camera camera = Camera(glm::vec3(0.0f, 10.0f, -20.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool enableCameraMovement = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// max number of lights
const unsigned int NR_POINT_LIGHTS = 4;
const unsigned int NR_SPOT_LIGHTS = 1;

int main()
{
	//initialize glfw and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//window creation
	GLFWwindow* mainWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGLDemo", nullptr, nullptr);
	if (mainWindow == nullptr) {
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(mainWindow);
	glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);
	glfwSetCursorPosCallback(mainWindow, mouse_callback);
	glfwSetKeyCallback(mainWindow, key_callback);
	glfwSetScrollCallback(mainWindow, scroll_callback);
	//glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//initialize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD." << std::endl;
		return -1;
	}

	// initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init((char*)glGetString(330));

	glEnable(GL_DEPTH_TEST);

	// load shaders
	Shader lightingShader("Shaders/lighting.vert", "Shaders/lighting.frag");
	Shader lightCubeShader("Shaders/light_cube.vert", "Shaders/light_cube.frag");
	Shader modelShader("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag");

	// load models
	Model house("Resources/Models/House/house.obj");
	Model ori("Resources/Models/ori/ori.obj");

	// cubes
	// -------------------
	// set up cube vertices
	float vertices[] = {
		  // positions		     // Normals				// texCoords
		 -0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
		  0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
		  0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
		 -0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
									       
		 -0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
		  0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
		  0.5f,  0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
		  0.5f,  0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
		 -0.5f,  0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,    0.0f, 1.0f,
		 -0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
									       
		 -0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
		 -0.5f,  0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
		 -0.5f, -0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
		 -0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
									       
		  0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
		  0.5f, -0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
		  0.5f, -0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
		  0.5f, -0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
		  0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
									       
		 -0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
		  0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
		  0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
		  0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
		 -0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
		 -0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
									       
		 -0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
		  0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
		  0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
		  0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
		 -0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
		 -0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,    0.0f, 1.0f
	};
	// world space positions of our cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};	
	
	unsigned int cubeVAO, VBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	// bind VAO first then bind VBO and configure vertex attributes
	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);

	// normal coord attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	// texture coord attributes
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	//unbind VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// load and create textures
	unsigned int floor = loadTexture("Resources/metal.png");	
	lightingShader.use();
	lightingShader.setInt("floor", 0);

	// floor
	// -----------
	// set up floor vertices
	float floorVertices[] = {
		 -10.0f, -10.0f, -10.0f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
		  10.0f, -10.0f, -10.0f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
		  10.0f,  10.0f, -10.0f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
		  10.0f,  10.0f, -10.0f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
		 -10.0f,  10.0f, -10.0f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
		 -10.0f, -10.0f, -10.0f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f
	};

	unsigned int floorVAO, floorVBO;
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);

	// bind VAO first then bind VBO and configure vertex attributes
	glBindVertexArray(floorVAO);

	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	// position attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);

	// normal coord attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	// texture coord attributes
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	//unbind VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// lights
	// -----------
	// point light positions
	glm::vec3 pointLightPositions[] = {
		glm::vec3(1.61f,  2.41f, -13.09f),
		glm::vec3(1.61f,  8.06f, -13.09f),
		glm::vec3(1.61f,  2.41f, -19.44f),
		glm::vec3(1.61f,  8.06f, -19.44f)
	};

	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

	// ImGUI state
	// ------------
	ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	// house
	glm::vec3 HRotateAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	float HRotateAngle = 360.0f;
	glm::vec3 HTranslate = glm::vec3(1.0f);
	// ori
	glm::vec3 ORotateAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	float ORotateAngle = 90.0f;
	glm::vec3 OTranslate = glm::vec3(14.0f, 0.0f, 1.0f);
	// floor
	glm::vec3 FTranslate = glm::vec3(15.0f, -48.96f, 0.0f);
	glm::vec3 FRotateAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	float FRotate = 90.0f;
	// light colors
	glm::vec3 lightColors[] = {
		glm::vec3(0.15f, 0.43, 0.89), // blue
		glm::vec3(0.15f, 0.89, 0.33), // green
		glm::vec3(1.00f, 1.00, 0.15), // yellow
		glm::vec3(1.00f, 0.15, 0.15)  // red
	};


	//render loop
	while (!glfwWindowShouldClose(mainWindow)) {

		// start ImGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(mainWindow);

		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floor);

		// draw floor
		// ---------------------------
		{
			lightingShader.use();
			// set floor light source uniforms
			DirectionalLight directionalLight = DirectionalLight(lightingShader, glm::vec3(1.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.3f));
			PointLight pointLights[NR_POINT_LIGHTS] = {
				PointLight(lightingShader, lightColors[0], glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f), pointLightPositions[0], 1.0f, 0.09, 0.032),
				PointLight(lightingShader, lightColors[1], glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f), pointLightPositions[1], 1.0f, 0.09, 0.032),
				PointLight(lightingShader, lightColors[2], glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f), pointLightPositions[2], 1.0f, 0.09, 0.032),
				PointLight(lightingShader, lightColors[3], glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f), pointLightPositions[3], 1.0f, 0.09, 0.032)
			};
			SpotLight  spotLights[NR_SPOT_LIGHTS] = {
				SpotLight(lightingShader, glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f), camera.Position, camera.Front, 1.0f, 0.09, 0.032, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)))
			};

			directionalLight.UseLight();

			for (unsigned int i = 0; i < NR_POINT_LIGHTS; ++i)
				pointLights[i].UseLight(i);

			for (unsigned int i = 0; i < NR_SPOT_LIGHTS; ++i)
				spotLights[i].UseLight(i);

			// set floor uniforms			
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			lightingShader.setVec3("viewPos", camera.Position);
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);
			lightingShader.setMat4("model", model);
			Material shinyMaterial = Material(lightingShader);
			shinyMaterial.UseMaterial(4.0f, 256);

			// draw floor
			glBindVertexArray(floorVAO);
			model = glm::mat4(1.0f);
			model = glm::translate(model, FTranslate);
			model = glm::rotate(model, glm::radians(FRotate), FRotateAxis);
			model = glm::scale(model, glm::vec3(5.0f));
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// draw models
		// ------------
		{
			modelShader.use();
			// set model light source uniforms
			DirectionalLight directionalLight = DirectionalLight(modelShader, glm::vec3(1.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.3f));
			PointLight pointLights[NR_POINT_LIGHTS] = {
				PointLight(modelShader, lightColors[0], glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f), pointLightPositions[0], 1.0f, 0.09, 0.032),
				PointLight(modelShader, lightColors[1], glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f), pointLightPositions[1], 1.0f, 0.09, 0.032),
				PointLight(modelShader, lightColors[2], glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f), pointLightPositions[2], 1.0f, 0.09, 0.032),
				PointLight(modelShader, lightColors[3], glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f), pointLightPositions[3], 1.0f, 0.09, 0.032)
			};
			SpotLight  spotLights[NR_SPOT_LIGHTS] = {
				SpotLight(modelShader, glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f), camera.Position, camera.Front, 1.0f, 0.09, 0.032, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)))
			};
			directionalLight.UseLight();

			for (unsigned int i = 0; i < NR_POINT_LIGHTS; ++i)
				pointLights[i].UseLight(i);

			for (unsigned int i = 0; i < NR_SPOT_LIGHTS; ++i)
				spotLights[i].UseLight(i);

			// set model uniforms			
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			modelShader.setVec3("viewPos", camera.Position);
			modelShader.setMat4("projection", projection);
			modelShader.setMat4("view", view);
			// draw house
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(HRotateAngle), HRotateAxis);
			model = glm::translate(model, HTranslate);
			modelShader.setMat4("model", model);
			Material shinyMaterial = Material(modelShader);
			shinyMaterial.UseMaterial(4.0f, 256);
			house.Draw(modelShader);
			// draw ori
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(ORotateAngle), ORotateAxis);
			model = glm::translate(model, OTranslate);
			model = glm::scale(model, glm::vec3(0.5f));
			modelShader.setMat4("model", model);
			ori.Draw(modelShader);
		}

		// lighting
		// ----------------------
		{
			// draw the lamp object
			lightCubeShader.use();
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			lightCubeShader.setMat4("projection", projection);
			lightCubeShader.setMat4("view", view);
			// spin light sources
			// blue
			pointLightPositions[0].x = 1.0f   + cos(glfwGetTime()) * 3.0f;
			pointLightPositions[0].y = 5.0f   + cos(glfwGetTime()) * 3.0f;
			pointLightPositions[0].z = -15.0f + sin(glfwGetTime()) * 3.0f;
			// green				   
			pointLightPositions[1].x = 1.0f   - cos(glfwGetTime()) * 3.0f;
			pointLightPositions[1].y = 5.0f   - cos(glfwGetTime()) * 3.0f;
			pointLightPositions[1].z = -15.0f - sin(glfwGetTime()) * 3.0f;
			// yellow						   
			pointLightPositions[2].x = 1.0f   - sin(glfwGetTime()) * 2.0f;
			pointLightPositions[2].y = 5.0f   - cos(glfwGetTime()) * 2.0f;
			pointLightPositions[2].z = -15.0f - cos(glfwGetTime()) * 2.0f;
			// red						   
			pointLightPositions[3].x = 1.0f   + sin(glfwGetTime()) * 2.0f;
			pointLightPositions[3].y = 5.0f   + cos(glfwGetTime()) * 2.0f;
			pointLightPositions[3].z = -15.0f + cos(glfwGetTime()) * 2.0f;

			// we now draw as many light bulbs as we have point lights.
			glBindVertexArray(lightCubeVAO);
			for (unsigned int i = 0; i < 4; i++) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, pointLightPositions[i]);
				model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
				lightCubeShader.setMat4("model", model);
				lightCubeShader.setVec3("color", lightColors[i]);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		// ImGui
		// ----------		
		{
			ImGui::Begin("Transformations");                          // Create a window called "Hello, world!" and append into it.

			for (size_t i = 0; i < 4; i++) {
				std::string slidername = "Light " + std::to_string(i);
				ImGui::SliderFloat3(slidername.c_str(), glm::value_ptr(pointLightPositions[i]), -25.0f, 25.0f);
			}
			// modify house attributes
			ImGui::SliderFloat3("HRotate Axis ", glm::value_ptr(HRotateAxis), 0.0f, 1.0f);
			ImGui::SliderFloat3("HTransform ", glm::value_ptr(HTranslate), -100.0f, 100.0f);
			ImGui::SliderFloat("HRotate Angle ", &HRotateAngle , 0.0f, 360.0f);

			// modify ori attributes
			ImGui::SliderFloat3("ORotate Axis ", glm::value_ptr(ORotateAxis), 0.0f, 1.0f);
			ImGui::SliderFloat3("OTransform ", glm::value_ptr(OTranslate), -100.0f, 100.0f);
			ImGui::SliderFloat("ORotate Angle ", &ORotateAngle, 0.0f, 360.0f);

			// modify floor attributes
			ImGui::SliderFloat3("FRotate Axis ", glm::value_ptr(FRotateAxis), 0.0f, 1.0f);
			ImGui::SliderFloat3("FTransform", glm::value_ptr(FTranslate), -60.0f, -40.0f);
			ImGui::SliderFloat("FRotate Angle ", &FRotate, 0.0f, 360.0f);

			ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color			

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}

	// shutdown ImGUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//deallocate all resources
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{	
	if (enableCameraMovement)
	{
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
	
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	
		lastX = xpos;
		lastY = ypos;
	
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever a button is pressed, this callback is called
// ----------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
		enableCameraMovement = !enableCameraMovement;
		firstMouse = true;
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	} else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}