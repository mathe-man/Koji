# Koji
A 3d game engine using an ECS


# Tech Stack (Overview)
> Those aren't definitive choices

 - **Engine Structure**
   - [EnTT](https://github.com/skypjack/entt): Entities, Components, Systems (ECS)
     
 - **Rendering / Graphics**
   - [Raylib](https://github.com/raysan5/raylib): Graphics abstraction

 - **Physics**
   - [ReactPhysics3D](https://github.com/DanielChappuis/reactphysics3d) ([website](https://www.reactphysics3d.com/download.html)): Rigid bodies, colliders, etc.
g
 - **UI**
   - [ImGui](https://github.com/ocornut/imgui): For debug and tools (not games GUI). Implementation can be foun [here](https://github.com/bkaradzic/bgfx/tree/master/examples/common/imgui)
   - [rlImGui](https://github.com/raylib-extras/rlImGui): The lib to use ImGui with Raylib  

 - **Later**
   - *Scripting*: Lua (using sol2)
   - *Audio*: OpenAL or other
