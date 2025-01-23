#include "pch.h"
#include "RendererWrapper.h"

namespace Autumn3DWrapper
{
    // Constructor: Initializes the unmanaged Renderer instance
    RendererWrapper::RendererWrapper()
    {
        _renderer = new Renderer();
    }

    // Destructor: Cleans up the unmanaged Renderer instance
    RendererWrapper::~RendererWrapper()
    {
        this->!RendererWrapper();  // Call the finalizer if the object is being garbage collected
    }

    // Finalizer: Ensures that the unmanaged Renderer is cleaned up
    RendererWrapper::!RendererWrapper()
    {
        if (_renderer != nullptr)
        {
            delete _renderer;
            _renderer = nullptr;
        }
    }

    // Wrapper for CreateGLFWWindow
    void RendererWrapper::CreateGLFWWindow(int width, int height)
    {
        _renderer->CreateGLFWWindow(width, height);
    }

    // Wrapper for InitializeOpenGL
    void RendererWrapper::InitializeOpenGL()
    {
        _renderer->InitializeOpenGL();
    }

    // Wrapper for Load3DModel
    void RendererWrapper::Load3DModel(String^ modelPath)
    {
        const std::string& unmanagedPath = msclr::interop::marshal_as<std::string>(modelPath);
        _renderer->Load3DModel(unmanagedPath);
    }

    // Wrapper for Render
    void RendererWrapper::Render()
    {
        _renderer->Render();
    }
}

