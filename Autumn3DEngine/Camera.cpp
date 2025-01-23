#include "Camera.h"

// Camera constructor with glm::vec3 position and up vector
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    m_Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    m_MovementSpeed(SPEED),
    m_MouseSensitivity(SENSITIVITY),
    m_Zoom(ZOOM)
{
    m_Position = position;
    m_WorldUp = up;
    m_Yaw = yaw;
    m_Pitch = pitch;
    UpdateCameraVectors();
}

// Camera constructor with scalar position and up values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
    m_Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    m_MovementSpeed(SPEED),
    m_MouseSensitivity(SENSITIVITY),
    m_Zoom(ZOOM)
{
    m_Position = glm::vec3(posX, posY, posZ);
    m_WorldUp = glm::vec3(upX, upY, upZ);
    m_Yaw = yaw;
    m_Pitch = pitch;
    UpdateCameraVectors();
}

// Process keyboard input
void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = m_MovementSpeed * deltaTime;

    try
    {
        if (direction == FORWARD)
            m_Position += m_Front * velocity;
        else if (direction == BACKWARD)
            m_Position -= m_Front * velocity;
        else if (direction == LEFT)
            m_Position -= m_Right * velocity;
        else if (direction == RIGHT)
            m_Position += m_Right * velocity;
        else
            throw std::invalid_argument("Invalid movement direction");
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR::CAMERA::INVALID_DIRECTION: " << e.what() << std::endl;
    }
}

// Process mouse movement input
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // Constrain pitch to avoid flipping the m_Camera
    if (constrainPitch)
    {
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
    }

    UpdateCameraVectors();
}

// Process mouse scroll input
void Camera::ProcessMouseScroll(float yoffset)
{
    m_Zoom -= (float)yoffset;
    if (m_Zoom < 1.0f)
        m_Zoom = 1.0f;
    if (m_Zoom > 45.0f)
        m_Zoom = 45.0f;
}

// Update m_Camera vectors based on Euler Angles
void Camera::UpdateCameraVectors()
{
    try
    {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);

        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  // Recalculate right vector
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));       // Recalculate up vector
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR::CAMERA::VECTOR_CALCULATION_FAILED: " << e.what() << std::endl;
    }
}
