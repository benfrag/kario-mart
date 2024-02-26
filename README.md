# Kario Mart

Submitted for extension assignment for 41039.

video showcase:  
youtube link: [http://www.youtube.com/watch?v=GtnzE2LabZM](http://www.youtube.com/watch?v=GtnzE2LabZM)

## What is this project?
This project was undertaken purely as an educational pursuit for my own further understanding of larger style project architecture and some of the underlying complexities of 3D rendering - I have undertaken similar projects before using Direct3D9, this project is new territory as there is no usage of external rendering libraries.

It is a game engine and a corresponding 'game', the game engine provides the ECS architecture to which the game interfaces with to program its logic, while the engine then takes care of orchestrating the running of the game and its integration with the software renderer. The 'game' is simply a straight track with the player in control of a car (controlled by WASD) with a third person camera, there are the beginnings of game elements with some rudimentary car physics and item boxes (uncollectable) similar to that in a popular Kart racing game.

The game and game engine do not come without flaws, I have not yet implemented proper clipping and a more robust way of doing z buffering and so there is some culling of visible vertices as well as some visual artifacts.

## Academic Disclaimer
Generative AI has in part been used for deliberation and consensus on architectural choices, as well as for some implementation - however, no code has been entirely copied into the project directly from Generative AI, all code was vetted and rehashed for the actual implementation - I liken this to querying stack-overflow but with more direct and contextually aware answers.

Generative AI was comparably heavily used for /lib/engine/primitives/triangle.cpp particularly for the AVX2 implementation, but less so for the edge detection which took further research and testing before implementing.

Further, Generative AI was used directly for some of the core ECS implementation found in:
include/engine/core/ecs/component.h
include/engine/core/ecs/ecs.h
include/engine/core/ecs/entity.h
include/engine/core/ecs/system.h


The entire codebase is around ~3500-4000 lines of code without taking into account comments,
Thus it is unreasonable to expect the marker to rigorously go through the source, I have provided some brief documentation and information on broader architecture, but despite this the source remains quite unexplained/uncommented and in some of the later parts quite mad-scientistish (due to laziness).

Further, although the optional assignment is quite a small task, I had already worked on a good portion of the engine in January this year, so it was convenient for me to add some additional code for the sole purpose of the assignment as I do not have many public programs that would be worth scrutiny.

## Architecture

This engine is extremely unconventional, as a programming task it is purely out of educational rather than practical purposes.

No external libraries (besides windows api and standard libraries) are used! Thus, the engine is confined to use purely software rendering rather than the compute of a GPU. Due to this modern impracticality, a resolution of 320x240 (stretched to 1920x1080) is used to gain some decent frames per second.

The software rendering is achieved by creating both a back buffer and front buffer, clearing them at the start of each render cycle, rendering to the back buffer, and at the end of the cycle the buffers are swapped (/lib/engine/renderer/renderer.cpp called by EngineCore /lib/engine/core/engine.cpp), the window's client area is invalidated which sends a WM_PAINT windows message to the main window handle which allows the frontbuffer to copy its contents into the window.

The engine uses an Entity Component System (ECS) architecture which allows for the corresponding game to implement entities (discrete objects containing a multitude of components), and components to represent data attached to those entities (implemented as structs), as well as systems to act on the data contained in components for specific entities. This was chosen over an inheritance based architecture for its ability to keep larger projects somewhat simplistic in nature. The ECS architecture allows for systems to act on entities containing a certain signature of components (the signature is constructed as an std::bitset, with each component using their unique incrementing id as a position in the bitset to represent what components the signature encompasses) and thus systems ideally contain reusable code which can act on certain data attributed to entities - for example, there is a render system which acts on GeometryComponents, many entities hold geometry such as the player/car, the track, and item boxes, this allows other systems to work on other parts of the data of an entity, for example include/game/systems/local_player_input.h works on player input (and against the traditional usage of ECS architecture, for simplicity, also works on the physics of the car and camera positioning).

In terms of rendering the world to the screen/back buffer, geometry is constructed purely of vertices (without indices) and a corresponding colour for that geometry (for simplicity's sake, as well as the limitations of the CPU, texturing, lighting and shadows are not implemented). The ECS basically has an engine system for rendering geometry, which it does so by inefficiently applying position transformations and rotations to the original vertices, adding these new 'render vertices' to a queue which gets handled in the EngineCore's run cycle after the ECS has handled all systems for that frame. This queue originally had the intention of being multi-threaded but remains single-threaded, the implementation can be found in /lib/engine/core/render_manager.cpp. From here the vertices are handled in batches of three, and a translation from 3D world to 2D screen space using the combined view projection matrix of the main camera (which is created as a component of a game entity and specified to the engine's instance of the renderer) is performed. From here, the 'primitive renderer' allows for triangle rasterization by forming edge equations from the transformed screen coordinates, a simple form of z buffering is implemented which is unsuccessful at times. SIMD AVX2 intrinsics are used as a modern work around for the CPU burden.

While the engine exposes some basic components and systems found in /include/core/base_components and /base_systems, it is up to the game to use these systems. Otherwise, game specific components and systems are implemented in include/game/components and /systems. The creation of entities happens in /lib/game/core/game.cpp, this file is particularly procedural rather than object oriented for simplicity, the game uses the engine directly creating verbose code, this could be solved by a wrapper for engine interaction.

The code is written solely for Windows as it heavily uses windows api. CMake is configured to use Clang++ but MSVC or GCC could also be used.

## Brief Documentation

This project uses a somewhat unconventional folder structure with src containing only main.cpp with the entry point, and /lib and /include for holding (mostly) implementation (.cpp) and declarations (.h) respectively. /lib for implementation was originally used to retain modular structure for the engine, but ends up holding both implementation for the engine and the game.

[/src/main.cpp](src/main.cpp)  
This contains the entry point and the creation of the EngineCore and Game instance. The resolution (both window resolution and rendering resolution) is configured, the game is setup and the engine starts running. 

```
#include <windows.h>
#include <iostream>

#include "engine/core/engine.h"
#include "game/core/game.h"

int main()
{
    EngineCore engine;
    Game game(&engine);

    try
    {
        WindowConfig window_config;
        window_config.window_name = L"Kario Mart 64";

        window_config.width = 320;
        window_config.height = 240;

        window_config.window_width = 1920;
        window_config.window_height = 1080;
        engine.configure_window(window_config);
        engine.initialize();

        game.setup();

        engine.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "An exception occured: " << e.what() << std::endl;
    }

    engine.shutdown();

    return 0;
}
```

[/lib/game/core/game.cpp](lib/game/core/game.cpp)  
This contains the implementation of the setting up of game logic, creation of entities including the player, the track, some item boxes, the notifying of the main camera to the engine, and all of the systems that will act on entities are registered here.

All game specific components and systems are declared or implemented in [/include/game/components](include/game/components) and [/include/game/systems](include/game/systems)
All engine basic components and systems are declared or implemented in [/include/engine/core/base_components](include/engine/core/base_components) and [/include/engine/core/base_systems](include/engine/core/base_systems)

[/include/engine/core/engine.h](include/engine/core/engine.h)  
[/lib/engine/core/engine.cpp](lib/engine/core/engine.cpp)  
This is the implementation of EngineCore, which holds the ECSController and provides the run method which is the main loop for both updating and rendering the game. It holds both RenderManager and Renderer for rendering.

Rendering  
[/include/engine/renderer/renderer.h](include/engine/renderer/renderer.h)  
This is the 'primitive' renderer with member functions to manipulate the back buffer, it also provides cycle_start and cycle_end which handle the render cycle, clearing the buffers and swapping the buffers respectively.

[/include/engine/core/render_manager.h](include/engine/core/render_manager.h)  
This is the RenderManager class which allows for the rendering of geometry by containing the main view projection matrix, applying transformations to geometry in the render queue and then using the primitive renderer for triangle rasterization.

[/include/engine/window_manager/window_manager.h](include/engine/window_manager/window_manager.h)  
The window manager works in tandem with the renderers to display the front buffer in the window's client area.


[/include/engine/math/](include/engine/math/)...  
provides basic classes for vectors and matrices.

There are some old files in the repo such as camera.h prior to the implementation of ECS.


![Game Screenshot 320x240.](https://github.com/benfrag/kario-mart/blob/main/game_screenshot.png?raw=true)

[![showcase on youtube](http://img.youtube.com/vi/GtnzE2LabZM/0.jpg)](http://www.youtube.com/watch?v=GtnzE2LabZM "Showcase")

Raw youtube link: [http://www.youtube.com/watch?v=GtnzE2LabZM](http://www.youtube.com/watch?v=GtnzE2LabZM)

320x240 used in source for fps.
