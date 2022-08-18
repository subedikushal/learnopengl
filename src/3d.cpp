#include "../include/3d.h"
#include <cstddef>
#include <iostream>
void Three_d::set_model(float rotValue, glm::vec3 axis, glm::vec3 transl) {
  glm::mat4 identity = glm::mat4(1.0f);
  identity = glm::translate(identity, transl);
  this->model = glm::rotate(identity, glm::radians(rotValue), axis);
}

void Three_d::set_projection(float fov) {
  this->projection =
      glm::perspective(glm::radians(fov), 1920.0f / 1080.0f, 0.1f, 100.0f);
}
