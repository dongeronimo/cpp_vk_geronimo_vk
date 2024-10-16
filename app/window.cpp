#include "window.h"
#include "glm/glm.hpp"
/// <summary>
/// I need this global variable here bc glfwSetCursorPosCallback does not accept lamba scope capture.
/// </summary>
glm::vec2 gMousePosition;
app::Window* gWindow = nullptr;
void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    assert(gWindow);
    gWindow->OnResize();
}
namespace app {
    app::Window::Window(uint32_t w, uint32_t h)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//don't want to use opengl
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        mWindow = glfwCreateWindow(w, h, "Hello Vulkan", nullptr, nullptr);
        assert(mWindow != nullptr);
        glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xpos, double ypos) {
            gMousePosition.x = static_cast<float>(xpos);
            gMousePosition.y = static_cast<float>(ypos);
            //TODO input: do something with the mouse
        });
        glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods) {
            //TODO input: do something with the mouse
        });
        gWindow = this;
        glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);
    }

    app::Window::~Window()
    {
        glfwDestroyWindow(mWindow);
    }
    void Window::MainLoop()
    {
        while (!glfwWindowShouldClose(mWindow))
        {
            glfwPollEvents();
            OnRender(this);
        }
    }

}
