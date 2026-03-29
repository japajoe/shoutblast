#include "OpenGL.hpp"
#include "../../../libs/glfw/include/GLFW/glfw3.h"

namespace ShoutBlast
{
    bool OpenGL::Initialize()
    {
        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            return false;

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return true;
    }
}