#ifndef CAMERA_H
#define CAMERA_H

#pragma once

#include <glad.h>
#include <iostream>  // For error messages

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Defines several possible options for m_Camera movement.
enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default m_Camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// An abstract m_Camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // m_Camera Attributes
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    // Euler Angles
    float m_Yaw;
    float m_Pitch;

    // m_Camera options
    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_Zoom;

    // Constructors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Destructor
    virtual ~Camera() {}

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() { return glm::lookAt(m_Position, m_Position + m_Front, m_Up); }

    // Processes input received from any keyboard-like input system.
    void ProcessKeyboard(CameraMovement direction, float deltaTime);

    // Processes input received from a mouse input system.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // Processes input received from a mouse scroll-wheel event.
    void ProcessMouseScroll(float yoffset);

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();
};
#endif