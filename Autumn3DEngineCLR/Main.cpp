#include "pch.h"
#include "RendererWrapper.h"

using namespace System;
using namespace Autumn3DWrapper;

int main()
{
    RendererWrapper^ wrapper = gcnew RendererWrapper();
    wrapper->Load3DModel("D:\\Downloads\\3D Models\\shiba.glb");
    wrapper->CreateGLFWWindow(800, 600);
    wrapper->InitializeOpenGL();
    wrapper->Render();

    return 0;
}
