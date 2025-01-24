#include "Renderer.h"

#include <filesystem>

Renderer::Renderer()
    : m_ScreenWidth(0), m_ScreenHeight(0), m_GlfwWindow(nullptr), m_Shader(nullptr),
    m_DeltaTime(0.0f), m_LastFrame(0.0f), m_LastX(0.0f), m_LastY(0.0f), m_FirstMouse(true)
{
    try
    {
        // Setup the camera
        m_Camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Failed to initialize the camera: " + std::string(e.what()));
    }
}

void Renderer::CreateGLFWWindow(int width, int height)
{
    try
    {
        // Initialize GLFW and configure it
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW!");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // Create GLFW Window
        m_GlfwWindow = glfwCreateWindow(width, height, "Autumn 3D", nullptr, nullptr);
        if (m_GlfwWindow == nullptr)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW Window");
        }
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Error in CreateGLFWWindow: " + std::string(e.what()));
    }

    m_ScreenWidth = width;
    m_ScreenHeight = height;

    m_LastX = m_ScreenWidth / 2.0f;
    m_LastY = m_ScreenHeight / 2.0f;
}

void Renderer::InitializeOpenGL()
{
    try
    {
        if (!m_GlfwWindow)
            throw std::runtime_error("GLFW window is not initialized.");

        glfwMakeContextCurrent(m_GlfwWindow);

        // Set the user pointer to this instance of Renderer
        glfwSetWindowUserPointer(m_GlfwWindow, this);

        glfwSetFramebufferSizeCallback(m_GlfwWindow, FrameBufferSizeCallback);
        glfwSetCursorPosCallback(m_GlfwWindow, MouseCallback);
        glfwSetScrollCallback(m_GlfwWindow, ScrollCallback);

        // Capture the mouse cursor
        glfwSetInputMode(m_GlfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Initialize GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD.");
        }

        // Set OpenGL state
        glEnable(GL_DEPTH_TEST);

        // Setup the shaders
        m_Shader = std::make_shared<Shader>("..\\..\\..\\..\\Shaders\\VertexShader.glsl", "..\\..\\..\\..\\Shaders\\FragmentShader.glsl");
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Error in InitializeOpenGL: " + std::string(e.what()));
    }
}

void Renderer::Load3DModel(const std::string& modelPath)
{
    try
    {
        const auto& model = std::make_shared<Model>(modelPath);
        m_Models.push_back(model);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Failed to load 3D model from path: " + modelPath + ". Error: " + std::string(e.what()));
    }
}

void Renderer::Render()
{
    try
    {
        if (!m_GlfwWindow)
            throw std::runtime_error("GLFW window is not initialized.");

        // Make sure to set up the meshes
        for (const auto& model : m_Models)
        {
            for (const auto& mesh : model->m_Meshes)
            {
                SetupMesh(mesh);
                LoadTextures(mesh);
            }
        }

        while (!glfwWindowShouldClose(m_GlfwWindow))
        {
            // Time calculation
            float currentFrame = static_cast<float>(glfwGetTime());
            m_DeltaTime = currentFrame - m_LastFrame;
            m_LastFrame = currentFrame;

            // Handle input
            ProcessInput();

            // Render
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Use the shader program
            m_Shader->UseProgram();

            // Set view and projection matrices
            glm::mat4 projection = glm::perspective(glm::radians(m_Camera->m_Zoom),
                (float)m_ScreenWidth / (float)m_ScreenHeight,
                0.1f, 100.0f);
            glm::mat4 view = m_Camera->GetViewMatrix();
            m_Shader->SetMat4("projectionMatrix", projection);
            m_Shader->SetMat4("viewMatrix", view);

            for (const auto& model : m_Models)
            {
                glm::mat4 modelMatrix = glm::mat4(1.0f);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
                m_Shader->SetMat4("modelMatrix", modelMatrix);
                DrawModel(model);
            }

            glfwSwapBuffers(m_GlfwWindow);
            glfwPollEvents();
        }

        glfwTerminate();
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Error during rendering: " + std::string(e.what()));
    }
}

void Renderer::ProcessInput()
{
    try
    {
        if (glfwGetKey(m_GlfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_GlfwWindow, true);

        if (glfwGetKey(m_GlfwWindow, GLFW_KEY_W) == GLFW_PRESS)
            m_Camera->ProcessKeyboard(FORWARD, m_DeltaTime);
        if (glfwGetKey(m_GlfwWindow, GLFW_KEY_S) == GLFW_PRESS)
            m_Camera->ProcessKeyboard(BACKWARD, m_DeltaTime);
        if (glfwGetKey(m_GlfwWindow, GLFW_KEY_A) == GLFW_PRESS)
            m_Camera->ProcessKeyboard(LEFT, m_DeltaTime);
        if (glfwGetKey(m_GlfwWindow, GLFW_KEY_D) == GLFW_PRESS)
            m_Camera->ProcessKeyboard(RIGHT, m_DeltaTime);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Error in ProcessInput: " + std::string(e.what()));
    }
}

void Renderer::SetupMesh(const std::shared_ptr<Model::Mesh>& mesh)
{
    try
    {
        // Generate buffers and array objects
        glGenVertexArrays(1, &mesh->m_VAO);
        glGenBuffers(1, &mesh->m_VBO);
        glGenBuffers(1, &mesh->m_EBO);

        if (!mesh->m_VAO || !mesh->m_VBO || !mesh->m_EBO)
            throw std::runtime_error("Failed to generate OpenGL buffers or VAO.");

        // Bind VAO
        glBindVertexArray(mesh->m_VAO);

        // Load vertex data into buffers
        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->m_Vertices.size() * sizeof(Model::Mesh::Vertex), &mesh->m_Vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_Indices.size() * sizeof(unsigned int), &mesh->m_Indices[0], GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Mesh::Vertex), (void*)0); // m_Position

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Mesh::Vertex), (void*)offsetof(Model::Mesh::Vertex, m_Normal)); // m_Normal

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Model::Mesh::Vertex), (void*)offsetof(Model::Mesh::Vertex, m_TexCoords)); // Texture coords

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Mesh::Vertex), (void*)offsetof(Model::Mesh::Vertex, m_Tangent)); // m_Tangent

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Mesh::Vertex), (void*)offsetof(Model::Mesh::Vertex, m_Bitangent)); // m_Bitangent

        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Model::Mesh::Vertex), (void*)offsetof(Model::Mesh::Vertex, m_BoneIDs)); // Bone IDs

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Model::Mesh::Vertex), (void*)offsetof(Model::Mesh::Vertex, m_Weights)); // Weights

        // Unbind VAO
        glBindVertexArray(0);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error during SetupMesh: " << e.what() << std::endl;
        throw;
    }
}

void Renderer::DrawMesh(const std::shared_ptr<Model::Mesh>& mesh)
{
    try
    {
        unsigned int diffuseNr = 0, specularNr = 0, normalNr = 0, heightNr = 0;

        // Bind appropriate textures
        for (unsigned int i = 0; i < mesh->m_TexturesLoaded.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            const std::string& name = mesh->m_TexturesLoaded[i]->m_TextureType;

            // Assign texture unit number based on texture type
            if (name == "texture_diffuse") number = std::to_string(++diffuseNr);
            else if (name == "texture_specular") number = std::to_string(1);
            else if (name == "texture_normal") number = std::to_string(1);
            else if (name == "texture_height") number = std::to_string(1);

            // Set the sampler to the correct texture unit
            int location = glGetUniformLocation(m_Shader->ID, (name + number).c_str());
            if (location == -1)
                throw std::runtime_error("Shader uniform location not found: " + name + number);

            glUniform1i(location, i);
            glBindTexture(GL_TEXTURE_2D, mesh->m_TexturesLoaded[i]->m_TextureID);
        }

        // Draw mesh
        glBindVertexArray(mesh->m_VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh->m_Indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Reset active texture
        glActiveTexture(GL_TEXTURE0);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error during mesh draw: " << e.what() << std::endl;
        throw;
    }
}

void Renderer::LoadTextures(const shared_ptr<Model::Mesh>& mesh)
{
    for (const auto& textureImage : mesh->m_TextureImages)
    {
        auto texture = std::make_shared<Model::Mesh::Texture>();
        unsigned int textureID;
        glGenTextures(1, &textureID);

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.width, textureImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.image.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        texture->m_TextureID = textureID;
        texture->m_TextureType = "texture_diffuse";
        mesh->m_TexturesLoaded.emplace_back(texture);
    }
}

void Renderer::DrawModel(const std::shared_ptr<Model>& model)
{
    for (auto& mesh : model->m_Meshes) {
        try
        {
            DrawMesh(mesh);
        }
        catch (const std::exception& e) {
            std::cerr << "Error drawing mesh: " << e.what() << std::endl;
        }
    }
}

// Static callback functions
void Renderer::FrameBufferSizeCallback(GLFWwindow* m_GlfwWindow, int width, int height)
{
    try
    {
        glViewport(0, 0, width, height);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Error in FrameBufferSizeCallback: " + std::string(e.what()));
    }
}

void Renderer::MouseCallback(GLFWwindow* m_GlfwWindow, double xposIn, double yposIn)
{
    try
    {
        Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(m_GlfwWindow));
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (renderer->m_FirstMouse)
        {
            renderer->m_LastX = xpos;
            renderer->m_LastY = ypos;
            renderer->m_FirstMouse = false;
        }

        float xoffset = xpos - renderer->m_LastX;
        float yoffset = renderer->m_LastY - ypos;

        renderer->m_LastX = xpos;
        renderer->m_LastY = ypos;

        renderer->m_Camera->ProcessMouseMovement(xoffset, yoffset, true);
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Error in MouseCallback: " + std::string(e.what()));
    }
}

void Renderer::ScrollCallback(GLFWwindow* m_GlfwWindow, double xoffset, double yoffset)
{
    try
    {
        Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(m_GlfwWindow));
        renderer->m_Camera->ProcessMouseScroll(static_cast<float>(yoffset));
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error("Error in ScrollCallback: " + std::string(e.what()));
    }
}
