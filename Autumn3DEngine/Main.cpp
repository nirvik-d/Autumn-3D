#include "Renderer.h"

int main()
{
    // Initialize Renderer
    Renderer renderer;

    // Load the model
    renderer.Load3DModel("D:\\Downloads\\3D Models\\shiba.glb");

    // Create a glFW Window
    renderer.CreateGLFWWindow(800, 600);

    // Initialize OpenGL.
    renderer.InitializeOpenGL();

    // Start the render loop
    renderer.Render();

    return 0;
}
