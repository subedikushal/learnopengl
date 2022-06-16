#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
class Three_d {
public:
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection;
  Three_d(float rotValue,glm::vec3 axis, glm::vec3 pos) {
    set_model(rotValue, axis);
    set_view(pos);
    set_projection();
  }
  void set_model(float rotValue, glm::vec3 axis);
  void set_view(glm::vec3 pos);
  void set_projection();
};
