#pragma once

#include <glm/glm.hpp>

// DEBUG
#include <iostream>
#include <glm/gtx/string_cast.hpp>


struct SceneCameraData {
    glm::vec4 pos  = {0.0, 0.0, 2.0, 1.0};
    glm::vec4 look = {0.0, 0.0, -1.0, 0.0};
    glm::vec4 up   = {0.0, 1.0, 0.0, 0.0};

    double heightAngle = glm::quarter_pi<double>();
};


// A class representing a virtual camera.
class Camera {

public:
    Camera() = default;
    Camera(SceneCameraData const &c_data, int c_width, int c_height, double c_near, double c_far);

    glm::vec4 const & getPos() const;
    glm::vec4 const & getLook() const;
    glm::vec4 const & getUp() const;
    glm::vec4 const & getRight() const;
    glm::mat4 const & getViewMatrix() const;
    glm::mat4 const & getViewMatrixInverse() const;
    glm::mat4 const & getProjMatrix() const;
    glm::mat4 const & getProjView() const;

    // Returns the rotation part of the view matrix
    glm::mat4 const getRotationMatrix() const;

    // Returns orientation angles of the camera.
    double const & getTheta() const;
    double const & getPhi() const;

    // Returns the aspect ratio of the camera.
    double const & getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    double const & getHeightAngle() const;

    // Returns the depth of near/far plane of the camera
    double const & getNearPlane() const;
    double const & getFarPlane() const;


    double yMax() const;
    double xMax() const;


    void setNearFarPlanes(double zNear, double zFar);
    void setWidthHeight(int width, int height);
    void setPos(const glm::vec4 &newPos);
    void setThetaPhi(double newTheta, double newPhi);

    void updateLook();
    void updateViewMatrix();
    void updateProjMatrix();
    void updateProjView();

    bool hasDOF() const;
    const bool &projChanged() const {return newProj;}

    // DEBUG
    void printInfo() const {
        std::cout << "[Camera]\n";
        std::cout << "pos: " << glm::to_string(pos) << "\n";
        std::cout << "dir: " << glm::to_string(look) << "\n";
        std::cout << "(theta, phi) = (" << theta << ", " << phi << ")\n";
        std::cout << "view: " << glm::to_string(viewMatrix) << "\n";
//        std::cout << "near: " << nearPlane << "\n";
//        std::cout << "far: " << farPlane << "\n";
//        std::cout << "proj: " << glm::to_string(projMatrix) << "\n";
//        std::cout << "projView: " << glm::to_string(projView) << "\n";
    }

private:
    glm::mat4 viewMatrix, viewMatrixInverse;
    glm::mat4 projMatrix;
    glm::mat4 projView;  // projMatrix * viewMatrix
    glm::vec4 pos, look, up;
    int width, height;   // unit: px
    double theta, phi;   // orientation angles in radians
    double aspectRatio;
    double heightAngle;
    double nearPlane, farPlane;
    glm::mat4 computeProjMatrix() const;

    bool newProj = false;
};
