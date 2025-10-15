# Koji
A 3d game engine using an ECS


# Tech Stack (Overview)
> Those aren't definitive choices

 - **Engine Structure**
   - [EnTT](https://github.com/skypjack/entt): Entities, Components, Systems (ECS)
     
 - **Rendering / Graphics**
   - [bgfx](https://github.com/bkaradzic/bgfx): GPU abstraction (Vulkan, DX12, OpenGL, Metal)
   - [GLFW](https://github.com/glfw/glfw) ([website](https://www.glfw.org)): window creation & input handling
   - Shaders: GLSL / HLSL

 - **Physics**
   - [ReactPhysics3D](https://github.com/DanielChappuis/reactphysics3d) ([website](https://www.reactphysics3d.com/download.html)): Rigid bodies, colliders, etc.
   - [glm](https://github.com/g-truc/glm): vectors, matrices, and other maths

 - **UI**
   - [ImGui](https://github.com/ocornut/imgui): For debug and tools (not games GUI). Implementation can be foun [here](https://github.com/bkaradzic/bgfx/tree/master/examples/common/imgui)
  
 - **Later**
   - *Scripting*: Lua (using sol2)
   - *Audio*: OpenAL or other
