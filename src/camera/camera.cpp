#include <stdexcept>
#include "camera.h"
#include "glm/gtx/transform.hpp"



Camera::Camera(SceneCameraData const &c_data, int c_width, int c_height, double c_near, double c_far) :
    pos(c_data.pos), look(glm::normalize(c_data.look)), up(glm::normalize(c_data.up)),
    heightAngle(c_data.heightAngle), width(c_width), height(c_height),
    nearPlane(c_near), farPlane(c_far),
    aspectRatio(static_cast<double>(width) / height)
{
    // compute theta and phi from look vector
    theta = glm::half_pi<double>() - std::acos(look.y);
    phi = std::atan2(look.z, look.x);

    // udpate matrices
    updateViewMatrix();
    updateProjMatrix();
    updateProjView();
}

glm::vec4 const & Camera::getPos() const { return pos; };
glm::vec4 const & Camera::getLook() const { return look; };
glm::vec4 const & Camera::getUp() const { return up; };
glm::vec4 const & Camera::getRight() const { return viewMatrixInverse[0]; };
glm::mat4 const & Camera::getViewMatrixInverse() const { return viewMatrixInverse; }
glm::mat4 const & Camera::getViewMatrix() const { return viewMatrix; }
glm::mat4 const & Camera::getProjMatrix() const { return projMatrix; }
glm::mat4 const & Camera::getProjView() const { return projView; }
double const & Camera::getAspectRatio() const { return aspectRatio; }
double const & Camera::getTheta() const { return theta; }
double const & Camera::getPhi() const { return phi; }
double const & Camera::getHeightAngle() const { return heightAngle; }
double const & Camera::getNearPlane() const { return nearPlane; }
double const & Camera::getFarPlane() const { return farPlane; }

double Camera::yMax() const { return std::tan(0.5f * heightAngle); }
double Camera::xMax() const { return yMax() * aspectRatio; }

void Camera::setPos(const glm::vec4 &newPos) { pos = newPos; }
void Camera::setNearFarPlanes(double zNear, double zFar) {
    newProj = (zNear != nearPlane || zFar != farPlane);
    nearPlane = zNear;
    farPlane = zFar;
}
void Camera::setWidthHeight(int width, int height) {
    newProj = (width != this->width || height != this->height);
    this->width = width;
    this->height = height;
    this->aspectRatio = static_cast<double>(width) / height;
}
void Camera::setThetaPhi(double newTheta, double newPhi) {
    theta = std::clamp(newTheta, -glm::half_pi<double>(), glm::half_pi<double>());
    phi = newPhi;
}

void Camera::updateViewMatrix() {
    auto up3  = glm::vec3(up);
    auto pos3 = glm::vec3(pos);

//    std::cout << "pos: " << glm::to_string(pos3) << "\n\n";
//    viewMatrix = glm::lookAt(pos3, pos3 + glm::vec3(look), up3);
//    viewMatrixInverse = glm::inverse(viewMatrix);

    auto w    = glm::vec3(-look);  // assume already normalized
    auto v    = glm::normalize(up3 - glm::dot(up3, w) * w);
    auto u    = glm::cross(v, w);
    auto R    = glm::mat4(glm::mat3(u, v, w));
    auto T    = glm::translate(-pos3);
    viewMatrix = glm::transpose(R) * T;
    viewMatrixInverse = R;
    viewMatrixInverse[0][3] = pos.x;
    viewMatrixInverse[1][3] = pos.y;
    viewMatrixInverse[2][3] = pos.z;
}
void Camera::updateLook() {
    // use theta (pitch) and phi (yaw) to update look and up
    look.x = std::cos(phi) * std::cos(theta);
    look.y = std::sin(theta);
    look.z = std::sin(phi) * std::cos(theta);
    look = glm::normalize(look);
}
void Camera::updateProjView() {
    projView = projMatrix * viewMatrix;
}
void Camera::updateProjMatrix() {
    projMatrix = glm::perspective(heightAngle, aspectRatio, nearPlane, farPlane);
    newProj = false;
}

