#pragma once

#include "utils/pch.h"

class Camera {
public:
    Camera(
        float fov, 
        float aspect, 
        float nearZ, 
        float farZ
    );
    
    ~Camera() = default;

    void setPosition(const XMFLOAT3& pos) { 
        position = pos; 
        updateViewMatrix();
    }

    void setTarget(const XMFLOAT3& tgt) { 
        target = tgt; 
        updateViewMatrix();
    }

    XMMATRIX getViewMatrix() const { 
        return view; 
    }

    XMMATRIX getProjectionMatrix() const { 
        return projection; 
    }

    XMMATRIX getViewProjectionMatrix() const {
        return XMMatrixMultiply(view, projection);
    }

    void update(float delta);

private:
    void updateViewMatrix();

private:
    XMFLOAT3 position;
    XMFLOAT3 target;
    XMMATRIX projection;
    XMMATRIX view;
};
