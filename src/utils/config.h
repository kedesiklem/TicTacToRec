#include <GLFW/glfw3.h>
#include <algorithm>

float getMinSideScreen(){
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    return std::min(mode->width, mode->height);
}