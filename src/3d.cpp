#include "../include/3d.h"
#include <iostream>
void Three_d::set_model(float rotValue, glm::vec3 axis) {
  glm::mat4 identity = glm::mat4(1.0f);
  this->model = glm::rotate(identity, glm::radians(rotValue), axis);
}

void Three_d::set_view(glm::vec3 pos) {
  glm::mat4 identity = glm::mat4(1.0f);
  this->view = glm::translate(identity, pos);
}
void Three_d::set_projection() {
  this->projection =
      glm::perspective(glm::radians(80.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
}
