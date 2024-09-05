#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class BackendGLFW {
public:
    BackendGLFW();
    ~BackendGLFW();

    bool Initialize(int width, int height, const char* title);
    void BeginGlfw();
    void EndGlfw();
    void Cleanup();
    GLFWwindow* GetWindow() const;

private:
    GLFWwindow* window;

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
};