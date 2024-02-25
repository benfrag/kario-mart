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
