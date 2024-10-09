#pragma once
#include <functional>
#include <GLFW/glfw3.h>

namespace app {
    /// <summary>
    /// Encapsulates a window. I'm using GLFW so much of the functionaly
    /// is done by it.
    /// </summary>
    class Window {
    public:
        /// <summary>
        /// Initializes glfw
        /// Creates a resizable window without decoration.
        /// </summary>
        /// <param name="w"></param>
        /// <param name="h"></param>
        Window(uint32_t w, uint32_t h);
        /// <summary>
        /// Destroy the window
        /// </summary>
        ~Window();
        /// <summary>
        /// The glfw window
        /// </summary>
        /// <returns></returns>
        GLFWwindow* GetWindow()const { return mWindow; }
        /// <summary>
        /// The main event loop. This function blocks until 
        /// finished.
        /// </summary>
        void MainLoop();
        /// <summary>
        /// Callback called by the window main loop initiated when MainLoop is called.
        /// </summary>
        std::function<void(Window*)> OnRender;

    private:
        GLFWwindow* mWindow = nullptr;
    };
}