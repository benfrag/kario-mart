# Kario Mart

using benfrag/oasis-engine

## Academic Disclaimer
Generative AI has in part been used for deliberation and consensus on architectural choices, as well as for some implementation - however, no code has been entirely copied into the project directly from Generative AI, all code was vetted and rehashed for the actual implementation - I liken this to querying stack-overflow with more direct contextually aware answers.

Generative AI was comparably heavily used for /lib/engine/primitives/triangle.cpp particularly for the AVX2 implementation, but less so for the edge detection which took considerable research and testing before implementing.

Further, Generative AI was used directly for some of the core ECS implementation found in:
include/engine/core/ecs/component.h
include/engine/core/ecs/ecs.h
include/engine/core/ecs/entity.h
include/engine/core/ecs/system.h


The entire codebase is around ~3500-4000 lines of code without taking into account comments,
Thus it is unreasonable to expect the marker to rigorously go through the source, I have provided some brief documentation and broader architecture, but despite this the source remains quite unexplained/uncommented and in some of the later parts quite mad-scientistish (due to laziness).

Further, although the optional assignment is quite a small task, I had already worked on a good portion of the engine in January this year, so it was convenient for me to add some additional code for the sole purpose of the assignment as I do not have many public programs that would be worth scrutiny.

## Architecture

This engine is extremely unconventional, as a programming task it is purely out of educational rather than practical purposes.

No external libraries (besides windows api and standard libraries) are used! Thus, the engine is confined to use purely software rendering rather than the compute of a GPU. Due to this modern impracticality, a resolution of 320x240 (stretched to 1920x1080) is used to gain some decent frames per second.

The software rendering is achieved by creating both a back buffer and front buffer, clearing them at the start of each render cycle, rendering to the back buffer, and at the end of the cycle the buffers are swapped (/lib/engine/renderer/renderer.cpp called by EngineCore /lib/engine/core/engine.cpp), the window's client area is invalidated which sends a WM_PAINT windows message to the main window handle which allows the frontbuffer to copy its contents into the window.

The engine uses a an Entity Component System (ECS) architecture which allows for the corresponding game to implement entities (discrete objects containing a multitude of components), and components to represent data attached to those entities (implemented as structs), as well as systems to act on the data contained in components for specific entities. This was chosen over an inheritance based architecture for its ability to keep larger projects somewhat simplistic in nature. The ECS allows for systems to act on entities containing a certain signature of components (the signature is constructed as an std::bitset, with each component using their unique incrementing id as a position in the bitset to represent what components the signature encompasses) and thus systems ideally contain reusable code which can act on certain data attributed to entities - for example, there is a render system which acts on GeometryComponents, many entities hold geometry such as the player/car, the track, and item boxes, this allows other systems to work on other parts of the data of an entity, for example include/game/systems/local_player_input.h works on player input (and against the traditional usage of ECS architecture, for simplicity, also works on the physics of the car and camera positioning).

In terms of rendering the world to the screen/back buffer, geometry is constructed purely of vertices (without indices) and a corresponding colour for that geometry (for simplicity's sake, as well as the limitations of the CPU, texturing, lighting and shadows are not implemented). The ECS basically has an engine system for rendering geometry, which it does so by inefficiently applying position transformations and rotations to the original vertices, adding these new 'render vertices' to a queue which gets handled in the EngineCore's run cycle after the ECS has handled all systems for that frame. This queue originally had the intention of being multi-threaded but remains single-threaded, the implementation can be found in /lib/engine/core/render_manager.cpp. From here the vertices are handled in batches of three, and a translation from 3D world to 2D screen space using the combined view projection matrix of the main camera (which is created as a component of a game entity and specified to the engine's instance of the renderer) is performed. From here, the 'primitive renderer' allows for triangle rasterization by forming edge equations from the transformed screen coordinates, a simple form of z buffering is implemented which is unsuccessful at times. SIMD AVX2 intrinsics are used as a modern work around for the CPU burden.


## Brief Documentation

This project uses a somewhat unconventional folder structure with src containing only main.cpp with the entry point, and /lib and /include for holding (mostly) implementation (.cpp) and declarations (.h) respectively. /lib for implementation was originally used to retain modular structure for the engine, but ends up holding both implementation for the engine and the game.

![High res 1920x1080 rasterizing.](https://github.com/benfrag/kario-mart/blob/main/high_res.png?raw=true)

320x240 used in source for fps.
