# Koji
A 3d game engine using an ECS


# Tech Stack (Overview)
> Those aren't definitive choices

 - **Rendering / Graphics**
   - [glfw](https://github.com/glfw/glfw): Window creator
   - [bgfx](https://github.com/bkaradzic/bgfx) ([website](https://www.glfw.org)): Graphics abstraction

 - **Physics**
   - [ReactPhysics3D](https://github.com/DanielChappuis/reactphysics3d) ([website](https://www.reactphysics3d.com/download.html)): Rigid bodies, colliders, etc.
g

 - **Assets**
   - [Assimp](https://github.com/assimp/assimp): To load 3d models, textures...

- **Later**
   - *Scripting*: Lua (using sol2)
   - *Audio*: OpenAL or other
   - *Editor*: And project/scene editor using Qt
