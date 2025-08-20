#include "shader.h"

Shader::Shader(
    const std::wstring& filename, 
    const std::string& entryPoint, 
    const std::string& target
) {
    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG)
    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = D3DCompileFromFile(
        filename.c_str(),
        nullptr, nullptr,
        entryPoint.c_str(),
        target.c_str(),
        compileFlags,
        0,
        &bytecode,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            LOG_ERROR(L"Shader compilation error: %S", (char*)errorBlob->GetBufferPointer());
        } else {
            LOG_ERROR(L"Shader compilation failed without error blob.");
        }
        throwFailed(hr);
    }

    LOG_INFO(L"Shader compiled successfully: %s", filename.c_str());
}

