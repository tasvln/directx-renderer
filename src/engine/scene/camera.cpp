#include "camera.h"

Camera::Camera(
    float fov, 
    float aspect, 
    float nearZ, 
    float farZ
) : 
    position{ 0.0f, 0.0f, -5.0f }, 
    target{ 0.0f, 0.0f, 0.0f } 
{
    LOG_INFO(L"Initializing Camera...");
    LOG_INFO(L"Camera position set to (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
    LOG_INFO(L"Camera target set to (%.2f, %.2f, %.2f)", target.x, target.y, target.z);

    projection = XMMatrixPerspectiveFovLH(
        fov, 
        aspect, 
        nearZ, 
        farZ
    );
    LOG_INFO(L"Projection matrix created. FOV: %.2f, Aspect: %.2f, NearZ: %.2f, FarZ: %.2f", fov, aspect, nearZ, farZ);

    updateViewMatrix();
}

void Camera::setProjection(
    float fov, 
    float aspect, 
    float nearZ, 
    float farZ
) {
    projection = XMMatrixPerspectiveFovLH(
        fov,
        aspect,
        nearZ,
        farZ
    );

    LOG_INFO(L"Projection matrix updated. FOV: %.2f, Aspect: %.2f, NearZ: %.2f, FarZ: %.2f", fov, aspect, nearZ, farZ);
}

void Camera::updateViewMatrix() {
    XMVECTOR pos = XMLoadFloat3(&position);
    XMVECTOR tgt = XMLoadFloat3(&target);
    XMVECTOR up  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    view = XMMatrixLookAtLH(pos, tgt, up);
}

void Camera::update(float delta) {
    updateViewMatrix();
}
