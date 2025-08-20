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

    // Optional: Initialize view matrix immediately
    view = XMMatrixLookAtLH(
        XMLoadFloat3(&position),
        XMLoadFloat3(&target),
        XMVectorSet(0, 1, 0, 0)
    );
    LOG_INFO(L"View matrix created from position and target");
}
