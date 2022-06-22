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

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;
float zoom = 0.0f;
// function prototypes
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void gen_texture(unsigned int *texture, const char *path, const char *type);
void processInput(GLFWwindow *window);
int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  glViewport(0, 0, 800, 600);
  unsigned int indices[] = {
      // note that we start from 0!
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  float pyramid_vertices[] = {
      -1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,  1.0f, 0.0f,
      0.0f,  0.0f, -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f,  0.0f, 0.0f,
      1.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
      -1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
      0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f,
      0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
  };
  // float square_vertices[] = {
  //     -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
  //     0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
  //     -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
  //
  //     -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
  //     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  //     -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
  //
  //     -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
  //     -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
  //     -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,
  //
  //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
  //     0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
  //     0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  //
  //     -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
  //     0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
  //     -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
  //
  //     -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
  //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  //     -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};
  // glm::vec3 cubePositions[] = {
  //     glm::vec3(0.0f, 0.0f, -1.0f),   glm::vec3(2.0f, 5.0f, -15.0f),
  //     glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
  //     glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
  //     glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
  //     glm::vec3(1.5f, 0.2f, 0.0f),    glm::vec3(-1.3f, 1.0f, -1.5f)};
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // glfwSetCursorPosCallback(window, mouse_callback);
  // glfwSetScrollCallback(window, scroll_callback);
  glEnable(GL_DEPTH_TEST);
  // VERTEX ARRAY OBJECT
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // VERTEX BUFFER OBJECT
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  // 0. copy our vertices array in a buffer for OpenGl to use
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_vertices), pyramid_vertices, GL_STATIC_DRAW);

  // ELEMENT BUFFER OBJECT
  unsigned int EBO;
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Texutre
  stbi_set_flip_vertically_on_load(true);
  unsigned int texture2;
  gen_texture(&texture2, "images/awesomeface.png", "png");
  unsigned int texture1;
  gen_texture(&texture1, "images/container.jpg", "jpg");

  // LINKING VERTEX ATTRIBUTES
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // initialize shaders
  Shader ourShader("shaders/vshader.vs", "shaders/fshader.fs");
  ourShader.use();
  ourShader.setInt("texture1", 0);
  ourShader.setInt("texture2", 1); // or with shader class

  Three_d *three_d =
      new Three_d(sin(glfwGetTime()) * 20, glm::vec3(0.3f, 0.5f, 1.0f),
                  glm::vec3(0.0f, 0.0f, -2.0f));

  // mouse cursor visible or not
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  // main loop where the rendering happens
  while (!glfwWindowShouldClose(window)) {
    // input
    processInput(window);
    // rendering
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // activate shader
    ourShader.use();

    // transformations
    int modelLoc = glGetUniformLocation(ourShader.ID, "model");
    int viewLoc = glGetUniformLocation(ourShader.ID, "view");
    int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
    glm::mat4 view = glm::mat4(
        1.0f); // make sure to initialize matrix to identity matrix first
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // 3.now draw the objectconst float radius = 10.0f;

      three_d->set_model((glfwGetTime()) * 180,
                         glm::vec3( 0.5f , 0.6f ,-0.3f),
                         glm::vec3(0.9f , 1.8f, -1.0f));
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(three_d->model));
      three_d->set_projection(fov);
      glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
                         glm::value_ptr(three_d->projection));
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 12);
  //   for (int i = 0; i < 10; i++) {
  //     float k = i * 0.1;
  //     three_d->set_model((glfwGetTime()) * 180 + i,
  //                        glm::vec3(k + 2 * i, k + 3 * i, k + 4 * i),
  //                        cubePositions[i]);
  //     glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(three_d->model));
  //     three_d->set_projection(fov);
  //     glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
  //                        glm::value_ptr(three_d->projection));
  //   }
    glfwSwapBuffers(window); // concept of double buffers (section: 4.3)
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window) {
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
      GLFW_PRESS) // if not pressed glfwGetKey returns 'GLFW_RELEASE' instead of
    // 'GLFW_PRESS'
    glfwSetWindowShouldClose(window, true);
}
void gen_texture(unsigned int *texture, const char *path, const char *type) {
  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, *texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // load and generate the texture
  int width, height, nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (data) {
    if (strcmp(type, "png") == 0) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, data);
    } else {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    std::cout << "texture loaded" << std::endl;
  } else {
    std::cout << "failed to load texture" << std::endl;
  }
  stbi_image_free(data);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
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
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  fov -= (float)yoffset;
  if (fov < 1.0f)
    fov = 1.0f;
  if (fov > 45.0f)
    fov = 45.0f;
}
