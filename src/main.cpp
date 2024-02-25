#include <windows.h>
#include <iostream>

#include "engine/core/engine.h"
#include "game/core/game.h"
//render system adds to triangle queue?
//triangle queue handles depth perception
//triangle queue handles world to screen
//render system performs transformationcomponent onto meshcomponent and sends world coordinates of mesh (triangle and indices) to queue.

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
        //window_config.width = 800;
        //window_config.height = 600;
//        window_config.width = 1920;
//        window_config.height = 1080;

        //window_config.window_width = 1920;
        //window_config.window_height = 1080;
        window_config.window_width = 1280;
        window_config.window_height = 960;
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
