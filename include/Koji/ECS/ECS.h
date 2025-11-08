#pragma once

// Entry point, include necessary files fo the ECS

namespace ECS
    {
    #include "World.h"

    #include "System.h"
    #include "Component.h"
    #include "Entity.h"
    #include "EntityManager.h"

    #include "Archetype.h"
    #include "Chunck.h"


    // Components
    #include "Components/MetaData.h"
    #include "Components/Transform.h"
    #include "Components/Sphere.h"
    
    // Systems
    #include "Systems/Time.h"

}
