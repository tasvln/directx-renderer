#pragma once

#include "utils/pch.h"

class Shader{
    public:
        Shader(
            const std::wstring& filename, 
            const std::string& entryPoint, 
            const std::string& target
        );

        ~Shader() = default;

        ComPtr<ID3DBlob> getBytecode() const { 
            return bytecode; 
        }

        ComPtr<ID3DBlob> getErrorBlob() const { 
            return errorBlob; 
        }

    private:
        ComPtr<ID3DBlob> bytecode;
        ComPtr<ID3DBlob> errorBlob;
};