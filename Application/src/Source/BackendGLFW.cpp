#include "../Headers/BackendGLFW.h"

// Constructor
BackendGLFW::BackendGLFW() : window(nullptr) {}

// Destructor
BackendGLFW::~BackendGLFW()
{
    Cleanup();
}

// Initialize OpenGL and GLFW
bool BackendGLFW::Initialize(int width, int height, const char* title) 
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    return true;
}

// Handle GLFW events and prepare for rendering
void BackendGLFW::BeginGlfw() 
{
    glfwPollEvents();
}

// Swap buffers
void BackendGLFW::EndGlfw() 
{
    glfwSwapBuffers(window);
}

// Clean up resources
void BackendGLFW::Cleanup() 
{
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}

/// <summary>
/// returns the GlfwWindow
/// </summary>
/// <returns></returns>
GLFWwindow* BackendGLFW::GetWindow() const 
{
    return window;
}

// Framebuffer size callback
void BackendGLFW::FramebufferSizeCallback(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);
}