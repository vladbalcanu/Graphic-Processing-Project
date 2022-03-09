#include "Camera.hpp"

namespace gps {

    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp, glm::vec3 cameraFrontDirection, glm::vec3 cameraRightDirection) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = glm::normalize(cameraUp);
        this->cameraFrontDirection = glm::normalize(cameraTarget-cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection,this->cameraUpDirection));
    }

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition+cameraFrontDirection, cameraUpDirection);
    }
    void Camera::StartAnimation(glm::vec3 startPosition) {
        this->cameraPosition = startPosition;
    }

    void Camera::move(MOVE_DIRECTION direction, float speed) {
        if (direction == MOVE_FORWARD) {
            this->cameraPosition = this->cameraPosition+speed*this->cameraFrontDirection;
        }

        if (direction == MOVE_BACKWARD) {
            this->cameraPosition = this->cameraPosition-speed*this->cameraFrontDirection;
        }

        if (direction == MOVE_LEFT) {
          this->cameraPosition= this->cameraPosition- speed* this->cameraRightDirection;
        }

        if (direction == MOVE_RIGHT) {
            this->cameraPosition = this->cameraPosition + speed * this->cameraRightDirection;
        }

        if (direction == MOVE_UP) {
            this->cameraPosition = this->cameraPosition + speed * this->cameraUpDirection;
        }

        if (direction == MOVE_DOWN) {
            this->cameraPosition = this->cameraPosition - speed * this->cameraUpDirection;
        }
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
    }
    void Camera::rotate(float pitch, float yaw) {
        glm::vec3 vector;
        vector.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        vector.y = sin(glm::radians(pitch));
        vector.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->cameraFrontDirection = glm::normalize(vector);
    }
}