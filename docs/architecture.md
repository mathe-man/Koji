## View 1 - Application, Layers, Window and Events
```mermaid
classDiagram
direction TB
class EntryPoint {
  +main(argc int, argv string) int
}
class Application {
  +Application(spec ApplicationSpec)
  +Run() int
  +PushLayer(layer Layer) void
  +PushOverlay(layer Layer) void
  +OnEvent(e Event) void
  +RequestClose() void
}
class IWindow {
  <<interface>>
  +PollEvents() void
  +SwapBuffers() void
  +SetEventCallback(cb EventCallback) void
  +NativeHandle() VoidPtr
}
class GlfwWindow
class Event {
  +handled bool
}
class EventDispatcher {
  +EventDispatcher(e Event)
  +Dispatch(fn Handler) bool
}
class LayerStack {
  +PushLayer(layer Layer) void
  +PushOverlay(layer Layer) void
  +PopLayer(layer Layer) void
}
class Layer {
  <<abstract>>
  +OnAttach() void
  +OnDetach() void
  +OnUpdate(dt float) void
  +OnEvent(e Event) void
}
class EditorLayer
class RuntimeLayer
EntryPoint ..> Application : CreateApplication() then Run()
Application *-- IWindow : own
Application *-- LayerStack : own
IWindow <|.. GlfwWindow
IWindow ..> Application : callback OnEvent(e)
LayerStack o-- Layer
Layer <|-- EditorLayer
Layer <|-- RuntimeLayer
Application ..> Layer : OnUpdate(dt) from bottom to the top
Layer ..> EventDispatcher : Dispatch(fn) in OnEvent
EventDispatcher ..> Event : read and set as handled
note for EntryPoint "Each app define CreateApplication() wich stack is layers"
note for Application "No game logic here : window and layers stack"
note for IWindow "Isolated platform : GLFW for PC, something else for android"
note for Layer "Layer = application level (editor, runtime). Not a system wich is a game logic in the World"
note for Event "OnEvent goes from top layer to the bottom. handled = true stop propagation"
```

## View 2 - Project, Scene and Assets
```mermaid
classDiagram
direction TB
class EditorLayer
class RuntimeLayer
class ProjectManager {
  +Open(path string) bool
  +Save() bool
  +Export(target string) bool
  +Config() ProjectConfig
  +Scenes() SceneManager
  +Assets() AssetManager
}
class ProjectConfig {
  +enabledSystems List~string~
  +startScene UUID
  +engineVersion string
}
class SceneManager {
  +Load(id UUID) bool
  +Active() Scene
  +BeginPlay() void
  +EndPlay() void
}
class Scene {
  +name string
  +CreateEntity(name string) EntityId
  +DestroyEntity(e EntityId) void
  +Clone() Scene
  +GetWorld() World
}
class World
class AssetManager {
  +Import(path string) UUID
  +Load(id UUID) Handle
  +Meta(id UUID) AssetMeta
}
class AssetMeta {
  +uuid UUID
  +relativePath string
}
ProjectManager *-- ProjectConfig : own
ProjectManager *-- SceneManager : own
ProjectManager *-- AssetManager : own
SceneManager o-- Scene : Load and copy
Scene *-- World : own, Clone() copy chunks
AssetManager o-- AssetMeta : Read .meta files
EditorLayer ..> ProjectManager : Open(path), Save(), Export(target)
EditorLayer ..> SceneManager : BeginPlay(), EndPlay()
RuntimeLayer ..> ProjectManager : Open(data/game.kojiproj)
RuntimeLayer ..> SceneManager : Load(config.startScene)
note for World "ECS View 3"
note for Scene "Clone() used to play in editor : The copy is thrown away after leaving the play"
note for ProjectConfig "List of systems in the project, config (e.g. Render2D, Physic)"
note for AssetMeta "UUID 64 bits + relative path"
```

## View 3 - KojiECS: Entities and Components storage
```mermaid
classDiagram
    direction TB
    class ComponentRegistry {
        +Register(name string, size uint, align uint) ComponentId
        +RegisterType(name string) ComponentId
        +IdOf() ComponentId
        +Find(name string) ComponentId
        +Info(id ComponentId) ComponentInfo
        +LastError() string
    }
    class ComponentInfo {
        +id ComponentId
        +name string
        +size uint
        +align uint
        +IsTag() bool
    }
    class EntityManager {
        <<codé>>
        +Create() EntityId
        +Destroy(e EntityId) bool
        +IsAlive(e EntityId) bool
        +Locate(e EntityId) optional~EntityRecord~
        +SetLocation(e EntityId, rec EntityRecord) bool
        +AliveCount() size
    }
    class EntityRecord {
        +archetype ArchetypeId
        +chunk uint
        +row uint
    }
    class ChunkLayout {
        <<codé>>
        +Create(chunkBytes size, comps ComponentInfoList) optional~ChunkLayout~
        +Capacity() uint
        +Signature() ComponentMask
        +Has(id ComponentId) bool
        +FindColumn(id ComponentId) ChunkColumn
    }
    class ChunkColumn {
        +id ComponentId
        +size uint
        +align uint
        +offset size
    }
    class Chunk {
        <<codé>>
        +Chunk(layout ChunkLayout)
        +Push(e EntityId) uint
        +SwapRemove(row uint) EntityId
        +Entities() span~EntityId~
        +Components(id ComponentId) span~T~
        +Column(id ComponentId) VoidPtr
        +ComponentAt(id ComponentId, row uint) VoidPtr
        +Clone() Chunk
        +Count() uint
    }
    ComponentRegistry o-- ComponentInfo : one per component
    EntityManager o-- EntityRecord : one per entity
    ChunkLayout o-- ChunkColumn : one per component
    ChunkLayout ..> ComponentInfo : Create(chunkBytes, comps) read size and align
    Chunk ..> ChunkLayout : read offsets and capacity
    note for EntityManager "EntityId = index + generation : old destroyed id is not longer valid"
    note for EntityRecord "Where the entity live : archetype, chunk, row. Updated by the World when moved"
    note for ChunkLayout "Calculated one time per archetype."
    note for Chunk "SwapRemove return the moved entity : Caller have to update his EntityRecord."
```

## View 4 - KojiECS: World, archetypes and requests
```mermaid
classDiagram
direction TB
class World {
  +CreateEntity() EntityId
  +DestroyEntity(e EntityId) bool
  +IsAlive(e EntityId) bool
  +Add(e EntityId, value T) bool
  +Remove(e EntityId) bool
  +Has(e EntityId) bool
  +Get(e EntityId) TPtr
  +AddRaw(e EntityId, id ComponentId, data VoidPtr) bool
  +ForEachChunk(q Query, fn ChunkFn) void
  +Registry() ComponentRegistry
  +Clone() World
}
class Archetype {
  +Remove(rec EntityRecord) EntityId
  +MoveRow(from EntityRecord, to Archetype) MoveResult
  +Signature() ComponentMask
  +GetChunk(index uint) Chunk
  +ChunkCount() uint
  +EntityCount() uint
}
class MoveResult {
  +newRecord EntityRecord
  +movedInSource EntityId
}
class Query {
  +Require(id ComponentId) Query
  +Exclude(id ComponentId) Query
  +Matches(sig ComponentMask) bool
}
class QueryCache {
  +Match(q Query, archetypes ArchetypeList) ArchetypeIdList
}
class ChunkView {
  +Count() uint
  +Entities() span~EntityId~
  +Components(id ComponentId) span~T~
  +Column(id ComponentId) VoidPtr
}
class EntityManager
class ComponentRegistry
class ChunkLayout
class Chunk
World *-- EntityManager : own
World *-- ComponentRegistry : own
World *-- Archetype : one per components combination
World *-- QueryCache : own
Archetype *-- ChunkLayout : own the layout
Archetype *-- Chunk : own the list
World ..> EntityManager : Create(), Locate(e), SetLocation(e, rec)
World ..> ComponentRegistry : RegisterType(), Info(id)
World ..> Archetype : Allocate(e), Remove(rec), MoveRow(from, to)
World ..> QueryCache : Match(q, archetypes)
World ..> ChunkView : ForEachChunk(q, fn) give a ChunkView for each non-empty chunk
Archetype ..> Chunk : Push(e), SwapRemove(row)
Archetype ..> MoveResult : return
QueryCache ..> Query : Matches(sig)
QueryCache ..> Archetype : Signature()
note for World "Only way to access the ECS."
note for MoveResult "movedInSource = entity which filled the hole : son EntityRecord doit être mis à jour"
note for Query "Created by the scheduler from reads and writes declared in the SystemDesc"
note for ChunkView "Systems don't get a Chunk : no Push() or SwapRemove() during the update cycle. Those type of action during the cycle will use a CommandBuffer later"
```

## View 5 - KojiECS: Systems
```mermaid
classDiagram
direction TB
class RuntimeLayer {
  +OnUpdate(dt float) void
}
class SystemScheduler {
  +Build(reg SystemRegistry, cfg ProjectConfig) bool
  +Init(ctx SystemContext) bool
  +Run(ctx SystemContext) void
  +Close(ctx SystemContext) void
}
class SystemRegistry {
  +AddSystem(args Args) T
  +GetSystem() T
  +RemoveSystem() bool
  +All() List~System~
}
class System {
  <<abstract>>
  +Describe() SystemDesc
  +Init(ctx SystemContext) bool
  +Update(ctx SystemContext) void
  +OnChunk(ctx SystemContext, chunk ChunkView) void
  +Close(ctx SystemContext) bool
}
class SystemDesc {
  +requiredSystems List~TypeId~
  +reads List~ComponentId~
  +writes List~ComponentId~
}
class SystemContext {
  +world World
  +dt float
  +GetSystem() T
}
class ChunkView {
  +count int
  +Column(id ComponentId) VoidPtr
}
class TimeSystem {
  +DeltaTime() float
  +FixedDelta() float
}
class PhysicsSystem
class ManagedSystem {
  +OnChunk(ctx SystemContext, chunk ChunkView) void
}
class CApi {
  +koji_register_system(desc SystemDescC, fn SystemFn) SystemId
  +koji_register_component(name string, size int, align int) ComponentId
}
class GameCode_CSharp {
  +MoveSystem_Update(ctx Ptr, chunk ChunkPtr) void
}
class World
RuntimeLayer ..> SystemScheduler : Build(reg, cfg) then Run(ctx) on each frame
SystemScheduler ..> SystemRegistry : All() then topological sort
SystemRegistry o-- System : own
System ..> SystemDesc : Describe()
SystemScheduler ..> World : Query(sig) with reads and writes needed by the system 
SystemScheduler ..> System : Update(ctx) then OnChunk(ctx, chunk)
System ..> SystemContext : receive in parameter
SystemContext ..> World : exposes world
System <|-- TimeSystem
System <|-- PhysicsSystem
System <|-- ManagedSystem
PhysicsSystem ..> TimeSystem : ctx.GetSystem() then FixedDelta()
CApi ..> SystemRegistry : AddSystem(ManagedSystem)
CApi ..> ManagedSystem : create
ManagedSystem ..> GameCode_CSharp : callback fn(ctx, chunk) one time per chunk
GameCode_CSharp ..> CApi : koji_register_system(desc, fn) at startup
note for SystemScheduler "Topological sort give a clear error for missing systems or unresolvable dependencies. Close in reverse order. Later : parallel execution by solving read and writes"
note for ChunkView "View on a chunk (View 3)"
note for ManagedSystem "One C# call per chunk, never per entity"
```