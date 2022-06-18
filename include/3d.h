#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
class Three_d {
public:
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection;
  Three_d(float rotValue,glm::vec3 axis, glm::vec3 pos, glm::vec3 transl = glm::vec3(0.0f, 0.0f, 0.0f),float fov = 80.0f) {
    set_model(rotValue, axis, transl);
    set_view(pos);
    set_projection(fov);
  }
  void set_model(float rotValue, glm::vec3 axis, glm::vec3 transl = glm::vec3(0.0f, 0.0f, 0.0f));
  void set_view(glm::vec3 pos);
  void set_projection(float fov);
};
