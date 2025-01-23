#ifndef RENDERER_H
#define RENDERER_H

#pragma once

#ifdef AUTUMN3D_EXPORTS
#define AUTUMN3D_API __declspec(dllexport)
#else
#define AUTUMN3D_API __declspec(dllimport)
#endif

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <GLFW/glfw3.h>
#include <iostream>

class Renderer
{
public:
    AUTUMN3D_API Renderer();
    AUTUMN3D_API virtual ~Renderer() {}

    AUTUMN3D_API void CreateGLFWWindow(int width, int height);
    AUTUMN3D_API void InitializeOpenGL();
    AUTUMN3D_API void Load3DModel(const std::string& modelPath);
    AUTUMN3D_API void Render();

private:
    int m_ScreenWidth, m_ScreenHeight;
    float m_DeltaTime, m_LastFrame;
    float m_LastX, m_LastY;
    bool m_FirstMouse;
    GLFWwindow* m_GlfwWindow;
    std::shared_ptr<Camera> m_Camera;
    std::shared_ptr<Shader> m_Shader;
    std::vector<std::shared_ptr<Model>> m_Models;

    // GLFW callback functions
    static void FrameBufferSizeCallback(GLFWwindow* m_GlfwWindow, int width, int height);
    static void MouseCallback(GLFWwindow* m_GlfwWindow, double xposIn, double yposIn);
    static void ScrollCallback(GLFWwindow* m_GlfwWindow, double xoffset, double yoffset);
    void ProcessInput();

    // Model, Mesh and Texture functions
    void SetupMesh(const std::shared_ptr<Model::Mesh>& mesh);
    void DrawMesh(const std::shared_ptr<Model::Mesh>& mesh);
    void LoadTextures(const shared_ptr<Model::Mesh>& mesh);
    void DrawModel(const std::shared_ptr<Model>& model);
};

#endif
