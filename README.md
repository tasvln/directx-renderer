# DirectX 12 3D Graphics Renderer

## Overview
This project demonstrates a complete **DirectX 12 graphics pipeline** through a simple 3D cube renderer. The implementation covers GPU initialization, resource setup, shader compilation, command recording, synchronization, and presentation. The demo includes an orbit camera, vertex-colored cube, a custom logging system, and an FPS counter shown in the window title.

<img width="1435" height="728" alt="Screenshot 2025-10-08 135345" src="https://github.com/user-attachments/assets/37297420-225e-4623-90dd-b10ce7d134a5" />

---

## Features
- Full DirectX 12 graphics pipeline setup (device, command queue, swap chain, descriptor heaps)
- Vertex and pixel shaders written in HLSL
- Vertex color interpolation for smooth gradients on the cube
- Orbit camera with basic mouse/keyboard controls
- Custom logger for categorized runtime messages (info, warn, error)
- Real-time FPS counter displayed in the window title
- Proper resource state transitions and GPU synchronization
- Simple, single-file demo structure suitable for learning and extension

---

## Concepts Demonstrated
- Direct3D 12 device and command queue creation
- Swap chain and render target view setup
- Descriptor heap management (RTV/CBV/SRV as applicable)
- Creating and binding vertex and index buffers
- Constant buffer usage and updating (view/projection/model matrices)
- Compiling and binding HLSL shaders (vertex/pixel)
- Command list recording and executing
- Resource barriers and fence-based GPU synchronization
- Basic camera math (view and projection matrices)
- Basic performance tracking (frame timing and FPS)

---

## Requirements
- Language: C++17 or later
- Graphics API: DirectX 12
- IDE: Visual Studio Code (Visual Studio 2019/2022 works too)
- SDKs/Libraries:
  - (Check Cmake File For all libraries)

---

## How to Build & Run
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/directx-3d.git
   cd directx-3d
   ```

2. Build shaders
    ```bash
   cd assets/shaders
   bash compile.sh
    ```
3. Build Demo (Back to root folder)
    ```bash
   bash build.sh
    ```
5. The demo window will appear showing a colored cube. The current FPS value is displayed in the window title.

Notes:
- If shader compilation fails, ensure `dxc` (or `fxc`) is installed and the project shader paths are correct.
- Run the application with administrator privileges only if required by your environment (normally not needed).

---

## Shaders
Place shader HLSL files under `assests/shaders/`. Typical files:
- `vertex.hlsl` — transforms vertices and passes interpolated color to pixel shader.
- `pixel.hlsl` — receives interpolated color and outputs final pixel color.

---

## Logging and FPS
- The project includes a small custom logger to emit categorized messages (INFO/WARN/ERROR). The logger can be configured to log to console, file, or both.
- FPS is measured in the render loop and updated in the application window title so you can monitor performance during runtime.

---

## References
- https://github.com/FaberSanZ/DX12-Samples
- https://github.com/tasvln/Xbox-ATG-Samples
   
