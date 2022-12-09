#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include "sceneparser.h"

class Camera
{
public:
    Camera() {};
    Camera(int width, int height, float near, float far, SceneCameraData& cameraData);

    float getNear() const {return this->near;};
    float getFar() const {return this->far;};

    glm::mat4 getViewMatrix() const {return this->viewMatrix;}
    glm::mat4 getWorldMatrix() const {return this->worldMatrix;}
    glm::mat4 getPerspectiveMatrix() const {return this->perspective;}
    glm::mat4 getScaleMatrix() const {return this->scale;}
    glm::mat4 getUnhingingMatrix() const {return this->unhinging;}

    glm::vec4 getPosition() const {return this->pos;}
    glm::vec4 getLook() const {return this->look;}
    glm::vec4 getUp() const {return this->up;}

    glm::vec4 getWorldPosition() const {return this->worldMatrix * glm::vec4(0, 0, 0, 1);}

    float getHeightAngle() const {return this->heightAngle;}
    float getAspectRatio() const {return this->aspectRatio;}

    void updateFrustum(float width, float height, float near, float far);
    void move(float alongLook, float alongClup, float alongVert, float timeStep);
    void rotate(float deltaX, float deltaY);
    void reset();
private:
    bool initialized = false;
    float near;
    float far;

    glm::vec4 pos;
    glm::vec4 look;
    glm::vec4 up;

    glm::vec4 initialPos;
    glm::vec4 initialLook;
    glm::vec4 initialUp;

    glm::vec4 u;
    glm::vec4 v;
    glm::vec4 w;

    glm::vec3 clup;

    float heightAngle;
    float widthAngle;

    glm::mat4 viewMatrix;
    glm::mat4 worldMatrix;

    float viewDepth;
    float aspectRatio;

    glm::mat4 perspective;
    glm::mat4 scale;
    glm::mat4 unhinging;

    void computeView();

    constexpr const static glm::mat4 remap = glm::mat4(
        glm::vec4(1, 0, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 0, -2, 0),
        glm::vec4(0, 0, -1, 1)
    );
};

#endif // CAMERA_H