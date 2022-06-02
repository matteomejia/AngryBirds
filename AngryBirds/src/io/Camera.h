#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// direcciones
enum class CameraDirection
{
    NONE = 0,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera
{
public:
    glm::vec3 cameraPos;

    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;

    glm::vec3 worldUp; // siempre es vec3(0.0, 1.0, 0.0)

    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    float zoom;

    // constructor
    Camera(glm::vec3 position = glm::vec3(0.0f));

    // actualizar datos
    void updateCameraDirection(double dx, double dy);
    void updateCameraPos(CameraDirection dir, double dt);
    void updateCameraZoom(double dy);

    // accesores
    glm::mat4 getViewMatrix();
    float getZoom();

private:
    //actualizar vectores
    void updateCameraVectors();
};

#endif