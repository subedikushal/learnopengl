#include "../include/shader.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../include/3d.h"
#include "../include/stb_image.h"
#include <vector>
// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool firstMouse = true;
float yaw =
    -90.0f; // rotating about y-axis or angle made with x axis in xz-plane
float pitch = 0.0f; // rotationg about x-axis
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;
float zoom = 0.0f;
// sphere
int sectorCount{36}, stackCount{18};
std::vector<float> gen_sphere_vertices()
{
    std::vector<float> vertices;
    float x, y, z, xy, radius{1.0f}; // vertex position
    float s, t;                      // vertex texCoord
    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;
    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);        // r * cos(u)
        z = radius * sinf(stackAngle);         // r * sin(u)
        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but
        // different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep; // starting from 0 to 2pi
            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            vertices.push_back(s);
            vertices.push_back(t);
        }
    }
    return vertices;
}
std::vector<unsigned int> gen_sphere_indices()
{
    std::vector<unsigned int> indices;
    int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1); // beginning of current stack
        k2 = k1 + sectorCount + 1;  // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    return indices;
}
// function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void gen_texture(unsigned int* texture, const char* path, const char* type);
void processInput(GLFWwindow* window);
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);

    float square_vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
        -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
        0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
        1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f,
        0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
        0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  -0.5f, -0.5f,
        0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f,
        0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,
        1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f,
        0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f,
        0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
        -0.5f, 1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,
        0.0f,  0.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,
        0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f,
        0.5f,  0.0f,  -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
        -1.0f, 0.0f,  1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
        0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
        -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
        1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f
        // -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
        // 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
        //
        // -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
        // 0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,
        //
        // -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
        // -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,
        //
        // 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,
        // 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
        //
        // -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,
        // 0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,
        //
        // -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,
        // 0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,
    };
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // mouse input callbacks
    //  comment them if want to disable cursor input
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glEnable(GL_DEPTH_TEST);
    // VERTEX ARRAY OBJECT
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    // VERTEX BUFFER OBJECT
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // LINKING VERTEX ATTRIBUTES
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);

    // initialize shaders
    Shader ourShader("shaders/vshader.vs", "shaders/fshader.fs");
    Shader lightCubeShader("shaders/lightSource_vshader.vs",
                           "shaders/lightSource_fshader.fs");

    Three_d* three_d =
        new Three_d(glfwGetTime() * 20.0f, glm::vec3(0.0f, 0.0f, 1.0f),
                    glm::vec3(0.0f, 0.0f, -0.1f));
    float theta = 0.0;
    float dtheta = 0.5f;
    int c = 0;
    while (!glfwWindowShouldClose(window))
    {
        if (theta > 360)
        {
            theta = 0;
        }
        if (theta >= 180)
        {
            c = 3;
        }
        theta += dtheta;
        // input
        processInput(window);
        // rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        ourShader.use();

        // ourShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        // ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("viewPos", cameraPos);
        ourShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        ourShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        ourShader.setFloat("material.shininess", 32.0f);
        ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darkened
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        // transformations
        // setting projection uniform
        int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        three_d->set_projection(fov);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
                           glm::value_ptr(three_d->projection));

        // setting view uniform
        int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        glm::mat4 view = glm::mat4(
            1.0f); // make sure to initialize matrix to identity matrix first
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // setting model uniform
        int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        three_d->set_model(glfwGetTime() * 40, glm::vec3(0.0f, 1.0f, 0.0f),
                           glm::vec3(0.0f, 0.0f, -1.8f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
                           glm::value_ptr(three_d->model));

        // binding vao and drawing
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        float lightPosX = 1.8f * cos(theta * (M_PI / 180));
        float lightPosY = 0.0f;
        float lightPosZ = (3.8+c)  * sin(theta * (M_PI / 180));

        ourShader.setVec3("light.position", lightPosX, lightPosY, lightPosZ);
        three_d->set_model(glfwGetTime() * 40, glm::vec3(0.0f, 1.0f, 0.0f),
                           glm::vec3(lightPosX, lightPosY, lightPosZ));
        // three_d->set_model(glfwGetTime() * 40, glm::vec3(0.0f, 1.0f, 0.0f),
        //                    glm::vec3(1.8, 0.0, -1.8f));
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", three_d->projection);
        lightCubeShader.setMat4("view", view);
        glm::mat4 model =
            glm::scale(three_d->model, glm::vec3(0.4f)); // a smaller cube
        lightCubeShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window); // concept of double buffers (section: 4.3)
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window)
{
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -=
            glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos +=
            glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) ==
        GLFW_PRESS) // if not pressed glfwGetKey returns 'GLFW_RELEASE' instead
                    // of
        // 'GLFW_PRESS'
        glfwSetWindowShouldClose(window, true);
}
void gen_texture(unsigned int* texture, const char* path, const char* type)
{
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        if (strcmp(type, "png") == 0)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "texture loaded" << std::endl;
    }
    else
    {
        std::cout << "failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw += xoffset;
    pitch += yoffset;
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}
