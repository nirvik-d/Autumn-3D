#pragma once

#include <string>
#include <msclr/marshal_cppstd.h> // For marshaling strings between .NET and C++

#include "Renderer.h"  // Include the original unmanaged Renderer class

using namespace System;

namespace Autumn3DWrapper
{
    public ref class RendererWrapper
    {
    private:
        Renderer* _renderer;  // Pointer to the unmanaged Renderer instance

    public:
        // Constructor
        RendererWrapper();

        // Destructor
        ~RendererWrapper();

        // Finalizer (to ensure proper cleanup of unmanaged resources)
        !RendererWrapper();

        // Wrapper for CreateGLFWWindow
        void CreateGLFWWindow(int width, int height);

        // Wrapper for InitializeOpenGL
        void InitializeOpenGL();

        // Wrapper for Load3DModel
        void Load3DModel(String^ modelPath);

        // Wrapper for Render
        void Render();
    };
}
