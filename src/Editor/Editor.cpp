#include <Koji/Editor/Editor.hpp>
#include <Koji/Engine/Scene.h>

#include "Koji/Editor/Systems.hpp"

using namespace Koji::Editor;
using namespace Koji::Engine;

Scene* Editor::scene {};

bool Editor::Run(Scene* s)
{
    // Set the scene static member
    scene = s;
    scene->systems.insert(scene->systems.begin(), new Systems::EditorRenderingSystem);



    // === Init all systems ===
    for (System* sys : scene->systems)
        if (!sys->Init(*scene, scene->registry))
            return false;
    
    // Loop
    while (true) {

        // === Call BeginFrame() on all systems ===
        for (System* sys : scene->systems)
            if (!sys->BeginFrame(scene->registry))
                return false;
        
        // === Call Frame() on all systems ===
        for (System* sys : scene->systems)
            if (!sys->Frame(scene->registry))
                return false;

        // === Call BeginFrame() on all systems ===
        for (System* sys : scene->systems)
            if (!sys->EndFrame(scene->registry))
                return false;
    }

    // Shutdown
    for (System*  sys : scene->systems)
        if (!sys->Close())
            return false;

    
    return true;
}